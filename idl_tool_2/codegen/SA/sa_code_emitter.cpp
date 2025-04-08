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

#include <unistd.h>
#include <sys/stat.h>
#include "sa_code_emitter.h"
#include "ast/ast_map_type.h"
#include "ast/ast_orderedmap_type.h"
#include "ast/ast_set_type.h"
#include "type/sa_boolean_type_emitter.h"
#include "type/sa_byte_type_emitter.h"
#include "type/sa_short_type_emitter.h"
#include "type/sa_int_type_emitter.h"
#include "type/sa_long_type_emitter.h"
#include "type/sa_float_type_emitter.h"
#include "type/sa_double_type_emitter.h"
#include "type/sa_char_type_emitter.h"
#include "type/sa_cstring_type_emitter.h"
#include "type/sa_string_type_emitter.h"
#include "type/sa_u16string_type_emitter.h"
#include "type/sa_seq_type_emitter.h"
#include "type/sa_set_type_emitter.h"
#include "type/sa_rawdata_type_emitter.h"
#include "type/sa_interface_type_emitter.h"
#include "type/sa_map_type_emitter.h"
#include "type/sa_orderedmap_type_emitter.h"
#include "type/sa_array_type_emitter.h"
#include "type/sa_ptr_type_emitter.h"
#include "type/sa_uchar_type_emitter.h"
#include "type/sa_uint_type_emitter.h"
#include "type/sa_ulong_type_emitter.h"
#include "type/sa_ushort_type_emitter.h"
#include "type/sa_fd_type_emitter.h"
#include "type/sa_enum_type_emitter.h"
#include "type/sa_struct_type_emitter.h"
#include "type/sa_union_type_emitter.h"
#include "util/file.h"
#include "util/options.h"
#include "util/logger.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
SACodeEmitter::TypeEmitterMap SACodeEmitter::basicEmitters_ = {
    {TypeKind::TYPE_BOOLEAN,           new SaBooleanTypeEmitter()     },
    {TypeKind::TYPE_BYTE,              new SaByteTypeEmitter()        },
    {TypeKind::TYPE_SHORT,             new SaShortTypeEmitter()       },
    {TypeKind::TYPE_INT,               new SaIntTypeEmitter()         },
    {TypeKind::TYPE_LONG,              new SaLongTypeEmitter()        },
    {TypeKind::TYPE_FLOAT,             new SaFloatTypeEmitter()       },
    {TypeKind::TYPE_DOUBLE,            new SaDoubleTypeEmitter()      },
    {TypeKind::TYPE_CHAR,              new SaCharTypeEmitter()        },
    {TypeKind::TYPE_CSTRING,           new SaCStringTypeEmitter()     },
    {TypeKind::TYPE_STRING,            new SaStringTypeEmitter()      },
    {TypeKind::TYPE_U16STRING,         new SaU16stringTypeEmitter()   },
    {TypeKind::TYPE_UCHAR,             new SaUcharTypeEmitter()       },
    {TypeKind::TYPE_UINT,              new SaUintTypeEmitter()        },
    {TypeKind::TYPE_ULONG,             new SaUlongTypeEmitter()       },
    {TypeKind::TYPE_USHORT,            new SaUshortTypeEmitter()      },
    {TypeKind::TYPE_FILEDESCRIPTOR,    new SaFdTypeEmitter()          },
};

bool SACodeEmitter::OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if (!Reset(ast, targetDirectory, mode)) {
        return false;
    }

    EmitCode();
    return true;
}

bool SACodeEmitter::Reset(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if ((ast == nullptr) || targetDirectory.empty()) {
        return false;
    }

    CleanData();

    ast_ = ast;
    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == false) {
            interface_ = interface;
            break;
        }
    }
    if (interface_ != nullptr) {
        interfaceName_ = interface_->GetName();
        interfaceFullName_ = interface_->GetNamespace()->ToString() + interfaceName_;
        baseName_ = StringHelper::StartWith(interfaceName_, "I") ? interfaceName_.substr(1) : interfaceName_;
        proxyName_ = baseName_ + "Proxy";
        proxyFullName_ = interface_->GetNamespace()->ToString() + proxyName_;

        stubName_ = baseName_ + "Stub";
        stubFullName_ = interface_->GetNamespace()->ToString() + stubName_;

        clientName_ = baseName_ + "Client";
        clientFullName_ = interface_->GetNamespace()->ToString() + clientName_;
        deathRecipientName_ = StringHelper::StartWith(interfaceName_, "I") ? interfaceName_.substr(1) + "Recipient" :
            interfaceName_ + "Recipient";
    } else if (ast_->GetASTFileType() == ASTFileType::AST_TYPES) {
        baseName_ = ast_->GetName();
    }

    if (!ResolveDirectory(targetDirectory)) {
        return false;
    }

    return true;
}

