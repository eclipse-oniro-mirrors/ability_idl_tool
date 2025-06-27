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

#include "hdi_c_code_emitter.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
void HDICCodeEmitter::GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->IsNativeBufferType()) {
            headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "base/buffer_util");
        }
    }
}

void HDICCodeEmitter::GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        std::string fileName = PackageToFilePath(importAst->GetFullName());
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName);
    }
}

void HDICCodeEmitter::EmitInterfaceMethodParameter(
    const AutoPtr<ASTParameter> &parameter, StringBuilder &sb, const std::string &prefix) const
{
    std::string name = parameter->GetName();
    AutoPtr<ASTType> type = parameter->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr) {
        return;
    }
    ASTParamAttr::ParamAttr attrAttr = parameter->GetAttribute();
    std::string typeName = (attrAttr == ASTParamAttr::PARAM_IN) ?
        typeEmitter->EmitCType(TypeMode::PARAM_IN) : typeEmitter->EmitCType(TypeMode::PARAM_OUT);

    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_ENUM:
        case TypeKind::TYPE_FILEDESCRIPTOR:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_NATIVE_BUFFER:
        case TypeKind::TYPE_POINTER:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST:
            sb.Append(prefix).AppendFormat("%s %s", typeName.c_str(), name.c_str());
            if ((type->GetTypeKind() == TypeKind::TYPE_STRING) && (attrAttr == ASTParamAttr::PARAM_OUT)) {
                sb.AppendFormat(", uint32_t %sLen", name.c_str());
            }
            if ((type->GetTypeKind() == TypeKind::TYPE_ARRAY) || (type->GetTypeKind() == TypeKind::TYPE_LIST)) {
                sb.AppendFormat(", uint32_t%s %sLen", (attrAttr == ASTParamAttr::PARAM_OUT) ? "*" : "", name.c_str());
            }
            break;
        default:
            sb.Append(prefix).AppendFormat("unknow type %s", name.c_str());
            break;
    }
}

void HDICCodeEmitter::EmitMethodNeedLoopVar(
    const AutoPtr<ASTMethod> &method, bool needRW, bool needFree, StringBuilder &sb, const std::string &prefix) const
{
    if (mode_ != GenMode::KERNEL) {
        return;
    }

    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (EmitNeedLoopVar(param->GetType(), needRW, needFree)) {
            sb.Append(prefix).Append("uint32_t i = 0;\n");
            break;
        }
    }
}

bool HDICCodeEmitter::EmitNeedLoopVar(const AutoPtr<ASTType> &type, bool needRW, bool needFree) const
{
    if (type == nullptr) {
        return false;
    }

    auto rwNeedLoopVar = [needRW](const AutoPtr<ASTType> &elementType) -> bool {
        if (!needRW) {
            return false;
        }

        if (elementType->IsPod()) {
            return elementType->IsBooleanType() ? true : false;
        }

        return elementType->IsStringType() ? false : true;
    };

    auto freeNeedLoopVar = [needFree](const AutoPtr<ASTType> &elementType) -> bool {
        if (!needFree) {
            return false;
        }
        return elementType->IsPod() ? false : true;
    };

    if (type->IsArrayType()) {
        AutoPtr<ASTArrayType> arrType = static_cast<ASTArrayType *>(type.Get());
        if (rwNeedLoopVar(arrType->GetElementType()) || freeNeedLoopVar(arrType->GetElementType())) {
            return true;
        }
    } else if (type->IsListType()) {
        AutoPtr<ASTListType> listType = static_cast<ASTListType *>(type.Get());
        if (rwNeedLoopVar(listType->GetElementType()) || freeNeedLoopVar(listType->GetElementType())) {
            return true;
        }
    }

    return false;
}

void HDICCodeEmitter::EmitHeadMacro(StringBuilder &sb, const std::string &fullName) const
{
    std::string macroName = MacroName(fullName);
    sb.Append("#ifndef ").Append(macroName).Append("\n");
    sb.Append("#define ").Append(macroName).Append("\n");
}

void HDICCodeEmitter::EmitTailMacro(StringBuilder &sb, const std::string &fullName) const
{
    std::string macroName = MacroName(fullName);
    sb.Append("#endif // ").Append(macroName);
}

void HDICCodeEmitter::EmitHeadExternC(StringBuilder &sb) const
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("extern \"C\" {\n");
    sb.Append("#endif /* __cplusplus */\n");
}

void HDICCodeEmitter::EmitTailExternC(StringBuilder &sb) const
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("}\n");
    sb.Append("#endif /* __cplusplus */\n");
}

std::string HDICCodeEmitter::MacroName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    std::string macro = StringHelper::StrToUpper(StringHelper::Replace(name, '.', '_')) + "_H";
    return macro;
}

std::string HDICCodeEmitter::SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const
{
    size_t maxLineLen = 120;
    size_t replaceLen = 2;
    std::string paramStr = paramSb.ToString();
    size_t preIndex = 0;
    size_t curIndex = 0;

    std::string insertStr = StringHelper::Format("\n%s", prefix.c_str());
    for (; curIndex < paramStr.size(); curIndex++) {
        if (curIndex == maxLineLen && preIndex > 0) {
            StringHelper::Replace(paramStr, preIndex, replaceLen, ",");
            paramStr.insert(preIndex + 1, insertStr);
        } else {
            if (paramStr[curIndex] == ',') {
                preIndex = curIndex;
            }
        }
    }
    return paramStr;
}

std::string HDICCodeEmitter::EmitDescMacroName() const
{
    return StringHelper::Format("%s_INTERFACE_DESC", StringHelper::StrToUpper(interface_->GetName()).c_str());
}

} // namespace Idl
} // namespace OHOS