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
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
void SaCppClientProxyCodeEmitter::EmitCode()
{
    SaTypeEmitter::circleCount_ = 0;
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

    sb.Append("\n").Append("#include <iremote_proxy.h>\n");
    EmitSecurecInclusion(sb);
    sb.AppendFormat("#include \"%s.h\"\n", FileName(interfaceName_).c_str());
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
    EmitImportUsingNamespace(sb);
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

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyRegisterDeathRecipient(StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).Append("if (remote_) {\n");
    sb.Append(prefix + TAB).Append("if (!remote->IsProxyObject()) {\n");
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
    EmitInterfaceProxyRegisterDeathRecipient(sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyUnRegisterDeathRecipient(StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("if (remote_ == nullptr) {\n");
    sb.Append(prefix + TAB).Append(TAB).Append("return;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("if (deathRecipient_ == nullptr) {\n");
    sb.Append(prefix + TAB).Append(TAB).Append("return;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("remote_->RemoveDeathRecipient(deathRecipient_);\n");
    sb.Append(prefix + TAB).Append("remote_ = nullptr;\n");
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
    sb.Append(prefix).Append("}\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("explicit %s(\n", proxyName_.c_str());
    sb.Append(prefix + TAB).Append("const sptr<IRemoteObject>& remote)\n");
    sb.Append(prefix + TAB).AppendFormat(": IRemoteProxy<%s>(remote)\n", interfaceName_.c_str());
    if (ast_->GetHasCacheableProxyMethods()) {
        EmitInterfaceProxyAddCacheApi(sb, prefix);
    } else {
        sb.Append(prefix).Append("{}\n");
    }
    sb.Append("\n");
    sb.Append(prefix).AppendFormat("virtual ~%s()\n", proxyName_.c_str());
    if (ast_->GetHasCacheableProxyMethods()) {
        EmitInterfaceProxyUnRegisterDeathRecipient(sb, prefix);
    } else {
        sb.Append(prefix).Append("{}\n");
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
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

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyDeathRecipient(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("class %s : public IRemoteObject::DeathRecipient {\n", deathRecipientName_.c_str());
    sb.Append(prefix).Append("public:\n");
    sb.Append(prefix + TAB).AppendFormat("explicit %s(%s &client) : client_(client) {}\n", deathRecipientName_.c_str(),
        proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("~%s() override = default;\n", deathRecipientName_.c_str());
    sb.Append(prefix + TAB).Append("void OnRemoteDied(const wptr<IRemoteObject> &remote) override\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("client_.OnRemoteDied(remote);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("private:\n");
    sb.Append(prefix + TAB).AppendFormat("%s &client_;\n", proxyName_.c_str());
    sb.Append(prefix).Append("};\n\n");

    sb.Append(prefix).Append("void OnRemoteDied(const wptr<IRemoteObject> &remoteObject)\n");
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("(void)remoteObject;\n");
    sb.Append(prefix + TAB).Append("ApiCacheManager::GetInstance().ClearCache(GetDescriptor());\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).Append("sptr<IRemoteObject> remote_ = nullptr;\n");
    sb.Append(prefix).Append("sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;\n");
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyConstants(StringBuilder &sb, const std::string &prefix)
{
    if (ast_->GetHasCacheableProxyMethods()) {
        EmitInterfaceProxyDeathRecipient(sb, prefix);
    }
    sb.Append(prefix).AppendFormat("static inline BrokerDelegator<%s> delegator_;\n", proxyName_.c_str());

    EmitInterfaceProxyIpcCapacityValues(sb, prefix);
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyIpcCapacityValues(StringBuilder &sb, const std::string &prefix)
{
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (method->HasIpcInCapacity()) {
            sb.Append(prefix).AppendFormat("static constexpr size_t CAPACITY_%s_%d = %s << 10;\n",
                ConstantName(method->GetName()).c_str(), method->GetIpcCode(), method->GetIpcInCapacity().c_str());
        }
    }
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
    EmitImportUsingNamespace(sb);
    EmitInterfaceProxyMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
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
        EmitInterfaceProxyMethodErrCode(method, sb, prefix);
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

void SaCppClientProxyCodeEmitter::EmitInterfaceSetIpcCapacity(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    std::string capacity = StringHelper::Format("CAPACITY_%s_%d", ConstantName(method->GetName()).c_str(),
        method->GetIpcCode());
    sb.Append(prefix).AppendFormat("if (!data.SetMaxCapacity(%s)) {\n", capacity.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).AppendFormat(
            "HiLog::Error(LABEL, \"Failed to set maximum capacity to %%zu\", %s);\n",
            capacity.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix).Append("}\n\n");
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

    std::string option = "MessageOption::TF_SYNC";
    if (method->IsMessageOption()) {
        option = method->GetMessageOption();
    } else if (method->IsOneWay()) {
        option = "MessageOption::TF_ASYNC";
    }
    sb.Append(prefix + TAB).AppendFormat("MessageOption option(%s);\n\n", option.c_str());

    if (method->HasIpcInCapacity()) {
        EmitInterfaceSetIpcCapacity(method, sb, prefix + TAB);
    }

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

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodErrCode(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix + TAB).Append("ErrCode errCode = reply.ReadInt32();\n");
    sb.Append(prefix + TAB).Append("if (FAILED(errCode)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"Read result failed, code is: %{public}d.\",\n")
            .Append(prefix + TAB + TAB + TAB)
            .AppendFormat("static_cast<uint32_t>(%sIpcCode::COMMAND_%s));\n",
            interface_->GetName().c_str(), ConstantName(method->GetName()).c_str());
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
        if (param != nullptr && (param->GetAttribute() & ASTParamAttr::PARAM_OUT)) {
            EmitReadMethodParameter(param, "reply.", false, sb, prefix + TAB);
        }
    }
    AutoPtr<ASTType> returnType = method->GetReturnType();
    if (returnType != nullptr && (returnType->GetTypeKind() != TypeKind::TYPE_VOID)) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter != nullptr) {
            typeEmitter->EmitCppReadVar("reply.", "funcResult", sb, prefix + TAB, false);
        }
    }
}

void SaCppClientProxyCodeEmitter::EmitInterfaceProxyMethodRetValue(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    if (!method->IsOneWay()) {
        sb.Append("\n");
        EmitInterfaceProxyMethodErrCode(method, sb, prefix);
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