void SACodeEmitter::CleanData()
{
    Options &options = Options::GetInstance();

    domainId_ = options.GetDomainId();
    logTag_ = options.GetLogTag();
    hitraceTag_ = options.GetGenerateHitraceTag();
    hitraceOn_ = options.DoHitraceState();
    logOn_ = options.DoLogOn();
    SaTypeEmitter::logOn_ = logOn_;
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
    clientName_ = "";
    clientFullName_ = "";
}

bool SACodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = targetDirectory;
#ifdef __MINGW32__
    if (targetDirectory.find(":\\") == std::string::npos) {
        char* cwd = getcwd(nullptr, 0);
        directory_ = StringHelper::Format("%s\\%s", cwd, targetDirectory.c_str());
        free(cwd);
    }
#else
    if (!StringHelper::StartWith(targetDirectory, "/")) {
        char* cwd = getcwd(nullptr, 0);
        directory_ = StringHelper::Format("%s/%s", cwd, targetDirectory.c_str());
        free(cwd);
    }
#endif

    if (!access(directory_.c_str(), R_OK | W_OK)) {
        return true;
    }

#ifdef __MINGW32__
    if (mkdir(directory_.c_str()) != 0) {
#else
    if (mkdir(directory_.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
#endif
        Logger::E(TAG, "Create \"%s\" directory failed.", directory_.c_str());
        return false;
    }

    return true;
}

AutoPtr<SaTypeEmitter> SACodeEmitter::GetTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaTypeEmitter> typeEmitter;
    auto basicTypePair = basicEmitters_.find(astType->GetTypeKind());
    if (basicTypePair != basicEmitters_.end()) {
        typeEmitter = (static_cast<SaTypeEmitter*>(basicTypePair->second.Get()));
    }

    if (typeEmitter == nullptr) {
        typeEmitter = NewTypeEmitter(astType);
    }

    if (astType->IsSequenceableType() || astType->IsInterfaceType() || astType->IsRawDataType()) {
        typeEmitter->SetTypeName(astType->ToShortString());
    } else if (astType->IsEnumType() || astType->IsStructType() || astType->IsUnionType()) {
        typeEmitter->SetTypeName(astType->GetName());
    } else {
        typeEmitter->SetTypeName(astType->ToString());
    }
    return typeEmitter;
}

