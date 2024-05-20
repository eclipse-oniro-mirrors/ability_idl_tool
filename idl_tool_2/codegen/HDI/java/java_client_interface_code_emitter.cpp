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

#include "java_client_interface_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool JavaClientInterfaceCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFileParentPath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("JavaClientInterfaceCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void JavaClientInterfaceCodeEmitter::EmitCode()
{
    if (mode_ == GenMode::IPC) {
        EmitInterfaceFile();
    }
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.java", directory_.c_str(), FileName(interfaceName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;
    printf("%s/%s.java\n", directory_.c_str(), FileName(interfaceName_).c_str());
    EmitLicense(sb);
    EmitPackage(sb);
    sb.Append("\n");
    EmitInterfaceImports(sb);
    sb.Append("\n");
    EmitInterfaceDefinition(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceImports(StringBuilder &sb) const
{
    EmitInterfaceCorelibImports(sb);
    EmitInterfaceSelfDefinedTypeImports(sb);
    EmitInterfaceDBinderImports(sb);
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceCorelibImports(StringBuilder &sb) const
{
    bool includeList = false;
    bool includeMap = false;

    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_LIST: {
                if (!includeList) {
                    sb.Append("import java.util.List;\n");
                    includeList = true;
                }
                break;
            }
            case TypeKind::TYPE_MAP: {
                if (!includeMap) {
                    sb.Append("import java.util.Map;\n");
                    sb.Append("import java.util.HashMap;\n");
                    includeMap = true;
                }
                break;
            }
            default:
                break;
        }
    }
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceDBinderImports(StringBuilder &sb) const
{
    sb.Append("import ohos.rpc.IRemoteBroker;\n");
    sb.Append("import ohos.rpc.RemoteException;\n");
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceSelfDefinedTypeImports(StringBuilder &sb) const
{
    for (const auto &importPair : ast_->GetImports()) {
        AutoPtr<AST> import = importPair.second;
        sb.AppendFormat("import %s;\n", import->GetFullName().c_str());
    }
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceDefinition(StringBuilder &sb)
{
    sb.AppendFormat("public interface %s extends IRemoteBroker {\n", interface_->GetName().c_str());
    EmitInterfaceMethods(sb, TAB);
    sb.Append("}");
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceMethods(StringBuilder &sb, const std::string &prefix) const
{
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        EmitInterfaceMethod(method, sb, prefix);
        sb.Append("\n");
    }
}

void JavaClientInterfaceCodeEmitter::EmitInterfaceMethod(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat("int %s() throws RemoteException;\n", MethodName(method->GetName()).c_str());
    } else {
        StringBuilder paramStr;
        paramStr.Append(prefix).AppendFormat("int %s(", MethodName(method->GetName()).c_str());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.Append(") throws RemoteException;");
        sb.Append(SpecificationParam(paramStr, prefix + TAB));
        sb.Append("\n");
    }
}
} // namespace Idl
} // namespace OHOS
