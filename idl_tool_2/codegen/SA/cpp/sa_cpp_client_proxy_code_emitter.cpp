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
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, proxyFullName_);
    sb.Append("\n").Append("#include <mutex>\n")
        .Append("#include <iremote_proxy.h>\n")
        .AppendFormat("#include \"%s.h\"\n", FileName(interfaceName_).c_str());
    if (ast_ != nullptr && ast_->GetHasCacheableProxyMethods()) {
        sb.Append("#include \"api_cache_manager.h\"\n");
    }
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
    EmitInterfaceProxyConstructorDecl(sb, TAB);
    EmitInterfaceProxyRemoteDiedCallback(sb, TAB);
    EmitInterfaceProxyMethodDecls(sb, TAB);

    sb.Append("private:\n");
    EmitInterfaceProxyConstants(sb, TAB);

    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyRegisterDeathRecipient(StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).Append("if (remote_ && remote_ != remote) {\n")
        .Append(prefix + TAB).Append("RemoveDeathRecipient();\n")
        .Append(prefix).Append("}\n")
        .Append(prefix).Append("if (remote) {\n")
        .Append(prefix + TAB).Append("if (!remote->IsProxyObject()) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"remote is not proxy object!\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return;\n")
        .Append(prefix + TAB).Append("}\n")
        .Append(prefix + TAB)
        .AppendFormat("deathRecipient_ = new (std::nothrow) %s(*this);\n", deathRecipientName_.c_str())
        .Append(prefix + TAB).Append("if (!deathRecipient_) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"deathRecipient_ is nullptr!\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return;\n")
        .Append(prefix + TAB).Append("}\n")
        .Append(prefix + TAB).Append("if (!remote->AddDeathRecipient(deathRecipient_)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"AddDeathRecipient failed!\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return;\n")
        .Append(prefix + TAB).Append("}\n")
        .Append(prefix + TAB).Append("remote_ = remote;\n")
        .Append(prefix).Append("}\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyAddCacheApi(StringBuilder &sb, const std::string &prefix) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (!method->GetCacheable() || method->IsOneWay()) {
            continue;
        }
        int32_t cacheableTime = method->GetCacheableTime();
        if (cacheableTime != 0) {
            sb.Append(prefix).Append("ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(),\n")
                .Append(prefix + TAB).AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s), %d000);\n",
                    interface_->GetName().c_str(), ConstantName(method->GetName()).c_str(), cacheableTime);
        } else {
            sb.Append(prefix).Append("ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(),\n")
                .Append(prefix + TAB).AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s), 0);\n",
                    interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
        }
    }
    sb.Append("\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyDelCacheApi(StringBuilder &sb, const std::string &prefix) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    if (methodNumber > 0) {
        sb.Append("\n");
        for (size_t i = 0; i < methodNumber; i++) {
            AutoPtr<ASTMethod> method = interface_->GetMethod(i);
            if (method->GetCacheable() && !method->IsOneWay()) {
                sb.Append(prefix).Append("ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(),\n")
                    .Append(prefix + TAB).AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s));\n",
                        interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
            }
        }
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstructorDecl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("explicit %s(const sptr<IRemoteObject>& remote);\n\n", proxyName_.c_str())
        .Append(prefix).AppendFormat("~%s() override;\n\n", proxyName_.c_str());
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceProxyMethodDecl(method, sb, prefix);
        sb.Append("\n");
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyRemoteDiedCallback(StringBuilder &sb, const std::string &prefix)
{
    sb.Append(prefix).Append("void RegisterOnRemoteDiedCallback(const OnRemoteDiedCallback& callback) override\n")
        .Append(prefix).Append("{\n")
        .Append(prefix + TAB).Append("remoteDiedCallback_ = callback;\n")
        .Append(prefix).Append("}\n\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodDecl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("ErrCode %s(", method->GetName().c_str());
    EmitInterfaceMethodParams(method, sb, prefix + TAB);
    sb.Append(") override;\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyDeathRecipient(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("class %s : public IRemoteObject::DeathRecipient {\n", deathRecipientName_.c_str());
    sb.Append(prefix).Append("public:\n");
    sb.Append(prefix + TAB).AppendFormat("explicit %s(%s& client) : client_(client) {}\n", deathRecipientName_.c_str(),
        proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("~%s() override = default;\n", deathRecipientName_.c_str());
    sb.Append(prefix + TAB).Append("void OnRemoteDied(const wptr<IRemoteObject>& remote) override\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("client_.OnRemoteDied(remote);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("private:\n");
    sb.Append(prefix + TAB).AppendFormat("%s& client_;\n", proxyName_.c_str());
    sb.Append(prefix).Append("};\n\n");
    sb.Append(prefix).Append("void RemoveDeathRecipient();\n");
    sb.Append(prefix).Append("void OnRemoteDied(const wptr<IRemoteObject>& remote);\n\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstants(StringBuilder &sb, const std::string &prefix)
{
    EmitInterfaceProxyDeathRecipient(sb, prefix);
    sb.Append(prefix).AppendFormat("static inline BrokerDelegator<%s> delegator_;\n", proxyName_.c_str())
        .Append(prefix + "sptr<IRemoteObject> remote_;\n")
        .Append(prefix + "sptr<IRemoteObject::DeathRecipient> deathRecipient_;\n")
        .Append(prefix + "OnRemoteDiedCallback remoteDiedCallback_;\n")
        .Append(prefix + "std::mutex mutex_;\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyCppFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(proxyName_).c_str()));
    File file(filePath, File::WRITE);
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
    EmitInterfaceProxyConstructorImpl(sb);
    EmitInterfaceProxyRemoveDeathRecipient(sb);
    EmitInterfaceProxyOnRemoteDied(sb);
    EmitInterfaceProxyMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstructorImpl(StringBuilder &sb) const
{
    sb.AppendFormat("%s::%s(const sptr<IRemoteObject>& remote) : IRemoteProxy<%s>(remote)\n{\n",
        proxyName_.c_str(), proxyName_.c_str(), interfaceName_.c_str());
    if (ast_->GetHasCacheableProxyMethods()) {
        EmitInterfaceProxyAddCacheApi(sb, TAB);
    }
    sb.Append(TAB).Append("std::lock_guard<std::mutex> lock(mutex_);\n");
    EmitInterfaceProxyRegisterDeathRecipient(sb, TAB);
    sb.Append("}\n\n");

    sb.AppendFormat("%s::~%s()\n", proxyName_.c_str(), proxyName_.c_str())
        .Append("{\n")
        .Append(TAB).Append("RemoveDeathRecipient();\n");
    if (ast_->GetHasCacheableProxyMethods()) {
        EmitInterfaceProxyDelCacheApi(sb, TAB);
    }
    sb.Append("}\n\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyRemoveDeathRecipient(StringBuilder &sb) const
{
    sb.AppendFormat("void %s::RemoveDeathRecipient()\n", proxyName_.c_str())
        .Append("{\n")
        .Append(TAB).Append("std::lock_guard<std::mutex> lock(mutex_);\n");
    if (logOn_) {
        sb.Append(TAB).Append("HiLog::Info(LABEL, \"Remove death recipient\");\n");
    }
    sb.Append(TAB).Append("if (!remote_ || !deathRecipient_) {\n")
        .Append(TAB).Append(TAB).Append("return;\n")
        .Append(TAB).Append("}\n")
        .Append(TAB).Append("remote_->RemoveDeathRecipient(deathRecipient_);\n")
        .Append(TAB).Append("remote_ = nullptr;\n")
        .Append(TAB).Append("deathRecipient_ = nullptr;\n")
        .Append(TAB).Append("remoteDiedCallback_ = nullptr;\n")
        .Append("}\n\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyOnRemoteDied(StringBuilder &sb) const
{
    sb.AppendFormat("void %s::OnRemoteDied(const wptr<IRemoteObject>& remote)\n{\n", proxyName_.c_str());
    if (logOn_) {
        sb.Append(TAB).Append("HiLog::Info(LABEL, \"On remote died\");\n");
    }
    sb.Append(TAB).Append("if (remoteDiedCallback_) {\n")
        .Append(TAB).Append(TAB).Append("remoteDiedCallback_(remote);\n")
        .Append(TAB).Append("}\n")
        .Append(TAB).Append("RemoveDeathRecipient();\n");

    if (ast_->GetHasCacheableProxyMethods()) {
        sb.Append("\n").Append(TAB).Append("ApiCacheManager::GetInstance().ClearCache(GetDescriptor());\n");
    }
    sb.Append("}\n\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
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

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodPreSendRequest(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    if ((method->GetCacheable()) && (!method->IsOneWay())) {
        sb.Append(prefix).Append("bool hitCache = ApiCacheManager::GetInstance().PreSendRequest(GetDescriptor(),\n")
            .Append(prefix + TAB).AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s), data, reply);",
                interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
        sb.Append("\n");
        sb.Append(prefix).Append("if (hitCache) {\n");
        EmitInterfaceProxyMethodErrCode(sb, prefix);
        EmitInterfaceProxyMethodReply(method, sb, prefix);
        sb.Append(prefix + TAB).Append("return ERR_OK;\n");
        sb.Append(prefix).Append("}\n\n");
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodPostSendRequest(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix + TAB).Append("ApiCacheManager::GetInstance().PostSendRequest(GetDescriptor(),\n")
        .Append(prefix + TAB + TAB).AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s), data, reply);\n",
            interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
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
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Write interface token failed!\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    size_t paramNumber = method->GetParameterNumber();
    for (size_t i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            EmitWriteMethodParameter(param, "data.", sb, prefix + TAB);
        }
    }
    if (paramNumber > 0) {
        sb.Append('\n');
    }

    EmitInterfaceProxyMethodPreSendRequest(method, sb, prefix + TAB);
    sb.Append(prefix + TAB).Append("sptr<IRemoteObject> remote = Remote();\n");
    sb.Append(prefix + TAB).Append("if (!remote) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Remote is nullptr!\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("int32_t result = remote->SendRequest(\n")
        .Append(prefix + TAB + TAB)
        .AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s), data, reply, option);\n",
            interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
    sb.Append(prefix + TAB).Append("if (FAILED(result)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Send request failed!\");\n");
    }
    sb.Append(prefix + TAB).Append("    return result;\n");
    sb.Append(prefix + TAB).Append("}\n");
    EmitInterfaceProxyMethodRetValue(method, sb, prefix);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodErrCode(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix + TAB).Append("ErrCode errCode = reply.ReadInt32();\n");
    sb.Append(prefix + TAB).Append("if (FAILED(errCode)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Read Int32 failed!\");\n");
    }
    sb.Append(prefix + TAB).Append("    return errCode;\n");
    sb.Append(prefix + TAB).Append("}\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodReply(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    size_t paramNumber = method->GetParameterNumber();
    for (size_t i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            EmitReadMethodParameter(param, "reply.", false, sb, prefix + TAB);
        }
    }
    AutoPtr<ASTType> returnType = method->GetReturnType();
    if (returnType->GetTypeKind() != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        typeEmitter->EmitCppReadVar("reply.", "funcResult", sb, prefix + TAB, false);
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodRetValue(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    if (!method->IsOneWay()) {
        sb.Append("\n");
        EmitInterfaceProxyMethodErrCode(sb, prefix);
        sb.Append("\n");
        if (method->GetCacheable()) {
            EmitInterfaceProxyMethodPostSendRequest(method, sb, prefix);
        }
        EmitInterfaceProxyMethodReply(method, sb, prefix);
    }
    sb.Append(prefix + TAB).Append("return ERR_OK;\n");
    sb.Append(prefix).Append("}\n");
}

} // namespace Idl
} // namespace OHOS