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

#include "sa_cpp_code_emitter.h"

#include <regex>
#include <unordered_set>

#include "util/options.h"

#include "util/logger.h"

namespace OHOS {
namespace Idl {
void SACppCodeEmitter::GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    const AST::TypeStringMap &types = ast_->GetTypes();

    // Add header files dependency by default to avoid compatibility problems
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "string_ex");
    headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "cstdint");
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_ARRAY:
            case TypeKind::TYPE_LIST: {
                headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "vector");
                break;
            }
            case TypeKind::TYPE_MAP: {
                headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "unordered_map");
                break;
            }
            default:
                break;
        }
    }
}

void SACppCodeEmitter::EmitBeginNamespace(StringBuilder &sb) const
{
    std::string fullnamespace = GetNamespace(interface_->GetNamespace()->ToString());
    std::vector<std::string> namespaceVec = StringHelper::Split(fullnamespace, ".");

    for (const auto &nspace : namespaceVec) {
        sb.AppendFormat("namespace %s {\n", nspace.c_str());
    }
}

void SACppCodeEmitter::EmitEndNamespace(StringBuilder &sb) const
{
    std::string fullnamespace = GetNamespace(interface_->GetNamespace()->ToString());
    std::vector<std::string> namespaceVec = StringHelper::Split(fullnamespace, ".");

    for (std::vector<std::string>::const_reverse_iterator nspaceIter = namespaceVec.rbegin();
        nspaceIter != namespaceVec.rend(); ++nspaceIter) {
        sb.AppendFormat("} // namespace %s\n", nspaceIter->c_str());
    }
}

void SACppCodeEmitter::EmitWriteMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName,
    StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    typeEmitter->EmitCppWriteVar(parcelName, param->GetName(), sb, prefix);
}

void SACppCodeEmitter::EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName,
    bool emitType, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    typeEmitter->EmitCppReadVar(parcelName, param->GetName(), sb, prefix, emitType);
}

void SACppCodeEmitter::EmitInterfaceMethodParams(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    AutoPtr<ASTType> returnType = method->GetReturnType();
    TypeKind retTypeKind = returnType->GetTypeKind();
    int paramNumber = method->GetParameterNumber();

    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        sb.Append("\n").Append(prefix).AppendFormat(EmitCppParameter(param).c_str());
        if ((i != paramNumber - 1) || (retTypeKind != TypeKind::TYPE_VOID)) {
            sb.Append(",");
        }
    }
    if (retTypeKind != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        sb.Append("\n").Append(prefix).AppendFormat("%s result", typeEmitter->EmitCppType(TypeMode::PARAM_OUT).c_str());
    }
}

std::string SACppCodeEmitter::EmitCppParameter(AutoPtr<ASTParameter> &param) const
{
    std::string name = param->GetName();
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    ASTParamAttr::ParamAttr attrAttr = param->GetAttribute();
    if (attrAttr == ASTParamAttr::PARAM_INOUT) {
        return StringHelper::Format("%s %s", typeEmitter->EmitCppType(TypeMode::PARAM_INOUT).c_str(), name.c_str());
    } else if (attrAttr == ASTParamAttr::PARAM_IN) {
        return StringHelper::Format("%s %s", typeEmitter->EmitCppType(TypeMode::PARAM_IN).c_str(), name.c_str());
    } else if (attrAttr == ASTParamAttr::PARAM_OUT) {
        return StringHelper::Format("%s %s", typeEmitter->EmitCppType(TypeMode::PARAM_OUT).c_str(), name.c_str());
    }
    return StringHelper::Format("unknow param attr %s", name.c_str());
}

void SACppCodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    int methodNumber = interface_->GetMethodNumber();
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(prefix).AppendFormat("static constexpr int32_t COMMAND_%s = MIN_TRANSACTION_ID + %d;\n",
            ConstantName(method->GetName()).c_str(), i);
    }
}

} // namespace Idl
} // namespace OHOS