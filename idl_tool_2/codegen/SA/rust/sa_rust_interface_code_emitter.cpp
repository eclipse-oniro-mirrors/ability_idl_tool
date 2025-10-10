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

#include "sa_rust_interface_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
namespace {
const uint32_t WRAP_ANCHOR = 4;
}

bool SaRustInterfaceCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = GetFileParentPath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("SaRustInterfaceCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void SaRustInterfaceCodeEmitter::EmitCode()
{
    EmitInterfaceHeaderFile();
    return;
}

void SaRustInterfaceCodeEmitter::EmitInterfaceHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.rs", directory_.c_str(), interfaceName_.c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb);
    EmitHeaders(sb);
    sb.Append("\n");
    EmitCommands(sb);
    sb.Append("\n");
    EmitRemoteObject(sb);
    sb.Append("\n");
    EmitBrokers(sb);
    sb.Append("\n");
    EmitRemoteRequest(sb);
    sb.Append("\n");
    EmitStub(sb);
    sb.Append("\n");
    EmitProxy(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
    return;
}

void SaRustInterfaceCodeEmitter::EmitHeaders(StringBuilder &sb) const
{
    EmitCommonHeaders(sb);
    EmitIPCHeaders(sb);
    if (EmitCustomHeaders(sb)) {
        sb.Append("\n");
    }
}

void SaRustInterfaceCodeEmitter::EmitIPCHeaders(StringBuilder &sb) const
{
    sb.Append("extern crate ipc_rust;\n");
    sb.Append("\n");
    sb.Append("use ipc_rust::{\n");
    sb.Append("    IRemoteBroker, IRemoteObj, RemoteStub, Result,\n");
    sb.Append("    RemoteObj, define_remote_object, FIRST_CALL_TRANSACTION\n");
    sb.Append("};\n");
    sb.Append("use ipc_rust::{MsgParcel, BorrowedMsgParcel};\n");
    sb.Append("\n");
}

bool SaRustInterfaceCodeEmitter::EmitCustomHeaders(StringBuilder &sb) const
{
    uint32_t custom = false;
    int sequenceableNumber = static_cast<int>(ast_->GetSequenceableDefNumber());
    for (int i = 0; i < sequenceableNumber; i++) {
        AutoPtr<ASTSequenceableType> seqType = ast_->GetSequenceableDef(i);
        bool addPathMsRes = AppendRealPath(sb, seqType->GetFullName());
        custom |= static_cast<uint32_t>(addPathMsRes);
    }

    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal()) {
            bool addPathMiRes = AppendRealPath(sb, interface->GetFullName());
            custom |= static_cast<uint32_t>(addPathMiRes);
        }
    }
    return static_cast<bool>(custom);
}

bool SaRustInterfaceCodeEmitter::AppendRealPath(StringBuilder& sb, const std::string &fpnpp) const
{
    std::string result = GeneratePath(fpnpp);
    if (result.empty()) {
        return false;
    }
    sb.Append("use ").Append(result).Append(";\n");
    return true;
}

std::string SaRustInterfaceCodeEmitter::GeneratePath(const std::string &fpnp) const
{
    size_t pos = fpnp.rfind("..");
    if (pos == std::string::npos) {
        std::string path = TrimDot(fpnp);
        if (path.empty()) {
            return "";
        }
        return StringHelper::Replace(path, ".", "::");
    }

    std::string path = TrimDot(fpnp.substr(0, pos + 1));
    std::string file = TrimDot(fpnp.substr(pos));
    if (path.empty()) {
        return "";
    }

    if ((path.find("..") != std::string::npos) || (file.find("..") != std::string::npos)) {
        return "";
    }

    StringBuilder realPath;
    realPath.Append(StringHelper::Replace(path, ".", "::")).Append("::{");
    realPath.Append(StringHelper::Replace(file, ".", ", "));
    realPath.Append("}");

    return realPath.ToString();
}

std::string SaRustInterfaceCodeEmitter::TrimDot(const std::string &fpnp) const
{
    if (fpnp.empty()) {
        return "";
    }

    int left = 0;
    int right = static_cast<int>(fpnp.length()) - 1;
    while ((fpnp[left] == ' ') || (fpnp[left] == '.')) {
        left++;
    }

    while ((fpnp[right] == ' ') || (fpnp[right] == '.')) {
        right--;
    }

    if (left >= right) {
        return "";
    }

    return fpnp.substr(left, right + 1);
}

