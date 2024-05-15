/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hdi_code_emitter.h"
#include "type/hdi_boolean_type_emitter.h"
#include "type/hdi_byte_type_emitter.h"
#include "type/hdi_short_type_emitter.h"
#include "type/hdi_int_type_emitter.h"
#include "type/hdi_long_type_emitter.h"
#include "type/hdi_float_type_emitter.h"
#include "type/hdi_double_type_emitter.h"
#include "type/hdi_uchar_type_emitter.h"
#include "type/hdi_ushort_type_emitter.h"
#include "type/hdi_uint_type_emitter.h"
#include "type/hdi_ulong_type_emitter.h"
#include "type/hdi_string_type_emitter.h"
#include "type/hdi_fd_type_emitter.h"
#include "type/hdi_seq_type_emitter.h"
#include "type/hdi_interface_type_emitter.h"
#include "type/hdi_map_type_emitter.h"
#include "type/hdi_array_type_emitter.h"
#include "type/hdi_enum_type_emitter.h"
#include "type/hdi_struct_type_emitter.h"
#include "type/hdi_union_type_emitter.h"
#include "type/hdi_smq_type_emitter.h"
#include "type/hdi_native_buffer_type_emitter.h"
#include "type/hdi_pointer_type_emitter.h"

#include <cctype>
#include "util/file.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
HDICodeEmitter::TypeEmitterMap HDICodeEmitter::basicEmitters_ = {
    {TypeKind::TYPE_BOOLEAN,           new HdiBooleanTypeEmitter()         },
    {TypeKind::TYPE_BYTE,              new HdiByteTypeEmitter()            },
    {TypeKind::TYPE_SHORT,             new HdiShortTypeEmitter()           },
    {TypeKind::TYPE_INT,               new HdiIntTypeEmitter()             },
    {TypeKind::TYPE_LONG,              new HdiLongTypeEmitter()            },
    {TypeKind::TYPE_FLOAT,             new HdiFloatTypeEmitter()           },
    {TypeKind::TYPE_DOUBLE,            new HdiDoubleTypeEmitter()          },
    {TypeKind::TYPE_UCHAR,             new HdiUcharTypeEmitter()           },
    {TypeKind::TYPE_USHORT,            new HdiUshortTypeEmitter()          },
    {TypeKind::TYPE_UINT,              new HdiUintTypeEmitter()            },
    {TypeKind::TYPE_ULONG,             new HdiUlongTypeEmitter()           },
    {TypeKind::TYPE_STRING,            new HdiStringTypeEmitter()          },
    {TypeKind::TYPE_FILEDESCRIPTOR,    new HdiFdTypeEmitter()              },
    {TypeKind::TYPE_ASHMEM,            new HdiAshmemTypeEmitter()          },
    {TypeKind::TYPE_NATIVE_BUFFER,     new HdiNativeBufferTypeEmitter()    },
    {TypeKind::TYPE_POINTER,           new HdiPointerTypeEmitter()         },
};

bool HDICodeEmitter::OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if (!Reset(ast, targetDirectory, mode)) {
        return false;
    }

    EmitCode();
    return true;
}

bool HDICodeEmitter::Reset(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if (ast == nullptr || targetDirectory.empty()) {
        return false;
    }

    CleanData();

    mode_ = mode;
    ast_ = ast;
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        interface_ = ast_->GetInterfaceDef();
        interfaceName_ = interface_->GetName();
        std::string nameSpace = interface_->GetNamespace()->ToString();
        interfaceFullName_ = nameSpace + interfaceName_;
        baseName_ = StringHelper::StartWith(interfaceName_, "I") ? interfaceName_.substr(1) : interfaceName_;
        proxyName_ = baseName_ + "Proxy";
        proxyFullName_ = nameSpace + proxyName_;

        stubName_ = baseName_ + "Stub";
        stubFullName_ = nameSpace + stubName_;

        implName_ = baseName_ + "Service";
        implFullName_ = nameSpace + implName_;
    } else if (ast_->GetASTFileType() == ASTFileType::AST_TYPES) {
        baseName_ = ast_->GetName();
    } else if (ast_->GetASTFileType() == ASTFileType::AST_SEQUENCEABLE) {
        baseName_ = ast_->GetName();
    }

    majorVerName_ = StringHelper::Format("%s_MAJOR_VERSION", ConstantName(interfaceName_).c_str());
    minorVerName_ = StringHelper::Format("%s_MINOR_VERSION", ConstantName(interfaceName_).c_str());

    std::string prefix = StringHelper::Format("%c%s", tolower(baseName_[0]), baseName_.substr(1).c_str());
    HdiTypeEmitter::dataParcelName_ = prefix + "Data";
    HdiTypeEmitter::replyParcelName_ = prefix + "Reply";
    optionName_ = prefix + "Option";
    HdiTypeEmitter::errorCodeName_ = prefix + "Ret";
    flagOfSetMemName_ = prefix + "MemSet";

    if (!ResolveDirectory(targetDirectory)) {
        return false;
    }

    return true;
}

