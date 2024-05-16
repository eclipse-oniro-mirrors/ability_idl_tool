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

#include "sa_ts_client_proxy_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
bool SaTsClientProxyCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = GetFileParentPath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("SaTsClientProxyCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void SaTsClientProxyCodeEmitter::EmitCode()
{
    if (!CheckInterfaceType()) {
        return;
    }
    EmitInterfaceProxyFile();
}

void SaTsClientProxyCodeEmitter::EmitInterfaceProxyFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.ts", directory_.c_str(), FileName(baseName_ + "Proxy").c_str()));
    File file(filePath, File::write_);
    StringBuilder sb;

    EmitLicense(sb);
    EmitInterfaceImports(sb);
    sb.Append("\n");
    EmitInterfaceProxyImpl(sb);
    sb.Append("\n");

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaTsClientProxyCodeEmitter::EmitInterfaceProxyImpl(StringBuilder &sb)
{
    sb.AppendFormat("export default class %s implements %s {\n", proxyName_.c_str(), interfaceName_.c_str());
    EmitInterfaceProxyConstructor(sb, TAB);
    EmitInterfaceProxyMethodImpls(sb, TAB);
    sb.Append("\n");
    EmitInterfaceMethodCommands(sb, TAB);
    sb.Append(TAB).Append("private proxy");
    sb.Append("\n");
    sb.Append("}\n");
}

void SaTsClientProxyCodeEmitter::EmitInterfaceProxyConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("constructor(proxy) {\n");
    sb.Append(prefix).Append(TAB).Append("this.proxy = proxy;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaTsClientProxyCodeEmitter::EmitInterfaceProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    int methodNumber = interface_->GetMethodNumber();
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethodHead(method, sb, prefix);
        sb.Append("): void\n");
        EmitInterfaceProxyMethodBody(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
}

void SaTsClientProxyCodeEmitter::EmitInterfaceProxyMethodBody(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    bool haveOutPara = false;
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix).Append(TAB).Append("let option = new rpc.MessageOption();\n");
    if (method->IsOneWay() || interface_->IsOneWay()) {
        sb.Append(prefix).Append(TAB).Append("option.setFlags(_option.TF_ASYNC);\n");
    }
    sb.Append(prefix).Append(TAB).Append("let dataSequence = rpc.MessageSequence.create();\n");
    sb.Append(prefix).Append(TAB).Append("let replySequence = rpc.MessageSequence.create();\n");

    int paramNumber = method->GetParameterNumber();
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            typeEmitter->EmitTsWriteVar("dataSequence", param->GetName(), sb, prefix + TAB);
        }
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            haveOutPara = true;
        }
    }
    sb.Append(prefix).Append(TAB).AppendFormat(
        "this.proxy.sendMessageRequest(%s.COMMAND_%s, dataSequence, replySequence, option).", proxyName_.c_str(),
        ConstantName(method->GetName()).c_str());
    sb.Append("then((result: rpc.RequestResult) => {\n");
    EmitInterfaceMethodCallback(method, sb, prefix + TAB + TAB, haveOutPara);
    sb.Append(prefix).Append(TAB).Append("}).catch((e: Error) => ").Append("{\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append(
        "console.log(\'sendMessageRequest failed, message: \' + e.message);\n");
    sb.Append(prefix).Append(TAB).Append("}).finally(() => ").Append("{\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append("dataSequence.reclaim();").Append("\n");
    sb.Append(prefix).Append(TAB).Append(TAB).Append("replySequence.reclaim();").Append("\n");
    sb.Append(prefix).Append(TAB).Append("});").Append("\n");
    sb.Append(prefix).Append("}\n");
}

void SaTsClientProxyCodeEmitter::EmitInterfaceMethodCallback(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool haveOutPara) const
{
    sb.Append(prefix).Append("if (result.errCode === 0) {\n");
    // emit errCode
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(new ASTIntegerType());
    typeEmitter->EmitTsReadVar("result.reply", SuffixAdded(ERR_CODE).c_str(), sb, prefix + TAB);
    EmitInterfaceMethodErrorCallback(method, sb, prefix + TAB, haveOutPara);
    EmitInterfaceMethodCallbackInner(method, sb, prefix);
}

void SaTsClientProxyCodeEmitter::EmitInterfaceMethodErrorCallback(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool haveOutPara) const
{
    TypeKind retKind = method->GetReturnType()->GetTypeKind();
    if ((retKind != TypeKind::TYPE_VOID) || haveOutPara) {
        sb.Append(prefix).AppendFormat("if (%s != 0) {\n", SuffixAdded(ERR_CODE).c_str());
        int paramNumber = method->GetParameterNumber();
        for (int i = 0; i < paramNumber; i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
                sb.Append(prefix).Append(TAB).AppendFormat("let %s = undefined;\n",
                    SuffixAdded(param->GetName()).c_str());
            }
        }
        if (retKind != TypeKind::TYPE_VOID) {
            sb.Append(prefix).Append(TAB).AppendFormat(
                "let %s = undefined;\n", SuffixAdded(RETURN_VALUE).c_str());
        }
        sb.Append(prefix).Append(TAB).AppendFormat("callback(%s", SuffixAdded(ERR_CODE).c_str());
        if (retKind != TypeKind::TYPE_VOID) {
            sb.AppendFormat(", %s", SuffixAdded(RETURN_VALUE).c_str());
        }
        for (int i = 0; i < paramNumber; i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
                sb.AppendFormat(", %s", SuffixAdded(param->GetName()).c_str());
            }
        }
        sb.Append(");\n");
        sb.Append(prefix).Append(TAB).Append("return;\n");
        sb.Append(prefix).Append("}\n");
    }
}

void SaTsClientProxyCodeEmitter::EmitInterfaceMethodCallbackInner(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    // emit return
    int paramNumber = method->GetParameterNumber();
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            typeEmitter->EmitTsReadVar("result.reply", SuffixAdded(param->GetName()), sb, prefix + TAB,
                TypeMode::PARAM_OUT);
        }
    }
    AutoPtr<ASTType> returnType = method->GetReturnType();
    if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        typeEmitter->EmitTsReadVar("result.reply", SuffixAdded(RETURN_VALUE), sb, prefix + TAB, TypeMode::PARAM_OUT);
    }
    sb.Append(prefix + TAB).AppendFormat("callback(%s", SuffixAdded(ERR_CODE).c_str());
    if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
        sb.AppendFormat(", %s", SuffixAdded(RETURN_VALUE).c_str());
    }
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            sb.AppendFormat(", %s", SuffixAdded(param->GetName()).c_str());
        }
    }
    sb.Append(");\n");
    sb.Append(prefix).Append("} else {\n");
    sb.Append(prefix).Append(TAB).Append("console.log(\"sendMessageRequest failed, errCode: \" + result.errCode);\n");
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS