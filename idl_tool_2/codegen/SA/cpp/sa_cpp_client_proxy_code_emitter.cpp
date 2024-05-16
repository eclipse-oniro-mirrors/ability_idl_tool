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

#include "sa_cpp_client_proxy_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
void SaCppClientProxyCodeEmitter::EmitCode()
{
    EmitInterfaceProxyHeaderFile();
    EmitInterfaceProxyCppFile();
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(baseName_ + "Proxy").c_str()));
    File file(filePath, File::write_);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, proxyFullName_);
    sb.Append("\n");
    sb.AppendFormat("#include \"%s.h\"\n", FileName(interfaceName_).c_str());
    sb.Append("#include <iremote_proxy.h>\n");
    sb.Append("\n");
    EmitInterfaceProxyInHeaderFile(sb);
    EmitTailMacro(sb, proxyFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyInHeaderFile(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    sb.AppendFormat("class %s : public IRemoteProxy<%s> {\n", proxyName_.c_str(), interfaceName_.c_str());
    sb.Append("public:\n");
    EmitInterfaceProxyConstructor(sb, TAB);
    sb.Append("\n");
    EmitInterfaceProxyMethodDecls(sb, TAB);
    sb.Append("\n");
    sb.Append("private:\n");
    EmitInterfaceProxyConstants(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("explicit %s(\n", proxyName_.c_str());
    sb.Append(prefix + TAB).Append("const sptr<IRemoteObject>& remote)\n");
    sb.Append(prefix + TAB).AppendFormat(": IRemoteProxy<%s>(remote)\n", interfaceName_.c_str());
    sb.Append(prefix).Append("{}\n");
    sb.Append("\n");
    sb.Append(prefix).AppendFormat("virtual ~%s()\n", proxyName_.c_str());
    sb.Append(prefix).Append("{}\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    int methodNumber = interface_->GetMethodNumber();
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceProxyMethodDecl(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodDecl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("ErrCode %s(", method->GetName().c_str());
    EmitInterfaceMethodParams(method, sb, prefix + TAB);
    sb.Append(") override;\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstants(StringBuilder &sb, const std::string &prefix)
{
    EmitInterfaceMethodCommands(sb, prefix);
    sb.Append("\n");
    sb.Append(prefix).AppendFormat("static inline BrokerDelegator<%s> delegator_;\n", proxyName_.c_str());
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyCppFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(proxyName_).c_str()));
    File file(filePath, File::write_);
    StringBuilder sb;

    EmitLicense(sb);
    sb.AppendFormat("#include \"%s.h\"\n", FileName(proxyName_).c_str());
    if (logOn_) {
        sb.Append("#include \"hilog/log.h\"\n");
    }
    if (hitraceOn_) {
        sb.Append("#include \"hitrace_meter.h\"\n");
    }
    sb.Append("\n");
    if (logOn_) {
        sb.Append("using OHOS::HiviewDFX::HiLog;\n\n");
    }
    EmitBeginNamespace(sb);
    EmitInterfaceProxyMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    int methodNumber = interface_->GetMethodNumber();
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceProxyMethodImpl(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("ErrCode %s::%s(", proxyName_.c_str(), method->GetName().c_str());
    EmitInterfaceMethodParams(method, sb, prefix + TAB);
    sb.Append(")\n");
    EmitInterfaceProxyMethodBody(method, sb, prefix);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodBody(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).Append("{\n");
    if (hitraceOn_) {
        sb.Append(prefix + TAB).AppendFormat("HITRACE_METER_NAME(%s, __PRETTY_FUNCTION__);\n",
            hitraceTag_.c_str());
    }
    sb.Append(prefix + TAB).Append("MessageParcel data;\n");
    sb.Append(prefix + TAB).Append("MessageParcel reply;\n");
    sb.Append(prefix + TAB).AppendFormat("MessageOption option(%s);\n",
        method->IsOneWay() ? "MessageOption::TF_ASYNC" : "MessageOption::TF_SYNC");
    sb.Append("\n");
    sb.Append(prefix + TAB).Append("if (!data.WriteInterfaceToken(GetDescriptor())) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append(TAB).Append("HiLog::Error(LABEL, \"Write interface token failed!\");\n");
    }
    sb.Append(prefix + TAB).Append(TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("\n");

    int paramNumber = method->GetParameterNumber();
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            EmitWriteMethodParameter(param, "data.", sb, prefix + TAB);
        }
    }
    sb.Append("\n");
    sb.Append(prefix + TAB).Append("sptr<IRemoteObject> remote = Remote();\n");
    sb.Append(prefix + TAB).Append("if (remote == nullptr) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append(TAB).Append("HiLog::Error(LABEL, \"Remote is nullptr!\");\n");
    }
    sb.Append(prefix + TAB).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t result = remote->SendRequest(COMMAND_%s, data, reply, option);\n",
        ConstantName(method->GetName()).c_str());
    sb.Append(prefix + TAB).Append("if (FAILED(result)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append(TAB).Append("HiLog::Error(LABEL, \"Send request failed!\");\n");
    }
    sb.Append(prefix + TAB).Append("    return result;\n");
    sb.Append(prefix + TAB).Append("}\n");
    EmitInterfaceProxyMethodRetValue(method, sb, prefix);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodRetValue(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    if (!method->IsOneWay()) {
        sb.Append("\n");
        sb.Append(prefix + TAB).Append("ErrCode errCode = reply.ReadInt32();\n");
        sb.Append(prefix + TAB).Append("if (FAILED(errCode)) {\n");
        if (logOn_) {
            sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Read Int32 failed!\");\n");
        }
        sb.Append(prefix + TAB).Append("    return errCode;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append("\n");
        int paramNumber = method->GetParameterNumber();
        for (int i = 0; i < paramNumber; i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
                EmitReadMethodParameter(param, "reply.", false, sb, prefix + TAB);
            }
        }
        AutoPtr<ASTType> returnType = method->GetReturnType();
        if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
            typeEmitter->EmitCppReadVar("reply.", "result", sb, prefix + TAB, false);
        }
    }
    sb.Append(prefix + TAB).Append("return ERR_OK;\n");
    sb.Append(prefix).Append("}\n");
}

} // namespace Idl
} // namespace OHOS