void HDICodeEmitter::CleanData()
{
    ast_ = nullptr;
    interface_ = nullptr;
    directory_ = "";
    interfaceName_ = "";
    interfaceFullName_ = "";
    baseName_ = "";
    proxyName_ = "";
    proxyFullName_ = "";
    stubName_ = "";
    stubFullName_ = "";
    implName_ = "";
    implFullName_ = "";
    HdiTypeEmitter::dataParcelName_ = "";
    HdiTypeEmitter::replyParcelName_ = "";
    optionName_ = "";
    HdiTypeEmitter::errorCodeName_ = "";
}

std::string HDICodeEmitter::GetNameWithNamespace(AutoPtr<ASTNamespace> space, std::string name) const
{
    std::vector<std::string> namespaceVec = StringHelper::Split(space->ToString(), ".");
    std::regex rVer("[V|v][0-9]+_[0-9]+");
    std::vector<std::string> result;
    bool findVersion = false;

    std::string rootPackage = Options::GetInstance().GetRootPackage(space->ToString());
    size_t rootPackageNum = StringHelper::Split(rootPackage, ".").size();

    for (size_t i = 0; i < namespaceVec.size(); i++) {
        std::string ns;
        if (i < rootPackageNum) {
            ns = StringHelper::StrToUpper(namespaceVec[i]);
        } else if (!findVersion && std::regex_match(namespaceVec[i].c_str(), rVer)) {
            ns = StringHelper::Replace(namespaceVec[i], 'v', 'V');
            findVersion = true;
        } else {
            if (findVersion) {
                ns = namespaceVec[i];
            } else {
                ns = PascalName(namespaceVec[i]);
            }
        }

        result.emplace_back(ns);
    }
    StringBuilder sb;
    for (const auto &ns : result) {
        sb.AppendFormat("%s::", ns.c_str());
    }
    sb.Append(name);
    return sb.ToString();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::GetTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiTypeEmitter> typeEmitter;
    auto basicTypePair = basicEmitters_.find(astType->GetTypeKind());
    if (basicTypePair != basicEmitters_.end()) {
        typeEmitter = (static_cast<HdiTypeEmitter*>(basicTypePair->second.Get()));
    }

    if (typeEmitter == nullptr) {
        typeEmitter = NewTypeEmitter(astType);
    }

    typeEmitter->SetName(astType->GetName());
    typeEmitter->SetPod(astType->IsPod());
    if (astType->IsSequenceableType() || astType->IsInterfaceType() ||
        astType->IsEnumType() || astType->IsStructType() || astType->IsUnionType()) {
        typeEmitter->SetTypeName(GetNameWithNamespace(astType->GetNamespace(), astType->GetName()));
    } else {
        typeEmitter->SetTypeName(astType->ToString());
    }

    return typeEmitter;
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewTypeEmitter(AutoPtr<ASTType> astType) const
{
    switch (astType->GetTypeKind()) {
        case TypeKind::TYPE_MAP:
            return NewMapTypeEmitter(astType);
        case TypeKind::TYPE_ARRAY:
            return NewArrayTypeEmitter(astType);
        case TypeKind::TYPE_LIST:
            return NewListTypeEmitter(astType);
        case TypeKind::TYPE_ENUM:
            return NewEnumTypeEmitter(astType);
        case TypeKind::TYPE_STRUCT:
            return NewStructTypeEmitter(astType);
        case TypeKind::TYPE_UNION:
            return NewUnionTypeEmitter(astType);
        case TypeKind::TYPE_SMQ:
            return NewSmqTypeEmitter(astType);
        case TypeKind::TYPE_SEQUENCEABLE:
            return new HdiSeqTypeEmitter();
        case TypeKind::TYPE_INTERFACE:
            return new HdiInterfaceTypeEmitter();
        default: {
            // type not match, new a empty emitter
            AutoPtr<HdiTypeEmitter> typeEmitter = new HdiTypeEmitter();
            return typeEmitter;
        }
    }
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewMapTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiMapTypeEmitter> mapTypeEmitter = new HdiMapTypeEmitter();
    AutoPtr<ASTType> keyType = (static_cast<ASTMapType*>(astType.Get()))->GetKeyType();
    AutoPtr<ASTType> valueType = (static_cast<ASTMapType*>(astType.Get()))->GetValueType();
    AutoPtr<HdiTypeEmitter> keyEmitter = GetTypeEmitter(keyType);
    AutoPtr<HdiTypeEmitter> valueEmitter = GetTypeEmitter(valueType);
    mapTypeEmitter->SetKeyEmitter(keyEmitter);
    mapTypeEmitter->SetValueEmitter(valueEmitter);
    return mapTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewArrayTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiArrayTypeEmitter> arrayTypeEmitter = new HdiArrayTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTArrayType*>(astType.Get()))->GetElementType();
    AutoPtr<HdiTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    arrayTypeEmitter->SetElementEmitter(elemEmitter);
    return arrayTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewListTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiListTypeEmitter> listTypeEmitter = new HdiListTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTListType*>(astType.Get()))->GetElementType();
    AutoPtr<HdiTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    listTypeEmitter->SetElementEmitter(elemEmitter);
    return listTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewEnumTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiEnumTypeEmitter> enumTypeEmitter = new HdiEnumTypeEmitter();
    AutoPtr<ASTEnumType> enumType = (static_cast<ASTEnumType*>(astType.Get()));
    if (enumType->GetBaseType() != nullptr) {
        AutoPtr<HdiTypeEmitter> baseTypeEmitter = GetTypeEmitter(enumType->GetBaseType());
        enumTypeEmitter->SetBaseTypeName(baseTypeEmitter->EmitCppType());
    }
    for (auto it : enumType->GetMembers()) {
        if (it->GetExprValue() == nullptr) {
            enumTypeEmitter->AddMember(new HdiEnumValueEmitter(it->GetName(), std::string("")));
        } else {
            enumTypeEmitter->AddMember(new HdiEnumValueEmitter(it->GetName(), it->GetExprValue()->EmitCode()));
        }
    }
    return enumTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewStructTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiStructTypeEmitter> structTypeEmitter = new HdiStructTypeEmitter();
    AutoPtr<ASTStructType> structType = (static_cast<ASTStructType*>(astType.Get()));
    for (auto it : structType->GetMembers()) {
        structTypeEmitter->AddMember(std::get<0>(it), GetTypeEmitter(std::get<1>(it)));
    }
    return structTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewUnionTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiUnionTypeEmitter> unionTypeEmitter = new HdiUnionTypeEmitter();
    AutoPtr<ASTUnionType> unionType = (static_cast<ASTUnionType*>(astType.Get()));
    for (size_t i = 0; i < unionType->GetMemberNumber(); i++) {
        unionTypeEmitter->AddMember(unionType->GetMemberName(i), GetTypeEmitter(unionType->GetMemberType(i)));
    }
    return unionTypeEmitter.Get();
}

