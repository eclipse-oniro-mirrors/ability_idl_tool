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

#include "sa_cpp_service_stub_code_emitter.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {
void SaCppServiceStubCodeEmitter::EmitCode()
{
    SaTypeEmitter::circleCount_ = 0;
    EmitStubHeaderFile();
    EmitStubSourceFile();
}

void SaCppServiceStubCodeEmitter::EmitStubHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, stubFullName_);
    sb.Append("\n");
    sb.Append("#include <iremote_stub.h>\n");
    EmitSecurecInclusion(sb);
    sb.AppendFormat("#include \"%s.h\"\n", FileName(interfaceName_).c_str());
    sb.Append("\n");
    EmitInterfaceStubInHeaderFile(sb);
    EmitTailMacro(sb, stubFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubInHeaderFile(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    EmitImportUsingNamespace(sb);
    sb.AppendFormat("class %s : public IRemoteStub<%s> {\n", stubName_.c_str(), interfaceName_.c_str());
    sb.Append("public:\n");
    EmitInterfaceStubMethodDecls(sb, TAB);
    EmitInterfaceProxyIpcCapacityValues(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void SaCppServiceStubCodeEmitter::EmitInterfaceProxyIpcCapacityValues(StringBuilder &sb, const std::string &prefix)
{
    StringBuilder values;
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (method->HasIpcOutCapacity()) {
            values.Append(prefix).AppendFormat("static constexpr size_t CAPACITY_%s_%d = %s << 10;\n",
                ConstantName(method->GetName()).c_str(), method->GetIpcCode(), method->GetIpcOutCapacity().c_str());
        }
    }

    std::string valuesStr = values.ToString();
    if (!valuesStr.empty()) {
        sb.Append("private:\n").Append(valuesStr);
    }
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s(bool serialInvokeFlag = false): IRemoteStub(serialInvokeFlag){};\n",
        stubName_.c_str());
    sb.Append(prefix).Append("int32_t OnRemoteRequest(\n");
    sb.Append(prefix + TAB).Append("uint32_t code,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& data,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& reply,\n");
    sb.Append(prefix + TAB).Append("MessageOption& option) override;\n");
    if (ast_ != nullptr && ast_->GetOptionStubHooksOn()) {
        sb.Append(prefix).Append("virtual int32_t CallbackEnter([[maybe_unused]] uint32_t code) = 0;\n");
        sb.Append(prefix).Append(
            "virtual int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) = 0;\n");
        sb.Append(prefix).Append("int32_t OnRemoteRequestInner(\n");
        sb.Append(prefix + TAB).Append("uint32_t code,\n");
        sb.Append(prefix + TAB).Append("MessageParcel& data,\n");
        sb.Append(prefix + TAB).Append("MessageParcel& reply,\n");
        sb.Append(prefix + TAB).Append("MessageOption& option);\n");
    }
    if (ast_ != nullptr && ast_->GetOptionParcelHooksOn()) {
        sb.Append(prefix).Append("virtual int32_t CallbackParcel(\n");
        sb.Append(prefix + TAB).Append("[[maybe_unused]] uint32_t code,\n");
        sb.Append(prefix + TAB).Append("[[maybe_unused]] MessageParcel& data,\n");
        sb.Append(prefix + TAB).Append("[[maybe_unused]] MessageParcel& reply,\n");
        sb.Append(prefix + TAB).Append("[[maybe_unused]] MessageOption& option) = 0;\n");
    }
}

void SaCppServiceStubCodeEmitter::EmitStubSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    sb.AppendFormat("#include \"%s.h\"\n", FileName(stubName_).c_str());
    if (logOn_) {
        sb.Append("#include \"hilog/log.h\"\n");
    }
    if (hitraceOn_) {
        sb.Append("#include \"hitrace_meter.h\"\n");
    }
    sb.Append("\n");
    if (logOn_) {
        sb.Append(macroDefine_.c_str());
    }
    EmitBeginNamespace(sb);
    EmitImportUsingNamespace(sb);
    EmitInterfaceStubMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("int32_t %s::OnRemoteRequest(\n", stubName_.c_str());
    sb.Append(prefix + TAB).Append("uint32_t code,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& data,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& reply,\n");
    sb.Append(prefix + TAB).Append("MessageOption& option)\n");
    sb.Append(prefix).Append("{\n");
    if (ast_ != nullptr && ast_->GetOptionStubHooksOn()) {
        EmitInterfaceStubUseHooks(sb, prefix);
    }
    if (ast_ != nullptr && ast_->GetOptionParcelHooksOn()) {
        EmitInterfaceParcelUseHooks(sb, prefix);
    }
    if (hitraceOn_) {
    sb.Append(prefix + TAB).AppendFormat("HITRACE_METER_NAME(%s, __PRETTY_FUNCTION__);\n",
        hitraceTag_.c_str());
    }
    sb.Append(prefix + TAB).Append("std::u16string localDescriptor = GetDescriptor();\n");
    sb.Append(prefix + TAB).Append("std::u16string remoteDescriptor = data.ReadInterfaceToken();\n");
    sb.Append(prefix + TAB).Append("if (localDescriptor != remoteDescriptor) {\n");
    sb.Append(prefix + TAB).Append(TAB).Append("return ERR_TRANSACTION_FAILED;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("switch (static_cast<%sIpcCode>(code)) {\n", interface_->GetName().c_str());
    size_t methodNumber = interface_->GetMethodNumber();
    for (size_t i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        std::string overloadname = "";
        SACppCodeEmitter::CheckMethodOverload(method, i, overloadname);
        SACppCodeEmitter::SetOverloadName(overloadname);
        EmitInterfaceStubMethodImpl(method, sb, prefix + TAB + TAB);
    }
    sb.Append(prefix + TAB).Append(TAB).Append("default:\n");
    sb.Append(prefix + TAB).Append(TAB).Append(TAB).Append(
        "return IPCObjectStub::OnRemoteRequest(code, data, reply, option);\n");
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).Append("return ERR_TRANSACTION_FAILED;\n");
    sb.Append(prefix).Append("}\n");
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubUseHooks(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix + TAB).Append("int32_t enterRet = CallbackEnter(code);\n");
    sb.Append(prefix + TAB).Append("if (enterRet != ERR_NONE) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append(macroError_.c_str()).Append(", \"CallbackEnter failed\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return enterRet;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("int32_t ret = OnRemoteRequestInner(code, data, reply, option);\n\n");
    sb.Append(prefix + TAB).Append("int32_t exitRet = CallbackExit(code, ret);\n");
    sb.Append(prefix + TAB).Append("if (exitRet != ERR_NONE) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append(macroError_.c_str()).Append(", \"CallbackExit failed\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return exitRet;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("return ret;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("int32_t %s::OnRemoteRequestInner(\n", stubName_.c_str());
    sb.Append(prefix + TAB).Append("uint32_t code,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& data,\n");
    sb.Append(prefix + TAB).Append("MessageParcel& reply,\n");
    sb.Append(prefix + TAB).Append("MessageOption& option)\n");
    sb.Append(prefix).Append("{\n");
}

void SaCppServiceStubCodeEmitter::EmitInterfaceParcelUseHooks(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix + TAB).Append("int32_t parcelRet = CallbackParcel(code, data, reply, option);\n");
    sb.Append(prefix + TAB).Append("if (parcelRet != ERR_NONE) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB + TAB).Append(macroError_.c_str()).Append(", \"CallbackParcel failed\");\n");
    }
    sb.Append(prefix + TAB + TAB).Append("return parcelRet;\n");
    sb.Append(prefix + TAB).Append("}\n");
}

void SaCppServiceStubCodeEmitter::EmitInterfaceSetIpcCapacity(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    std::string capacity = StringHelper::Format("CAPACITY_%s_%d", ConstantName(method->GetName()).c_str(),
        method->GetIpcCode());
    sb.Append(prefix).AppendFormat("if (!reply.SetMaxCapacity(%s)) {\n", capacity.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroError_.c_str()).
            AppendFormat(", \"Failed to set maximum capacity to %%zu\", %s);\n", capacity.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix).Append("}\n");
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    bool hasOutParameter = false;
    if (method->IsMacro()) {
        sb.AppendFormat("#%s %s\n", method->GetMacroType().c_str(), method->GetMacroVal().c_str());
    }
    std::string overloadname = SACppCodeEmitter::GetOverloadName();
    sb.Append(prefix).AppendFormat("case %sIpcCode::COMMAND_%s: {\n", interface_->GetName().c_str(),
        ConstantName(method->GetName() + overloadname).c_str());
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            EmitReadMethodParameter(param, "data.", true, sb, prefix + TAB);
            if (param->GetAttribute() == ASTParamAttr::PARAM_INOUT) {
                hasOutParameter = true;
            }
        } else if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
            hasOutParameter = true;
            EmitLocalVariable(param, sb, prefix + TAB);
        }
    }
    EmitInterfaceStubMethodService(method, sb, prefix, hasOutParameter);
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubMethodService(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool hasOutParameter) const
{
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    EmitInterfaceStubMethodCall(method, sb, prefix);
    AutoPtr<ASTType> returnType = method->GetReturnType();
    TypeKind retTypeKind = returnType->GetTypeKind();
    if (hasOutParameter || (retTypeKind != TypeKind::TYPE_VOID)) {
        sb.Append(prefix + TAB).Append("if (SUCCEEDED(errCode)) {\n");
        if (method->HasIpcOutCapacity()) {
            EmitInterfaceSetIpcCapacity(method, sb, prefix + TAB + TAB);
        }
        for (int i = 0; i < paramNumber; i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            const std::string name = param->GetName();
            if (param->GetAttribute() & ASTParamAttr::PARAM_OUT) {
                EmitWriteMethodParameter(param, "reply.", sb, prefix + TAB + TAB);
                EmitInterfaceStubFdClose(param->GetType()->GetTypeKind(), name, sb, prefix);
            }
        }
        if (retTypeKind != TypeKind::TYPE_VOID) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
            if (typeEmitter == nullptr) {
                return;
            }
            typeEmitter->EmitCppWriteVar("reply.", "result", sb, prefix + TAB + TAB);
        }
        EmitInterfaceStubFdClose(retTypeKind, "result", sb, prefix);
        sb.Append(prefix + TAB).Append("}\n");
    }
    sb.Append(prefix + TAB).Append("return ERR_NONE;\n");
    sb.Append(prefix).Append("}\n");
    if (method->IsMacro()) {
        sb.Append("#endif\n");
    }
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubFdClose(TypeKind kind, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    if (kind == TypeKind::TYPE_FILEDESCRIPTOR) {
        sb.Append(prefix + TAB + TAB).AppendFormat("if (%s >= 0) {\n", name.c_str());
        sb.Append(prefix + TAB + TAB).Append(TAB).AppendFormat("close(%s);\n", name.c_str());
        sb.Append(prefix + TAB + TAB).Append("}\n");
    } else if (kind == TypeKind::TYPE_FILEDESCRIPTORSAN) {
        sb.Append(prefix + TAB + TAB).AppendFormat("if (%s >= 0) {\n", name.c_str());
        sb.Append(prefix + TAB + TAB).Append(TAB).AppendFormat("fdsan_close_with_tag(%s, %s);\n",
            name.c_str(), domainId_.c_str());
        sb.Append(prefix + TAB + TAB).Append("}\n");
    }
}

