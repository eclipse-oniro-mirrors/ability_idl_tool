/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "sa_cpp_client_code_emitter.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {

void SaCppClientCodeEmitter::EmitCode()
{
    EmitClientHeaderFile();
    EmitClientSourceFile();
}

void SaCppClientCodeEmitter::EmitClientHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(clientName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, clientFullName_);
    sb.Append('\n');

    sb.Append("#include <shared_mutex>\n");
    sb.Append("#include <utility>\n");
    sb.Append("#include <iremote_proxy.h>\n");
    sb.Append("#include <if_system_ability_manager.h>\n");
    sb.Append("#include <iservice_registry.h>\n");
    sb.Append("#include <system_ability_load_callback_stub.h>\n");

    EmitSecurecInclusion(sb);
    sb.AppendFormat("#include \"%s.h\"\n", FileName(interfaceName_).c_str());
    sb.Append('\n');
    EmitInterfaceClientInHeaderFile(sb);
    EmitTailMacro(sb, clientFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppClientCodeEmitter::EmitInterfaceClientInHeaderFile(StringBuilder &sb) const
{
    EmitBeginNamespace(sb);
    EmitImportUsingNamespace(sb);
    sb.AppendFormat("using %sOnRemoteDiedCallback = std::function<void()>;\n", baseName_.c_str());
    sb.AppendFormat("using %sOnLocalCallback = std::function<void(bool status)>;\n\n", baseName_.c_str());
    sb.AppendFormat("class %s {\n", clientName_.c_str());
    sb.Append("public:\n");
    EmitInterfaceClientConstructor(sb, TAB);
    sb.Append('\n');
    EmitLoadCallback(sb, TAB);
    sb.Append('\n');
    EmitInterfaceClientMethodDecls(sb, TAB);
    sb.Append('\n');
    sb.Append("private:\n");
    EmitInterfaceClientDeathRecipient(sb, TAB);
    sb.Append("\n");
    EmitInterfaceClientSetParam(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void SaCppClientCodeEmitter::EmitInterfaceClientConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("explicit %s(int32_t systemAbilityId, const std::string &deviceId = \"\");\n",
        clientName_.c_str());
    sb.Append('\n');
    sb.Append(prefix).AppendFormat("~%s();\n", clientName_.c_str());
}

void SaCppClientCodeEmitter::EmitLoadCallback(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("/**\n");
    sb.Append(prefix).Append(" * Sync load sa with timeout\n");
    sb.Append(prefix).Append(" * @param timeout timeout seconds\n");
    sb.Append(prefix).Append(" * @return error code\n");
    sb.Append(prefix).Append(" */\n");
    sb.Append(prefix).Append("ErrCode LoadSystemAbility(int32_t timeout);\n\n");

    sb.Append(prefix).Append("/**\n");
    sb.Append(prefix).Append(" * Async load sa with a callback\n");
    sb.Append(prefix).Append(" * @param cb callback function\n");
    sb.Append(prefix).Append(" * @return error code\n");
    sb.Append(prefix).Append(" */\n");
    sb.Append(prefix).AppendFormat("ErrCode LoadSystemAbility(const %sOnLocalCallback &cb);\n\n", baseName_.c_str());

    sb.Append(prefix).Append("/**\n");
    sb.Append(prefix).Append(" * Check sa\n");
    sb.Append(prefix).Append(" * @return error code\n");
    sb.Append(prefix).Append(" */\n");
    sb.Append(prefix).Append("ErrCode CheckSystemAbility();\n\n");

    sb.Append(prefix).Append("/**\n");
    sb.Append(prefix).Append(" * Get proxy status\n");
    sb.Append(prefix).Append(" * @return status\n");
    sb.Append(prefix).Append(" */\n");
    sb.Append(prefix).AppendFormat("bool Get%sStatus();\n\n", proxyName_.c_str());

    sb.Append(prefix).Append("/**\n");
    sb.Append(prefix).Append(" * Register remote death callback function\n");
    sb.Append(prefix).Append(" * @param cb callback function\n");
    sb.Append(prefix).Append(" */\n");
    sb.Append(prefix).AppendFormat("void RegisterOnRemoteDiedCallback(const %sOnRemoteDiedCallback &cb);\n",
        baseName_.c_str());
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    if (interface_ == nullptr) {
        return;
    }
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (method == nullptr) {
            return;
        }
        EmitInterfaceClientMethodDecl(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append('\n');
        }
    }
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodDecl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("ErrCode %s(", method->GetName().c_str());
    EmitInterfaceClientMethodParams(method, sb, prefix + TAB, true);
    sb.Append(");\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientDeathRecipient(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// death callback class\n");
    sb.Append(prefix).AppendFormat("class %sRecipient : public IRemoteObject::DeathRecipient {\n", baseName_.c_str());
    sb.Append(prefix).Append("public:\n");
    sb.Append(prefix + TAB).AppendFormat("explicit %sRecipient(%s &client) : client_(client) {}\n", baseName_.c_str(),
        clientName_.c_str());
    sb.Append(prefix + TAB).Append("void OnRemoteDied(const wptr<IRemoteObject> &remote) override\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("client_.OnRemoteDied(remote);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("private:\n");
    sb.Append(prefix + TAB).AppendFormat("%s &client_;\n", clientName_.c_str());
    sb.Append(prefix).Append("};\n");
    sb.Append(prefix).Append("void RemoveDeathRecipient();\n");
    sb.Append(prefix).Append("void OnRemoteDied(const wptr<IRemoteObject> &remote);\n\n");

    sb.Append(prefix).Append("// load callback class\n");
    sb.Append(prefix).Append("class LoadCallback : public SystemAbilityLoadCallbackStub {\n");
    sb.Append(prefix).Append("public:\n");
    sb.Append(prefix + TAB).AppendFormat(
        "LoadCallback(%sOnLocalCallback cb, %s &client): localCallback_(std::move(cb)), client_(client) {}\n",
            baseName_.c_str(), clientName_.c_str());
    sb.Append(prefix + TAB).Append(
        "void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remote) override;\n");
    sb.Append(prefix + TAB).Append("void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;\n");
    sb.Append(prefix).Append("private:\n");
    sb.Append(prefix + TAB).AppendFormat("%sOnLocalCallback localCallback_;\n", baseName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s &client_;\n", clientName_.c_str());
    sb.Append(prefix).Append("};\n\n");

    sb.Append(prefix).Append("// local load callback class\n");
    sb.Append(prefix).Append("class LocalLoadCallback : public SystemAbilityLoadCallbackStub {\n");
    sb.Append(prefix).Append("public:\n");
    sb.Append(prefix + TAB).AppendFormat("explicit LocalLoadCallback(%s &client) : client_(client) {}\n",
        clientName_.c_str());
    sb.Append(prefix + TAB).Append(
        "void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remote) override;\n");
    sb.Append(prefix + TAB).Append("void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;\n");
    sb.Append(prefix).Append("private:\n");
    sb.Append(prefix + TAB).AppendFormat("%s &client_;\n", clientName_.c_str());
    sb.Append(prefix).Append("};\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientSetParam(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("void Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix).AppendFormat("ErrCode Set%s(const sptr<IRemoteObject> &remote);\n", proxyName_.c_str());
    sb.Append(prefix).Append("ErrCode PreventJitter(bool isSync, int32_t timeout = 0);\n");
    sb.Append(prefix).Append("ErrCode LoadSystemAbility() const;\n\n");

    sb.Append(prefix).Append("sptr<IRemoteObject::DeathRecipient> deathRecipient_;\n");
    sb.Append(prefix).AppendFormat("sptr<%s> proxy_;\n", interfaceName_.c_str());
    sb.Append(prefix).Append("sptr<IRemoteObject> remote_;\n\n");

    sb.Append(prefix).AppendFormat("%sOnRemoteDiedCallback remoteDiedCallback_;\n", baseName_.c_str());
    sb.Append(prefix).Append("sptr<LoadCallback> loadCallback_;\n");
    sb.Append(prefix).Append("sptr<LocalLoadCallback> localLoadCallback_;\n\n");

    sb.Append(prefix).Append("std::shared_mutex mutexRemote_;\n");
    sb.Append(prefix).Append("std::shared_mutex mutexLoadSA_;\n");
    sb.Append(prefix).Append("std::shared_mutex mutexRemoteDied_;\n\n");

    sb.Append(prefix).Append("int32_t systemAbilityId_;\n");
    sb.Append(prefix).Append("std::string deviceId_;\n");
    sb.Append(prefix).Append("std::atomic_bool saLoading_ = false;\n");
    sb.Append(prefix).Append("std::atomic_llong loadStart_ = 0; // microsecond timestamp\n\n");

    sb.Append(prefix).
        Append("static constexpr int32_t LOAD_SA_TIMEOUT = 4;    // default load sa timeout is 4 seconds\n");
    sb.Append(prefix).
        Append("static constexpr double MICROSECOND_TO_SECOND = 1000000.0;    // microseconds to seconds\n");
    if (logOn_) {
        sb.Append(prefix).AppendFormat(
            "static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, %s, \"%s\"};\n",
            domainId_.c_str(), logTag_.c_str());
    }
}

void SaCppClientCodeEmitter::EmitClientSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(clientName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    sb.Append('\n');
    sb.Append("#include <chrono>\n");
    sb.AppendFormat("#include \"%s.h\"\n", FileName(clientName_).c_str());
    if (logOn_) {
        sb.Append("#include \"hilog/log.h\"\n");
    }
    if (hitraceOn_) {
        sb.Append("#include \"hitrace_meter.h\"\n");
    }
    sb.Append('\n');
    if (logOn_) {
        sb.Append("using OHOS::HiviewDFX::HiLog;\n\n");
    }
    EmitBeginNamespace(sb);
    EmitImportUsingNamespace(sb);
    EmitInterfaceClientMethodConstructor(sb, "");
    EmitInterfaceClientMethodAbility(sb);
    EmitInterfaceClientMethodDeathCallback(sb, "");
    EmitInterfaceClientMethodImpls(sb, "");
    sb.Append('\n');
    EmitInterfaceClientMethodDeathRecipient(sb, "");
    EmitInterfaceClientMethodAbilitySF(sb, "");
    EmitInterfaceClientMethodStatusProxy(sb, "");
    EmitInterfaceClientMethodPrevent(sb, "");
    EmitInterfaceClientMethodAbilityLoad(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s::%s(int32_t systemAbilityId, const std::string &deviceId) :\n",
        clientName_.c_str(), clientName_.c_str());
    sb.Append(prefix + TAB).Append("systemAbilityId_(systemAbilityId), deviceId_(deviceId)\n");
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("deathRecipient_ = new (std::nothrow) %sRecipient(*this);\n",
        baseName_.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append("if (!deathRecipient_) {\n");
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl init deathRecipient_ failed\");\n");
        sb.Append(prefix + TAB).Append("}\n");
    }
    sb.Append("\n");
    sb.Append(prefix + TAB).Append("localLoadCallback_ = new (std::nothrow) LocalLoadCallback(*this);\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append("if (!localLoadCallback_) {\n");
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl init localLoadCallback_ failed\");\n");
        sb.Append(prefix + TAB).Append("}\n");
    }
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("%s::~%s()\n", clientName_.c_str(), clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("RemoveDeathRecipient();\n");
    sb.Append(prefix + TAB).Append("remoteDiedCallback_ = nullptr;\n");
    sb.Append(prefix + TAB).Append("loadCallback_ = nullptr;\n");
    sb.Append(prefix + TAB).Append("localLoadCallback_ = nullptr;\n");
    sb.Append(prefix + TAB).Append("deathRecipient_ = nullptr;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbility(StringBuilder &sb) const
{
    EmitInterfaceClientMethodAbilitySync(sb, "");
    EmitInterfaceClientMethodAbilityAsync(sb, "");
    EmitInterfaceClientMethodAbilityCheck(sb, "");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbilitySync(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// Sync load sa with timeout\n");
    sb.Append(prefix).AppendFormat("ErrCode %s::LoadSystemAbility(int32_t timeout)\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    EmitInterfaceClientSamgr(sb, prefix);
    sb.Append(prefix + TAB).
        Append("sptr<IRemoteObject> remote = samgr->LoadSystemAbility(systemAbilityId_, timeout);\n");
    sb.Append(prefix + TAB).AppendFormat("ErrCode ret = Set%s(remote);\n", proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix + TAB).Append("return ret;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbilityAsync(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// Async load sa with a callback\n");
    sb.Append(prefix).AppendFormat("ErrCode %s::LoadSystemAbility(const %sOnLocalCallback &cb)\n",
        clientName_.c_str(), baseName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("std::shared_lock<std::shared_mutex> lock(mutexLoadSA_);\n");
    sb.Append(prefix + TAB + TAB).Append("if (!loadCallback_) {\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("loadCallback_ = new LoadCallback(cb, *this);\n");
    sb.Append(prefix + TAB + TAB).Append("}");
    if (logOn_) {
        sb.Append(" else {\n");
        sb.Append(prefix + TAB + TAB + TAB).Append("HiLog::Warn(LABEL, \"idl load callback is already exist\");\n");
        sb.Append(prefix + TAB + TAB).Append("}");
    }
    sb.Append("\n");
    sb.Append(prefix + TAB).Append("}\n\n");
    EmitInterfaceClientSamgr(sb, prefix);
    sb.Append(prefix + TAB).Append("if (deviceId_.empty()) {\n");
    sb.Append(prefix + TAB + TAB).Append("return samgr->LoadSystemAbility(systemAbilityId_, loadCallback_);\n");
    sb.Append(prefix + TAB).Append("} else {\n");
    sb.Append(prefix + TAB + TAB).
        Append("return samgr->LoadSystemAbility(systemAbilityId_, deviceId_, loadCallback_);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbilityCheck(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// Check sa\n");
    sb.Append(prefix).AppendFormat("ErrCode %s::CheckSystemAbility()\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    EmitInterfaceClientSamgr(sb, prefix);
    sb.Append(prefix + TAB).
        Append("sptr<IRemoteObject> remote = samgr->CheckSystemAbility(systemAbilityId_, deviceId_);\n");
    sb.Append(prefix + TAB).AppendFormat("ErrCode ret = Set%s(remote);\n", proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix + TAB).Append("return ret;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodDeathCallback(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// Get proxy status\n");
    sb.Append(prefix).AppendFormat("bool %s::Get%sStatus()\n", clientName_.c_str(), proxyName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("std::shared_lock<std::shared_mutex> readLock(mutexRemote_);\n");
    sb.Append(prefix + TAB).Append("return proxy_ != nullptr;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).Append("// Register remote death callback function\n");
    sb.Append(prefix).AppendFormat("void %s::RegisterOnRemoteDiedCallback(const %sOnRemoteDiedCallback &cb)\n",
        clientName_.c_str(), baseName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("std::unique_lock<std::shared_mutex> lock(mutexRemoteDied_);\n");
    sb.Append(prefix + TAB).Append("remoteDiedCallback_ = cb;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    if (interface_ == nullptr) {
        return;
    }
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (method == nullptr) {
            return;
        }
        EmitInterfaceClientMethodImpl(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append('\n');
        }
    }
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("ErrCode %s::%s(", clientName_.c_str(), method->GetName().c_str());
    EmitInterfaceClientMethodParams(method, sb, prefix + TAB, false);
    sb.Append(")\n");
    EmitInterfaceClientMethodBody(method, sb, prefix);
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodBody(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("std::shared_lock<std::shared_mutex> readLock(mutexRemote_);\n");
    sb.Append(prefix + TAB + TAB).Append("if (proxy_) {\n");
    sb.Append(prefix + TAB + TAB + TAB).AppendFormat("return proxy_->%s(", method->GetName().c_str());
    EmitInterfaceClientMethodParamsWithoutType(method, sb, prefix + TAB + TAB + TAB + TAB);
    sb.Append(");\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");

    if (method->IsOneWay()) {
        sb.Append(prefix + TAB).Append("ErrCode ret = PreventJitter(false, LOAD_SA_TIMEOUT);\n");
        sb.Append(prefix + TAB).Append("if (ret != ERR_OK) {\n");
        sb.Append(prefix + TAB + TAB).Append("return ret;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("return ERR_INVALID_VALUE;\n");
    } else {
        sb.Append(prefix + TAB).Append("ErrCode ret = PreventJitter(true, timeout);\n");
        sb.Append(prefix + TAB).Append("if (ret == ERR_OK) {\n");
        sb.Append(prefix + TAB + TAB).Append("std::shared_lock<std::shared_mutex> readLock(mutexRemote_);\n");
        sb.Append(prefix + TAB + TAB).Append("if (proxy_) {\n");
        sb.Append(prefix + TAB + TAB + TAB).AppendFormat("return proxy_->%s(", method->GetName().c_str());
        EmitInterfaceClientMethodParamsWithoutType(method, sb, prefix + TAB + TAB + TAB + TAB);
        sb.Append(");\n");
        sb.Append(prefix + TAB + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("return ret;\n");
    }
    sb.Append(prefix).AppendFormat("}\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodDeathRecipient(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("void %s::RemoveDeathRecipient()\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("std::unique_lock<std::shared_mutex> writeLock(mutexRemote_);\n");
    sb.Append(prefix + TAB).Append("proxy_ = nullptr;\n");
    sb.Append(prefix + TAB).Append("if (remote_ && deathRecipient_) {\n");
    sb.Append(prefix + TAB + TAB).Append("remote_->RemoveDeathRecipient(deathRecipient_);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("remote_ = nullptr;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("void %s::OnRemoteDied(const wptr<IRemoteObject> &remote)\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("std::shared_lock<std::shared_mutex> readLock(mutexRemote_);\n");
    sb.Append(prefix + TAB + TAB).Append("if (remote != remote_) {\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("return;\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("{\n");
    sb.Append(prefix + TAB + TAB).Append("std::unique_lock<std::shared_mutex> lock(mutexRemoteDied_);\n");
    sb.Append(prefix + TAB + TAB).Append("if (remoteDiedCallback_) {\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("remoteDiedCallback_();\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("RemoveDeathRecipient();\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbilitySF(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("void %s::LoadCallback::OnLoadSystemAbilitySuccess("
        "int32_t systemAbilityId, const sptr<IRemoteObject> &remote)\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("client_.Set%s(remote);\n", proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("client_.Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix + TAB).Append("if (localCallback_) {\n");
    sb.Append(prefix + TAB + TAB).Append("localCallback_(remote != nullptr);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("void %s::LoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)\n",
        clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    if (logOn_) {
        sb.Append(prefix + TAB).
            Append("HiLog::Error(LABEL, \"idl LoadSystemAbility failed, %{public}d\", systemAbilityId);\n");
    }
    sb.Append(prefix + TAB).AppendFormat("client_.Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix + TAB).Append("if (localCallback_) {\n");
    sb.Append(prefix + TAB + TAB).Append("localCallback_(false);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("void %s::LocalLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,\n",
        clientName_.c_str());
    sb.Append(prefix + TAB).Append("const sptr<IRemoteObject> &remote)\n");
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("client_.Set%s(remote);\n", proxyName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("client_.Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("void %s::LocalLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)\n",
        clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    if (logOn_) {
        sb.Append(prefix + TAB).
            Append("HiLog::Error(LABEL, \"idl local LoadSystemAbility failed, %{public}d\", systemAbilityId);\n");
    }
    sb.Append(prefix + TAB).AppendFormat("client_.Set%sStatus();\n", baseName_.c_str());
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodStatusProxy(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("void %s::Set%sStatus()\n", clientName_.c_str(), baseName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("saLoading_ = false;\n");
    sb.Append(prefix + TAB).Append("loadStart_ = std::chrono::duration_cast<std::chrono::microseconds>(\n");
    sb.Append(prefix + TAB + TAB).Append("std::chrono::system_clock::now().time_since_epoch()).count();\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("ErrCode %s::Set%s(const sptr<IRemoteObject> &remote)\n",
        clientName_.c_str(), proxyName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("std::unique_lock<std::shared_mutex> writeLock(mutexRemote_);\n\n");
    sb.Append(prefix + TAB).Append("// prevent re-entry\n");
    sb.Append(prefix + TAB).Append("if (!remote) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB)
            .Append("HiLog::Error(LABEL, \"idl remote is nullptr, said:%{public}d\", systemAbilityId_);\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_NULL_OBJECT;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("if (remote_ && remote == remote_) {\n");
    sb.Append(prefix + TAB + TAB).Append("return ERR_OK;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("remote_ = remote;\n\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append("if (deathRecipient_ && !remote_->AddDeathRecipient(deathRecipient_)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl AddDeathRecipient failed\");\n");
        sb.Append(prefix + TAB).Append("}\n\n");
    }
    sb.Append(prefix + TAB).AppendFormat("proxy_ = iface_cast<%s>(remote_);\n", interfaceName_.c_str());
    sb.Append(prefix + TAB).Append("if (!proxy_) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl init proxy failed\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_NULL_OBJECT;\n");
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).Append("return ERR_OK;\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodPrevent(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).
        Append("// Prevent jitter, avoid multiple calls at the same time. The unit of timeout is second.\n");
    sb.Append(prefix).AppendFormat("ErrCode %s::PreventJitter(bool isSync, int32_t timeout)\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("// Load once every TIMEOUT seconds, default is 4 seconds\n");
    sb.Append(prefix + TAB).Append("double loadDiff = 0;\n");
    sb.Append(prefix + TAB).Append("if (saLoading_) {\n");
    sb.Append(prefix + TAB + TAB).Append("auto loadEnd = std::chrono::duration_cast<std::chrono::microseconds>(\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("std::chrono::system_clock::now().time_since_epoch()).count();\n");
    sb.Append(prefix + TAB + TAB).
        Append("loadDiff = static_cast<double>(loadEnd - loadStart_.load()) / MICROSECOND_TO_SECOND;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append(prefix + TAB).Append("if (!saLoading_ || loadDiff >= static_cast<double>(timeout)) {\n");
    sb.Append(prefix + TAB + TAB).Append("saLoading_ = true;\n");\
    sb.Append(prefix + TAB + TAB).Append("if (isSync) {\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("return LoadSystemAbility(timeout);\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB + TAB).Append("return LoadSystemAbility();\n");
    sb.Append(prefix + TAB).Append("} else {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl sa is loading or not timeout\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientMethodAbilityLoad(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("// Async callback function\n");
    sb.Append(prefix).AppendFormat("ErrCode %s::LoadSystemAbility() const\n", clientName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("if (!localLoadCallback_) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl localLoadCallback_ is nullptr\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_NULL_OBJECT;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    EmitInterfaceClientSamgr(sb, prefix);

    sb.Append(prefix + TAB).Append("if (deviceId_.empty()) {\n");
    sb.Append(prefix + TAB + TAB).Append("return samgr->LoadSystemAbility(systemAbilityId_, localLoadCallback_);\n");
    sb.Append(prefix + TAB).Append("} else {\n");
    sb.Append(prefix + TAB + TAB).
        Append("return samgr->LoadSystemAbility(systemAbilityId_, deviceId_, localLoadCallback_);\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n");
}

void SaCppClientCodeEmitter::EmitInterfaceClientSamgr(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix + TAB).
        Append("auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();\n");
    sb.Append(prefix + TAB).Append("if (!samgr) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append("HiLog::Error(LABEL, \"idl samgr is nullptr\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return ERR_NULL_OBJECT;\n");
    sb.Append(prefix + TAB).Append("}\n\n");
}

} // namespace Idl
} // namespace OHOS