void SaRustInterfaceCodeEmitter::EmitCommands(StringBuilder &sb) const
{
    sb.AppendFormat("pub enum %sCode {\n", interfaceName_.c_str());
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (i == 0) {
            sb.AppendFormat("    %s  = FIRST_CALL_TRANSACTION,\n", GetCodeFromMethod(method->GetName()).c_str());
        } else {
            sb.AppendFormat("    %s,\n", GetCodeFromMethod(method->GetName()).c_str());
        }
    }
    sb.Append("}\n");
}

std::string SaRustInterfaceCodeEmitter::GetCodeFromMethod(const std::string &name) const
{
    StringBuilder sb;
    std::string result = name;
    bool hasUpper = false;

    sb.Append("Code");
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i] != '_') {
            if (!hasUpper) {
                sb.Append(toupper(result[i]));
                hasUpper = true;
            } else {
                sb.Append(result[i]);
            }
        } else {
            hasUpper = false;
        }
    }
    return sb.ToString();
}

void SaRustInterfaceCodeEmitter::EmitRemoteObject(StringBuilder &sb) const
{
    sb.Append("define_remote_object!(\n");
    if (StringHelper::StartWith(interfaceFullName_, ".")) {
        sb.AppendFormat("    %s[\"%s\"] {\n", interfaceName_.c_str(), interfaceName_.c_str());
    } else {
        sb.AppendFormat("    %s[\"%s\"] {\n", interfaceName_.c_str(), interfaceFullName_.c_str());
    }
    sb.AppendFormat("        stub: %s(on_remote_request),\n", stubName_.c_str());
    sb.AppendFormat("        proxy: %s,\n", proxyName_.c_str());
    sb.Append("    }\n");
    sb.Append(");\n");
}

void SaRustInterfaceCodeEmitter::EmitBrokers(StringBuilder &sb) const
{
    sb.AppendFormat("pub trait %s: IRemoteBroker {\n", interfaceName_.c_str());
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.AppendFormat("    fn %s(&self", method->GetName().c_str());
        AppendBrokerParameters(sb, method);
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(method->GetReturnType());
        if (typeEmitter != nullptr) {
            sb.AppendFormat(") -> Result<%s>;\n", typeEmitter->EmitRustType().c_str());
        }
    }
    sb.Append("}\n");
}

void SaRustInterfaceCodeEmitter::AppendBrokerParameters(StringBuilder &sb, AutoPtr<ASTMethod> &method) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        WrapLine(sb, i, "        ");
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
        if (typeEmitter != nullptr) {
            sb.AppendFormat("%s: &%s", GetNameFromParameter(param->GetName()).c_str(),
                typeEmitter->EmitRustType(true).c_str());
        }
    }
}

void SaRustInterfaceCodeEmitter::WrapLine(StringBuilder &sb, int index, const std::string &prefix) const
{
    if ((index + 1) % WRAP_ANCHOR == 0) {
        sb.AppendFormat(",\n%s", prefix.c_str());
    } else {
        sb.Append(", ");
    }
}

std::string SaRustInterfaceCodeEmitter::GetNameFromParameter(const std::string &name) const
{
    StringBuilder sb;
    std::string result = name;
    bool start = true;
    for (size_t i = 0; i < result.size(); i++) {
        if (start) {
            if (isupper(result[i])) {
                sb.Append('p');
            }
            start = false;
        }

        if (isupper(result[i])) {
            sb.Append('_');
            sb.Append(tolower(result[i]));
        } else {
            sb.Append(result[i]);
        }
    }
    return sb.ToString();
}

void SaRustInterfaceCodeEmitter::EmitRemoteRequest(StringBuilder &sb) const
{
    sb.AppendFormat("fn on_remote_request(stub: &dyn %s, code: u32, data: &BorrowedMsgParcel,\n",
        interfaceName_.c_str());
    sb.Append("    reply: &mut BorrowedMsgParcel) -> Result<()> {\n");
    sb.Append("    match code {\n");
    AddRemoteRequestMethods(sb);
    sb.Append("        _ => Err(-1)\n");
    sb.Append("    }\n");
    sb.Append("}\n");
}