std::string SACodeEmitter::GetNameWithNamespace(AutoPtr<ASTNamespace> space, const std::string name) const
{
    std::vector<std::string> namespaceVec = StringHelper::Split(space->ToString(), ".");
    std::vector<std::string> result;

    std::string rootPackage = Options::GetInstance().GetRootPackage(space->ToString());
    size_t rootPackageNum = StringHelper::Split(rootPackage, ".").size();

    for (size_t i = 0; i < namespaceVec.size(); i++) {
        std::string ns;
        if (i < rootPackageNum) {
            ns = StringHelper::StrToUpper(namespaceVec[i]);
        } else {
            ns = PascalName(namespaceVec[i]);
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

AutoPtr<SaTypeEmitter> SACodeEmitter::NewTypeEmitter(AutoPtr<ASTType> astType) const
{
    switch (astType->GetTypeKind()) {
        case TypeKind::TYPE_MAP:
            return NewMapTypeEmitter(astType);
        case TypeKind::TYPE_ORDEREDMAP:
            return NewOrderedMapTypeEmitter(astType);
        case TypeKind::TYPE_ARRAY:
            return NewArrayTypeEmitter(astType);
        case TypeKind::TYPE_LIST:
            return NewListTypeEmitter(astType);
        case TypeKind::TYPE_SET:
            return NewSetTypeEmitter(astType);
        case TypeKind::TYPE_SHAREDPTR:
        case TypeKind::TYPE_UNIQUEPTR:
        case TypeKind::TYPE_SPTR:
        case TypeKind::TYPE_NULL_SHAREDPTR:
        case TypeKind::TYPE_NULL_UNIQUEPTR:
        case TypeKind::TYPE_NULL_SPTR:
            return NewPtrTypeEmitter(astType, astType->GetTypeKind());
        case TypeKind::TYPE_ENUM:
            return NewEnumTypeEmitter(astType);
        case TypeKind::TYPE_STRUCT:
            return NewStructTypeEmitter(astType);
        case TypeKind::TYPE_UNION:
            return NewUnionTypeEmitter(astType);
        case TypeKind::TYPE_SEQUENCEABLE:
            return new SaSeqTypeEmitter();
        case TypeKind::TYPE_RAWDATA:
            return new SaRawDataTypeEmitter();
        case TypeKind::TYPE_INTERFACE:
            return new SaInterfaceTypeEmitter();
        default: {
            // type not match, new a empty emitter
            AutoPtr<SaTypeEmitter> typeEmitter = new SaTypeEmitter();
            return typeEmitter;
        }
    }
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewMapTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaMapTypeEmitter> mapTypeEmitter = new SaMapTypeEmitter();
    AutoPtr<ASTType> keyType = (static_cast<ASTMapType*>(astType.Get()))->GetKeyType();
    AutoPtr<ASTType> valueType = (static_cast<ASTMapType*>(astType.Get()))->GetValueType();
    AutoPtr<SaTypeEmitter> keyEmitter = GetTypeEmitter(keyType);
    AutoPtr<SaTypeEmitter> valueEmitter = GetTypeEmitter(valueType);
    mapTypeEmitter->SetKeyEmitter(keyEmitter);
    mapTypeEmitter->SetValueEmitter(valueEmitter);
    return mapTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewOrderedMapTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaOrderedMapTypeEmitter> orderedmapTypeEmitter = new SaOrderedMapTypeEmitter();
    AutoPtr<ASTType> keyType = (static_cast<ASTOrderedMapType*>(astType.Get()))->GetKeyType();
    AutoPtr<ASTType> valueType = (static_cast<ASTOrderedMapType*>(astType.Get()))->GetValueType();
    AutoPtr<SaTypeEmitter> keyEmitter = GetTypeEmitter(keyType);
    AutoPtr<SaTypeEmitter> valueEmitter = GetTypeEmitter(valueType);
    orderedmapTypeEmitter->SetKeyEmitter(keyEmitter);
    orderedmapTypeEmitter->SetValueEmitter(valueEmitter);
    return orderedmapTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewArrayTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaArrayTypeEmitter> arrayTypeEmitter = new SaArrayTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTArrayType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    arrayTypeEmitter->SetElementEmitter(elemEmitter);
    return arrayTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewListTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaListTypeEmitter> listTypeEmitter = new SaListTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTListType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    listTypeEmitter->SetElementEmitter(elemEmitter);
    return listTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewSetTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaSetTypeEmitter> setTypeEmitter = new SaSetTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTSetType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    setTypeEmitter->SetElementEmitter(elemEmitter);
    return setTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewPtrTypeEmitter(AutoPtr<ASTType> astType, TypeKind kind) const
{
    AutoPtr<SaPtrTypeEmitter> ptrTypeEmitter = new SaPtrTypeEmitter(kind);
    AutoPtr<ASTType> elemType = (static_cast<ASTPtrType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    ptrTypeEmitter->SetElementEmitter(elemEmitter);
    return ptrTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewEnumTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaEnumTypeEmitter> enumTypeEmitter = new SaEnumTypeEmitter();
    AutoPtr<ASTEnumType> enumType = static_cast<ASTEnumType*>(astType.Get());
    if (enumType->GetBaseType() != nullptr) {
        AutoPtr<SaTypeEmitter> baseTypeEmitter = GetTypeEmitter(enumType->GetBaseType());
        enumTypeEmitter->SetBaseTypeName(baseTypeEmitter->EmitCppType());
    }
    for (auto it : enumType->GetMembers()) {
        if (it->GetExprValue() == nullptr) {
            enumTypeEmitter->AddMember(new SaEnumValueEmitter(it->GetName(), std::string("")));
        } else {
            enumTypeEmitter->AddMember(new SaEnumValueEmitter(it->GetName(), it->GetExprValue()->EmitCode()));
        }
    }
    return enumTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewStructTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaStructTypeEmitter> structTypeEmitter = new SaStructTypeEmitter();
    AutoPtr<ASTStructType> structType = (static_cast<ASTStructType*>(astType.Get()));
    for (auto it : structType->GetMembers()) {
        structTypeEmitter->AddMember(std::get<0>(it), GetTypeEmitter(std::get<1>(it)));
    }
    return structTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewUnionTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaUnionTypeEmitter> unionTypeEmitter = new SaUnionTypeEmitter();
    AutoPtr<ASTUnionType> unionType = (static_cast<ASTUnionType*>(astType.Get()));
    for (size_t i = 0; i < unionType->GetMemberNumber(); i++) {
        unionTypeEmitter->AddMember(unionType->GetMemberName(i), GetTypeEmitter(unionType->GetMemberType(i)));
    }
    return unionTypeEmitter.Get();
}
} // namespace Idl
} // namespace OHOS