void SaCppServiceStubCodeEmitter::EmitInterfaceStubMethodCall(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    AutoPtr<ASTType> returnType = method->GetReturnType();
    TypeKind retTypeKind = returnType->GetTypeKind();
    if (retTypeKind != TypeKind::TYPE_VOID) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(returnType);
        if (typeEmitter == nullptr) {
            return;
        }
        if (retTypeKind == TypeKind::TYPE_INTERFACE ||
                ((retTypeKind == TypeKind::TYPE_SEQUENCEABLE) &&
                (typeEmitter->EmitCppType() == "sptr<IRemoteObject>"))) {
            sb.Append(prefix + TAB).AppendFormat("%s result = nullptr;\n", typeEmitter->EmitCppType().c_str());
        } else {
            sb.Append(prefix + TAB).AppendFormat("%s result;\n", typeEmitter->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        }
    }
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    sb.Append(prefix + TAB).AppendFormat("ErrCode errCode = %s(", method->GetName().c_str());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        const std::string name = param->GetName();
        AutoPtr<ASTType> type = param->GetType();
        if ((type->GetTypeKind() == TypeKind::TYPE_SEQUENCEABLE) && (param->GetAttribute() & ASTParamAttr::PARAM_IN)) {
            std::string parameterName = "*" + name;
            if (type->GetName() == "IRemoteObject") {
                parameterName = name;
            } else {
                parameterName = "*" + name;
            }
            sb.Append(parameterName.c_str());
        } else {
            sb.Append(name.c_str());
        }

        if ((i != paramNumber - 1) || (retTypeKind != TypeKind::TYPE_VOID)) {
            sb.Append(", ");
        }
    }
    if (retTypeKind != TypeKind::TYPE_VOID) {
        EmitSaReturnParameter("result", retTypeKind, sb);
    }
    sb.Append(");\n");
    sb.Append(prefix + TAB).Append("if (!reply.WriteInt32(errCode)) {\n");
    if (logOn_) {
        sb.Append(prefix + TAB).Append(TAB).Append(macroError_.c_str()).Append(", \"Write Int32 failed!\");\n");
    }
    sb.Append(prefix + TAB).Append(TAB).Append("return ERR_INVALID_VALUE;\n");
    sb.Append(prefix + TAB).Append("}\n");
}

