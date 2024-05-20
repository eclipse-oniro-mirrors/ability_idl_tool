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

#include "sa_ts_interface_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
namespace {
    const std::string ERR_CODE_TYPE = "errCode: number";
}
bool SaTsInterfaceCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = GetFileParentPath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("SaTsInterfaceCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void SaTsInterfaceCodeEmitter::EmitCode()
{
    if (!CheckInterfaceType()) {
        return;
    }
    EmitInterfaceHeaderFile();
}

void SaTsInterfaceCodeEmitter::EmitInterfaceHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.ts", directory_.c_str(), FileName(interfaceName_).c_str()));
    File file(filePath, File::WRITE);

    StringBuilder sb;

    EmitLicense(sb);
    EmitInterfaceSelfDefinedTypeImports(sb);
    sb.Append("\n");
    EmitInterfaceDefinition(sb);
    sb.Append("\n");

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
    return;
}

void SaTsInterfaceCodeEmitter::EmitInterfaceDefinition(StringBuilder &sb) const
{
    sb.AppendFormat("export default interface %s {\n", interface_->GetName().c_str());
    int methodNumber = interface_->GetMethodNumber();
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethod(method, sb, TAB);
    }
    sb.Append("}\n");

    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethodCallback(method, sb);
    }
}

void SaTsInterfaceCodeEmitter::EmitInterfaceMethod(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s(", MethodName(method->GetName()).c_str());
    int paramNumber = method->GetParameterNumber();
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            sb.AppendFormat("%s: %s, ", param->GetName().c_str(), typeEmitter->EmitTsType().c_str());
        }
    }
    sb.AppendFormat("callback: %sCallback): void;\n", MethodName(method->GetName()).c_str());
}

void SaTsInterfaceCodeEmitter::EmitInterfaceMethodCallback(AutoPtr<ASTMethod> &method, StringBuilder &sb) const
{
    int paramNumber = method->GetParameterNumber();
    sb.AppendFormat("export type %sCallback = (%s", MethodName(method->GetName()).c_str(), ERR_CODE_TYPE.c_str());
    AutoPtr<ASTType> returnType = method->GetReturnType();
    if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        sb.AppendFormat(", %s: %s", RETURN_VALUE, typeEmitter->EmitTsType().c_str());
    }

    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            sb.AppendFormat(", %s: %s", param->GetName().c_str(), typeEmitter->EmitTsType().c_str());
        }
    }
    sb.Append(") => void;\n");
}
} // namespace Idl
} // namespace OHOS