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
void SACppCodeEmitter::GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (const auto &importName : ast_->GetImportNames()) {
        size_t index = importName.rfind(SEPARATOR);
        if (index == std::string::npos) {
            index = -1;
        }
        std::string fileName = importName.substr(index + 1);
        std::string relativePath = importName.substr(0, index + 1) + FileName(fileName).c_str();
        std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, relativePath);
    }
}

void SACppCodeEmitter::EmitImportUsingNamespace(StringBuilder &sb) const
{
    std::unordered_map<std::string, int> usedNamespace;
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        if (importAst->GetASTFileType() == ASTFileType::AST_SEQUENCEABLE ||
            importAst->GetASTFileType() == ASTFileType::AST_RAWDATA) {
            continue;
        }
        size_t index = importPair.first.rfind(".");
        std::string usingName = importPair.first.substr(0, index);
        if (usingName.empty()) {
            continue;
        }
        std::regex dotToColons("\\.");
        usingName = std::regex_replace(usingName, dotToColons, "::");
        if (usedNamespace[usingName] == 0) {
            usedNamespace[usingName] = 1;
            sb.Append("using namespace ").AppendFormat("%s;\n", usingName.c_str());
        }
    }
    sb.Append("\n");
}

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
    if (typeEmitter != nullptr) {
        typeEmitter->isParamInout = param->GetAttribute() == ASTParamAttr::PARAM_INOUT;
        typeEmitter->isProxy = isProxy;
        typeEmitter->EmitCppWriteVar(parcelName, param->GetName(), sb, prefix);
    }
}

void SACppCodeEmitter::EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName,
    bool emitType, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    if (typeEmitter != nullptr) {
        typeEmitter->EmitCppReadVar(parcelName, param->GetName(), sb, prefix, emitType);
    }
}

void SACppCodeEmitter::EmitInterfaceMethodParams(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    EmitParamsWithReturnType(method, sb, prefix, true);
}

void SACppCodeEmitter::EmitInterfaceClientMethodParams(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool includeValue) const
{
    EmitParamsWithReturnType(method, sb, prefix, true);
    if (!method->IsOneWay()) {
        int paramNumber = static_cast<int>(method->GetParameterNumber());
        if (paramNumber == 0) {
            sb.Append("int32_t timeout");
        } else {
            sb.Append(",\n").Append(prefix).Append("int32_t timeout");
        }
        if (includeValue) {
            sb.Append(" = LOAD_SA_TIMEOUT");
        }
    }
}

void SACppCodeEmitter::EmitInterfaceClientMethodParamsWithoutType(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    EmitParamsWithReturnType(method, sb, prefix, false);
}

void SACppCodeEmitter::EmitParamsWithReturnType(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool includeType) const
{
    AutoPtr<ASTType> returnType = method->GetReturnType();
    TypeKind retTypeKind = returnType->GetTypeKind();
    int paramNumber = static_cast<int>(method->GetParameterNumber());

    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param == nullptr) {
            return;
        }
        sb.Append("\n").Append(prefix);
        if (includeType) {
            sb.AppendFormat(EmitCppParameter(param).c_str());
        } else {
            sb.AppendFormat(param->GetName().c_str());
        }
        if ((i != paramNumber - 1) || (retTypeKind != TypeKind::TYPE_VOID)) {
            sb.Append(",");
        }
    }
    if (retTypeKind != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter != nullptr) {
            sb.Append("\n").Append(prefix).AppendFormat(includeType ? "%s funcResult" : "funcResult",
                typeEmitter->EmitCppType(TypeMode::PARAM_OUT).c_str());
        }
    }
}

std::string SACppCodeEmitter::EmitCppParameter(AutoPtr<ASTParameter> &param) const
{
    std::string name = param->GetName();
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    if (typeEmitter == nullptr) {
        return StringHelper::Format("unknow param attr %s", name.c_str());
    }
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

void SACppCodeEmitter::EmitSecurecInclusion(StringBuilder &sb) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        size_t paramNumber = method->GetParameterNumber();
        for (size_t j = 0; j < paramNumber; j++) {
            AutoPtr<ASTParameter> param = method->GetParameter(j);
            TypeKind paramTypeKind = param->GetType()->GetTypeKind();
            if (paramTypeKind == TypeKind::TYPE_UNION || paramTypeKind == TypeKind::TYPE_STRUCT) {
                sb.Append("#include <securec.h>\n");
                return;
            }
        }

        AutoPtr<ASTType> returnType = method->GetReturnType();
        TypeKind retTypeKind = returnType->GetTypeKind();
        if (retTypeKind == TypeKind::TYPE_UNION || retTypeKind == TypeKind::TYPE_STRUCT) {
            sb.Append("#include <securec.h>\n");
            return;
        }
    }
}

void SACppCodeEmitter::GetOverloadName(AutoPtr<ASTMethod> &method, std::string &overloadname) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());

    for (size_t i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        ASTParamAttr::ParamAttr attrAttr = param->GetAttribute();
        if (attrAttr == ASTParamAttr::PARAM_INOUT) {
            overloadname += "_inout_";
        } else if (attrAttr == ASTParamAttr::PARAM_IN) {
            overloadname += "_in_";
        } else if (attrAttr == ASTParamAttr::PARAM_OUT) {
            overloadname += "_out_";
        }
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
        if (typeEmitter == nullptr) {
            return;
        }
        overloadname += typeEmitter->GetTypeName();
    }

    AutoPtr<ASTType> returnType = method->GetReturnType();
    TypeKind retTypeKind = returnType->GetTypeKind();
    if (retTypeKind != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter == nullptr) {
            return;
        }
        overloadname += "_out_" + typeEmitter->GetTypeName();
    }

    std::transform(overloadname.begin(), overloadname.end(), overloadname.begin(), ::tolower);
    overloadname = std::regex_replace(overloadname, std::regex("\\b[a-zA-Z]+\\."), "");
    overloadname = std::regex_replace(overloadname, std::regex("[ <]"), "_");
    overloadname = std::regex_replace(overloadname, std::regex("\\[]"), "_vector");
    overloadname = std::regex_replace(overloadname, std::regex("[,>]"), "");
}

void SACppCodeEmitter::CheckMethodOverload(AutoPtr<ASTMethod> &method, size_t &index, std::string &overloadname) const
{
    for (int i = 0; i < index; i++) {
        if (interface_->GetMethod(i)->GetName() == method->GetName()) {
            GetOverloadName(method, overloadname);
            break;
        }
    }
}

void SACppCodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    sb.AppendFormat("enum class %sIpcCode {\n", interface_->GetName().c_str());
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        std::string overloadname = "";
        CheckMethodOverload(method, i, overloadname);
        std::string commandCode = "COMMAND_" + ConstantName(method->GetName() + overloadname);
        bool hasIpcCode = method->HasIpcCode();
        if (i == 0 && !hasIpcCode) {
            commandCode += " = MIN_TRANSACTION_ID";
        } else if (hasIpcCode) {
            commandCode += " = " + method->GetIpcCodeStr();
        }
        commandCode += ",\n";
        sb.Append(prefix).AppendFormat(commandCode.c_str());
    }
    sb.Append("};\n\n");
}

} // namespace Idl
} // namespace OHOS