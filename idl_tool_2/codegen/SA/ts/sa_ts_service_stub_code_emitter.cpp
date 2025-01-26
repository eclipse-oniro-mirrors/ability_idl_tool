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

#include "sa_ts_service_stub_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool SaTsServiceStubCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = GetFileParentPath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("SaTsServiceStubCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void SaTsServiceStubCodeEmitter::EmitCode()
{
    if (!CheckInterfaceType()) {
        return;
    }
    EmitInterfaceStub();
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStub()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.ts", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitInterfaceImports(sb);
    sb.Append("\n");
    EmitInterfaceStubImpl(sb);
    sb.Append("\n");

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStubImpl(StringBuilder &sb)
{
    sb.AppendFormat("export default class %s extends rpc.RemoteObject implements %s {\n", stubName_.c_str(),
        interfaceName_.c_str());
    EmitInterfaceStubMethodImpls(sb, TAB);
    sb.Append("\n");
    EmitInterfaceMethodCommands(sb, TAB);
    sb.Append("}\n");
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStubMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    EmitInterfaceStubConstructor(sb, prefix);
    sb.Append("\n");
    sb.Append(prefix).Append("async onRemoteMessageRequest(code: number, data:rpc.MessageSequence,");
    sb.Append(" reply:rpc.MessageSequence, option:rpc.MessageOption): Promise<boolean> {\n");
    sb.Append(prefix).Append(TAB).Append(
        "let localDescriptor = this.getDescriptor();\n");
    sb.Append(prefix).Append(TAB).Append(
        "let remoteDescriptor = data.readInterfaceToken();\n");
    sb.Append(prefix).Append(TAB).Append(
        "if (localDescriptor !== remoteDescriptor) {\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append(
        "console.log(\"invalid interfaceToken\");\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append("return false;\n");
    sb.Append(prefix).Append(TAB).Append("}\n");
    sb.Append(prefix).Append(TAB).Append(
        "console.log(\"onRemoteMessageRequest called, code = \" + code);\n");
    sb.Append(prefix).Append(TAB).Append("switch(code) {\n");
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceStubMethodImpl(method, sb, prefix + TAB + TAB);
    }
    sb.Append(prefix).Append(TAB).Append(TAB).Append("default: {\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append(TAB).Append(
        "console.log(\"invalid request code\" + code);\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append(TAB).Append("break;\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append("}\n");
    sb.Append(prefix).Append(TAB).Append("}\n");
    sb.Append(prefix).Append(TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    // emit empty method
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethodHead(method, sb, prefix);
        sb.Append("): void{}\n");
    }
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStubConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("constructor(des: string) {\n");
    sb.Append(prefix).Append(TAB).Append("super(des);\n");
    sb.Append(prefix).Append("}\n");
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStubMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    bool haveOutPara = false;
    sb.Append(prefix).AppendFormat("case %s.COMMAND_%s: {\n", stubName_.c_str(),
        ConstantName(method->GetName()).c_str());
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            typeEmitter->EmitTsReadVar("data", SuffixAdded(param->GetName()), sb, prefix + TAB, TypeMode::PARAM_IN);
        }
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            haveOutPara = true;
        }
    }
    sb.Append(prefix).Append(TAB).Append("let promise = new Promise<void>((resolve,reject) => { \n");
    sb.Append(prefix).Append(TAB).Append(TAB).AppendFormat("this.%s(", MethodName(method->GetName()).c_str());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            sb.AppendFormat("%s, ", SuffixAdded(param->GetName()).c_str());
        }
    }
    sb.Append("(");
    EmitInterfaceStubMethodPromiseImpl(method, sb, prefix + TAB + TAB, haveOutPara);
    sb.Append(prefix).Append(TAB).Append("});\n");
    sb.Append(prefix).Append(TAB).Append("await promise;\n");
    sb.Append(prefix).Append(TAB).Append("return true;\n");
    sb.Append(prefix).Append("}\n");
}

void SaTsServiceStubCodeEmitter::EmitInterfaceStubMethodPromiseImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool haveOutPara) const
{
    sb.Append(ERR_CODE);
    TypeKind retKind = method->GetReturnType()->GetTypeKind();
    if (retKind != TypeKind::TYPE_VOID) {
        sb.AppendFormat(", %s", RETURN_VALUE);
    }

    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            sb.AppendFormat(", %s", param->GetName().c_str());
        }
    }
    sb.Append(") => {\n");

    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(new ASTIntegerType());
    typeEmitter->EmitTsWriteVar("reply", ERR_CODE, sb, prefix + TAB);

    AutoPtr<ASTType> returnType = method->GetReturnType();
    if ((returnType->GetTypeKind() != TypeKind::TYPE_VOID) || haveOutPara) {
        sb.Append(prefix).Append(TAB).AppendFormat("if (%s == 0) {\n", ERR_CODE);
        if (haveOutPara) {
            EmitMethodPromiseOutParamImpl(method, sb, prefix);
        }

        if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
            typeEmitter = GetTypeEmitter(returnType);
            typeEmitter->EmitTsWriteVar("reply", RETURN_VALUE, sb, prefix + TAB + TAB);
        }
        sb.Append(prefix).Append(TAB).Append("}\n");
    }
    sb.Append(prefix).Append(TAB).Append("resolve();\n");
    sb.Append(prefix).Append("}");
    sb.Append(");\n");
}

void SaTsServiceStubCodeEmitter::EmitMethodPromiseOutParamImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param != nullptr && (param->GetAttribute() & ASTParamAttr::PARAM_OUT)) {
            GetTypeEmitter(param->GetType())->EmitTsWriteVar("reply", param->GetName(), sb, prefix + TAB + TAB);
        }
    }
}
} // namespace Idl
} // namespace OHOS