AutoPtr<HdiTypeEmitter> HDICodeEmitter::NewSmqTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<HdiSmqTypeEmitter> smqTypeEmitter = new HdiSmqTypeEmitter();
    AutoPtr<ASTType> innerType = (static_cast<ASTSmqType*>(astType.Get()))->GetInnerType();
    AutoPtr<HdiTypeEmitter> innerTypeEmitter = GetTypeEmitter(innerType);
    smqTypeEmitter->SetInnerTypeEmitter(innerTypeEmitter);
    return smqTypeEmitter.Get();
}

void HDICodeEmitter::EmitUtilMethods(StringBuilder &sb, bool isDecl)
{
    (void)sb;
    (void)isDecl;
}

void HDICodeEmitter::EmitUtilMethodMap(StringBuilder &sb, const UtilMethodMap &methods)
{
    // generator util methods
    for (const auto &methodPair : methods) {
        sb.Append(methodPair.second);
    }
}

void HDICodeEmitter::EmitInterfaceBuffSizeMacro(StringBuilder &sb) const
{
    sb.AppendFormat("#ifndef %s\n", MAX_BUFF_SIZE_MACRO);
    sb.AppendFormat("#define %s (%s)\n", MAX_BUFF_SIZE_MACRO, MAX_BUFF_SIZE_VALUE);
    sb.Append("#endif\n\n");

    sb.AppendFormat("#ifndef %s\n", CHECK_VALUE_RETURN_MACRO);
    sb.AppendFormat("#define %s(lv, compare, rv, ret) do { \\\n", CHECK_VALUE_RETURN_MACRO);
    sb.Append(TAB).Append("if ((lv) compare (rv)) { \\\n");
    sb.Append(TAB).Append(TAB).Append("return ret; \\\n");
    sb.Append(TAB).Append("} \\\n");
    sb.Append("} while (false)\n");
    sb.Append("#endif\n\n");

    sb.AppendFormat("#ifndef %s\n", CHECK_VALUE_RET_GOTO_MACRO);
    sb.AppendFormat("#define %s(lv, compare, rv, ret, value, table) do { \\\n", CHECK_VALUE_RET_GOTO_MACRO);
    sb.Append(TAB).Append("if ((lv) compare (rv)) { \\\n");
    sb.Append(TAB).Append(TAB).Append("ret = value; \\\n");
    sb.Append(TAB).Append(TAB).Append("goto table; \\\n");
    sb.Append(TAB).Append("} \\\n");
    sb.Append("} while (false)\n");
    sb.Append("#endif\n");
}
} // namespace Idl
} // namespace OHOS