void SaCppServiceStubCodeEmitter::EmitLocalVariable(const AutoPtr<ASTParameter> &param, StringBuilder &sb,
    const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    const std::string name = param->GetName();
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr || type == nullptr) {
        return;
    }
    if (type->GetTypeKind() == TypeKind::TYPE_FILEDESCRIPTOR) {
        sb.Append(prefix).AppendFormat("%s %s = -1;\n",
            typeEmitter->EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s %s;\n", typeEmitter->EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    }
}

void SaCppServiceStubCodeEmitter::EmitSaReturnParameter(const std::string &name, const TypeKind kind,
    StringBuilder &sb)
{
    switch (kind) {
        case TypeKind::TYPE_CHAR:
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_CSTRING:
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_U16STRING:
        case TypeKind::TYPE_SEQUENCEABLE:
        case TypeKind::TYPE_RAWDATA:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_LIST:
        case TypeKind::TYPE_MAP:
        case TypeKind::TYPE_ORDEREDMAP:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_SET:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_SHAREDPTR:
        case TypeKind::TYPE_UNIQUEPTR:
        case TypeKind::TYPE_SPTR:
        case TypeKind::TYPE_NULL_SHAREDPTR:
        case TypeKind::TYPE_NULL_UNIQUEPTR:
        case TypeKind::TYPE_NULL_SPTR:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_FILEDESCRIPTOR:
        case TypeKind::TYPE_FILEDESCRIPTORSAN:
        case TypeKind::TYPE_ENUM:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
            sb.Append(name.c_str());
            break;
        default:
            break;
    }
}
} // namespace Idl
} // namespace OHOS
