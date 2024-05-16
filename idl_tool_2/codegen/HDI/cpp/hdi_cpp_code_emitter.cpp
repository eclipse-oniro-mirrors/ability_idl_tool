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

#include "hdi_cpp_code_emitter.h"

#include <regex>
#include <unordered_set>

#include "util/options.h"

#include "util/logger.h"

namespace OHOS {
namespace Idl {
void HDICppCodeEmitter::GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        std::string fileName = (importAst->GetASTFileType() == ASTFileType::AST_SEQUENCEABLE) ?
            PackageToFilePath(importAst->GetName()) :
            PackageToFilePath(importAst->GetFullName());
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName);
    }
}

void HDICppCodeEmitter::EmitInterfaceMethodParameter(
    const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const
{
    std::string name = param->GetName();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    if (param->GetAttribute() == ASTParamAttr::PARAM_IN) {
        sb.Append(prefix).AppendFormat("%s %s", typeEmitter->EmitCppType(TypeMode::PARAM_IN).c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s %s", typeEmitter->EmitCppType(TypeMode::PARAM_OUT).c_str(), name.c_str());
    }
}

bool HDICppCodeEmitter::IsVersion(const std::string &name) const
{
    std::regex rVer("[V|v][0-9]+_[0-9]+");
    return std::regex_match(name.c_str(), rVer);
}

std::vector<std::string> HDICppCodeEmitter::EmitCppNameSpaceVec(const std::string &namespaceStr) const
{
    std::vector<std::string> result;
    std::vector<std::string> namespaceVec = StringHelper::Split(namespaceStr, ".");
    bool findVersion = false;

    std::string rootPackage = Options::GetInstance().GetRootPackage(namespaceStr);
    size_t rootPackageNum = StringHelper::Split(rootPackage, ".").size();

    for (size_t i = 0; i < namespaceVec.size(); i++) {
        std::string name;
        if (i < rootPackageNum) {
            name = StringHelper::StrToUpper(namespaceVec[i]);
        } else if (!findVersion && IsVersion(namespaceVec[i])) {
            name = StringHelper::Replace(namespaceVec[i], 'v', 'V');
            findVersion = true;
        } else {
            if (findVersion) {
                name = namespaceVec[i];
            } else {
                name = PascalName(namespaceVec[i]);
            }
        }

        result.emplace_back(name);
    }
    return result;
}

std::string HDICppCodeEmitter::EmitPackageToNameSpace(const std::string &packageName) const
{
    if (packageName.empty()) {
        return packageName;
    }

    StringBuilder nameSpaceStr;
    std::vector<std::string> namespaceVec = EmitCppNameSpaceVec(packageName);
    for (auto nameIter = namespaceVec.begin(); nameIter != namespaceVec.end(); nameIter++) {
        nameSpaceStr.Append(*nameIter);
        if (nameIter != namespaceVec.end() - 1) {
            nameSpaceStr.Append("::");
        }
    }

    return nameSpaceStr.ToString();
}

void HDICppCodeEmitter::GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles)
{
    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_STRING: {
                headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "string");
                break;
            }
            case TypeKind::TYPE_ARRAY:
            case TypeKind::TYPE_LIST: {
                headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "vector");
                break;
            }
            case TypeKind::TYPE_MAP: {
                headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "map");
                break;
            }
            case TypeKind::TYPE_SMQ: {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "base/hdi_smq");
                break;
            }
            case TypeKind::TYPE_ASHMEM: {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "ashmem");
                break;
            }
            case TypeKind::TYPE_NATIVE_BUFFER: {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "base/native_buffer");
                break;
            }
            default:
                break;
        }
    }
}

void HDICppCodeEmitter::EmitBeginNamespace(StringBuilder &sb)
{
    std::vector<std::string> cppNamespaceVec = EmitCppNameSpaceVec(interface_->GetNamespace()->ToString());
    for (const auto &nspace : cppNamespaceVec) {
        sb.AppendFormat("namespace %s {\n", nspace.c_str());
    }
}

void HDICppCodeEmitter::EmitEndNamespace(StringBuilder &sb)
{
    std::vector<std::string> cppNamespaceVec = EmitCppNameSpaceVec(interface_->GetNamespace()->ToString());

    for (std::vector<std::string>::const_reverse_iterator nspaceIter = cppNamespaceVec.rbegin();
        nspaceIter != cppNamespaceVec.rend(); ++nspaceIter) {
        sb.AppendFormat("} // %s\n", nspaceIter->c_str());
    }
}

void HDICppCodeEmitter::EmitUsingNamespace(StringBuilder &sb)
{
    sb.Append("using namespace OHOS;\n");
    sb.Append("using namespace OHOS::HDI;\n");
    EmitImportUsingNamespace(sb);
}

std::string HDICppCodeEmitter::EmitNamespace(const std::string &packageName) const
{
    if (packageName.empty()) {
        return packageName;
    }

    size_t index = packageName.rfind('.');
    return index != std::string::npos ? StringHelper::SubStr(packageName, 0, index) : packageName;
}

void HDICppCodeEmitter::EmitImportUsingNamespace(StringBuilder &sb)
{
    using StringSet = std::unordered_set<std::string>;
    StringSet namespaceSet;
    std::string selfNameSpace = EmitPackageToNameSpace(EmitNamespace(ast_->GetFullName()));

    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> import = importPair.second;
        std::string nameSpace = EmitPackageToNameSpace(EmitNamespace(import->GetFullName()));
        if (nameSpace == selfNameSpace) {
            continue;
        }
        namespaceSet.emplace(nameSpace);
    }

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_SMQ ||
            type->GetTypeKind() == TypeKind::TYPE_NATIVE_BUFFER) {
            namespaceSet.emplace("OHOS::HDI::Base");
            break;
        }
    }

    for (const auto &nspace : namespaceSet) {
        sb.Append("using namespace ").AppendFormat("%s;\n", nspace.c_str());
    }
}

void HDICppCodeEmitter::EmitWriteMethodParameter(const AutoPtr<ASTParameter> &param,
    const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    typeEmitter->EmitCppWriteVar(parcelName, param->GetName(), sb, prefix);
}

void HDICppCodeEmitter::EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, TypeMode mode,
    StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
    typeEmitter->EmitCppReadVar(param->GetName(), sb, prefix, mode);
}

std::string HDICppCodeEmitter::SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const
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

std::string HDICppCodeEmitter::EmitHeaderNameByInterface(AutoPtr<ASTInterfaceType> interface, const std::string &name)
{
    return StringHelper::Format(
        "v%u_%u/%s", interface->GetMajorVersion(), interface->GetMinorVersion(), FileName(name).c_str());
}

std::string HDICppCodeEmitter::EmitDefinitionByInterface(
    AutoPtr<ASTInterfaceType> interface, const std::string &name) const
{
    StringBuilder sb;
    std::vector<std::string> cppNamespaceVec = EmitCppNameSpaceVec(interface->GetNamespace()->ToString());
    for (const auto &nspace : cppNamespaceVec) {
        sb.AppendFormat("%s", nspace.c_str());
        sb.Append("::");
    }
    sb.Append(name.c_str());
    return sb.ToString();
}
} // namespace Idl
} // namespace OHOS