void SaRustInterfaceCodeEmitter::AddRemoteRequestMethods(StringBuilder &sb) const
{
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.AppendFormat("        %d => {\n", i + 1);
        int paramNumber = static_cast<int>(method->GetParameterNumber());
        for (int j = 0; j < paramNumber; j++) {
            AutoPtr<ASTParameter> param = method->GetParameter(j);
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            if (typeEmitter != nullptr) {
                typeEmitter->EmitRustReadVar("data", GetNameFromParameter(param->GetName()), sb, "            ");
            }
        }
        AutoPtr<ASTType> returnType = method->GetReturnType();
        TypeKind retTypeKind = returnType->GetTypeKind();
        if ((retTypeKind != TypeKind::TYPE_UNKNOWN) && (retTypeKind != TypeKind::TYPE_VOID)) {
            sb.AppendFormat("            let result = stub.%s(", method->GetName().c_str());
        } else {
            sb.AppendFormat("            stub.%s(", method->GetName().c_str());
        }
        AddRemoteRequestParameters(sb, method);
        sb.Append(")?;\n");
        if ((retTypeKind != TypeKind::TYPE_UNKNOWN) && (retTypeKind != TypeKind::TYPE_VOID)) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
            typeEmitter->EmitRustWriteVar("reply", "result", sb, "            ");
        }
        sb.Append("            Ok(())\n");
        sb.Append("        }\n");
    }
}

void SaRustInterfaceCodeEmitter::AddRemoteRequestParameters(StringBuilder &sb, AutoPtr<ASTMethod> &method) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        sb.AppendFormat("&%s", GetNameFromParameter(param->GetName()).c_str());
        if (i + 1 != paramNumber) {
            WrapLine(sb, i, "                ");
        }
    }
}

void SaRustInterfaceCodeEmitter::EmitStub(StringBuilder &sb) const
{
    sb.AppendFormat("impl %s for RemoteStub<%s> {\n", interfaceName_.c_str(), stubName_.c_str());
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.AppendFormat("    fn %s(&self", method->GetName().c_str());
        AppendBrokerParameters(sb, method);
        AutoPtr<ASTType> returnType = method->GetReturnType();
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter == nullptr) {
            return;
        }
        sb.AppendFormat(") -> Result<%s> {\n", typeEmitter->EmitRustType().c_str());
        sb.AppendFormat("        self.0.%s(", method->GetName().c_str());
        AppendStubParameters(sb, method);
        sb.Append(")\n");
        sb.Append("    }\n");
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
    sb.Append("}\n");
}

void SaRustInterfaceCodeEmitter::AppendStubParameters(StringBuilder &sb, AutoPtr<ASTMethod> &method) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        sb.Append(GetNameFromParameter(param->GetName()));
        if (i + 1 != paramNumber) {
            WrapLine(sb, i, "                ");
        }
    }
}

void SaRustInterfaceCodeEmitter::EmitProxy(StringBuilder &sb) const
{
    sb.AppendFormat("impl %s for %s {\n", interfaceName_.c_str(), proxyName_.c_str());
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitProxyMethodImpl(method, sb);
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
    sb.Append("}\n");
}

void SaRustInterfaceCodeEmitter::EmitProxyMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb) const
{
    sb.AppendFormat("    fn %s(&self", method->GetName().c_str());
    AppendBrokerParameters(sb, method);

    AutoPtr<ASTType> returnType = method->GetReturnType();
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
    if (typeEmitter == nullptr) {
        return;
    }
    sb.AppendFormat(") -> Result<%s> {\n", typeEmitter->EmitRustType().c_str());
    sb.Append("        let mut data = MsgParcel::new().expect(\"MsgParcel should success\");\n");
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int j = 0; j < paramNumber; j++) {
        AutoPtr<ASTParameter> param = method->GetParameter(j);
        typeEmitter = GetTypeEmitter(param->GetType());
        if (typeEmitter == nullptr) {
            return;
        }
        typeEmitter->EmitRustWriteVar("data", GetNameFromParameter(param->GetName()), sb, "        ");
    }
    TypeKind retTypeKind = returnType->GetTypeKind();
    if ((retTypeKind == TypeKind::TYPE_UNKNOWN) || (retTypeKind == TypeKind::TYPE_VOID)) {
        sb.AppendFormat("        let _reply = self.remote.send_request(%sCode", interfaceName_.c_str());
    } else {
        sb.AppendFormat("        let reply = self.remote.send_request(%sCode", interfaceName_.c_str());
    }

    sb.AppendFormat("::%s as u32, &data, ", GetCodeFromMethod(method->GetName()).c_str());
    if (method->IsOneWay()) {
        sb.Append("true");
    } else {
        sb.Append("false");
    }
    sb.Append(")?;\n");
    if ((retTypeKind == TypeKind::TYPE_UNKNOWN) || (retTypeKind == TypeKind::TYPE_VOID)) {
        sb.Append("        ").Append("Ok(())\n");
    } else {
        typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter == nullptr) {
            return;
        }
        typeEmitter->EmitRustReadVar("reply", "result", sb, "        ");
        sb.Append("        ").Append("Ok(result)\n");
    }
    sb.Append("    }\n");
}
} // namespace Idl
} // namespace OHOS