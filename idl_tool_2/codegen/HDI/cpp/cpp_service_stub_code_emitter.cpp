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

#include "cpp_service_stub_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool CppServiceStubCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFileParentPath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CppServiceStubCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void CppServiceStubCodeEmitter::EmitCode()
{
    if (mode_ == GenMode::IPC) {
        EmitStubHeaderFile();
        EmitStubSourceFile();
    }
}

void CppServiceStubCodeEmitter::EmitStubHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, stubFullName_);
    sb.Append("\n");
    EmitStubHeaderInclusions(sb);
    sb.Append("\n");
    EmitStubDecl(sb);
    sb.Append("\n");
    EmitTailMacro(sb, stubFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CppServiceStubCodeEmitter::EmitStubHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(interfaceName_));
    if (interface_->GetExtendsInterface() != nullptr) {
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE,
            EmitHeaderNameByInterface(interface_->GetExtendsInterface(), stubName_));
    }
    GetHeaderOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CppServiceStubCodeEmitter::GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_parcel");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_option");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "ipc_object_stub");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "object_collector");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "refbase");
}

void CppServiceStubCodeEmitter::EmitStubDecl(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitStubUsingNamespace(sb);
    sb.AppendFormat("class %s : public IPCObjectStub {\n", stubName_.c_str());
    EmitStubBody(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void CppServiceStubCodeEmitter::EmitStubUsingNamespace(StringBuilder &sb) const
{
    sb.Append("using namespace OHOS;\n");
}

void CppServiceStubCodeEmitter::EmitStubBody(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append("public:\n");
    EmitStubConstructorDecl(sb, prefix);
    sb.Append("\n");
    EmitStubOnRequestDecl(sb, prefix);
    sb.Append("\n");
    EmitStubMethodDecls(sb, prefix);
    sb.Append("\n");
    EmitStubPrivateData(sb, prefix);
}

void CppServiceStubCodeEmitter::EmitStubConstructorDecl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("explicit %s(const sptr<%s> &impl);\n", stubName_.c_str(), interfaceName_.c_str());
    sb.Append(prefix).AppendFormat("virtual ~%s();\n", stubName_.c_str());
}

void CppServiceStubCodeEmitter::EmitStubOnRequestDecl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, ");
    sb.Append("MessageOption &option) override;\n");
}

void CppServiceStubCodeEmitter::EmitStubMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        EmitStubStaticMethodDecl(method, sb, prefix);
        sb.Append("\n");
    }
    if (interface_->GetExtendsInterface() == nullptr) {
        EmitStubStaticMethodDecl(interface_->GetVersionMethod(), sb, prefix);
        sb.Append("\n");
    }

    sb.Append("private:\n");
    AutoPtr<ASTInterfaceType> interface = interface_;
    while (interface != nullptr) {
        for (const auto &method : interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
            EmitStubMethodDecl(method, sb, prefix);
            sb.Append("\n");
        }
        interface = interface->GetExtendsInterface();
    }
    EmitStubMethodDecl(interface_->GetVersionMethod(), sb, prefix);
    sb.Append("\n");
}

void CppServiceStubCodeEmitter::EmitStubMethodDecl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("int32_t %s%s%s(MessageParcel& %s, MessageParcel& %s, MessageOption& %s);\n",
        stubName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str(),
        HdiTypeEmitter::dataParcelName_.c_str(), HdiTypeEmitter::replyParcelName_.c_str(), optionName_.c_str());
}

void CppServiceStubCodeEmitter::EmitStubStaticMethodDecl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "static int32_t %s%s%s_(MessageParcel& %s, MessageParcel& %s, MessageOption& %s, sptr<%s> impl);\n",
        stubName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str(),
        HdiTypeEmitter::dataParcelName_.c_str(), HdiTypeEmitter::replyParcelName_.c_str(),
        optionName_.c_str(), EmitDefinitionByInterface(interface_, interfaceName_).c_str());
}

void CppServiceStubCodeEmitter::EmitStubPrivateData(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("static inline ObjectDelegator<%s, %s> objDelegator_;\n",
        EmitDefinitionByInterface(interface_, stubName_).c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).AppendFormat("sptr<%s> impl_;\n", EmitDefinitionByInterface(interface_, interfaceName_).c_str());
}

void CppServiceStubCodeEmitter::EmitStubSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitStubSourceInclusions(sb);
    sb.Append("\n");
    EmitLogTagMacro(sb, FileName(stubName_));
    sb.Append("\n");
    EmitBeginNamespace(sb);
    EmitUtilMethods(sb, true);
    sb.Append("\n");
    EmitUtilMethods(sb, false);
    sb.Append("\n");
    EmitInterfaceGetMethodImpl(sb, "");
    sb.Append("\n");
    EmitStubConstructorImpl(sb, "");
    sb.Append("\n");
    EmitStubOnRequestMethodImpl(sb, "");
    sb.Append("\n");
    EmitStubMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CppServiceStubCodeEmitter::EmitStubSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(stubName_));
    GetSourceOtherLibInclusions(headerFiles);
    GetSourceOtherFileInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CppServiceStubCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    if (!interface_->IsSerializable()) {
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "string_ex");
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdi_support");
    } else {
        const AST::TypeStringMap &types = ast_->GetTypes();
        for (const auto &pair : types) {
            AutoPtr<ASTType> type = pair.second;
            if (type->GetTypeKind() == TypeKind::TYPE_UNION) {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
                break;
            }
        }
    }

    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        for (size_t paramIndex = 0; paramIndex < method->GetParameterNumber(); paramIndex++) {
            AutoPtr<ASTParameter> param = method->GetParameter(paramIndex);
            AutoPtr<ASTType> paramType = param->GetType();
            if ((param->GetAttribute() == ASTParamAttr::PARAM_IN) &&
                (param->GetType()->IsInterfaceType() || paramType->HasInnerType(TypeKind::TYPE_INTERFACE))) {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "iproxy_broker");
            }

            if ((param->GetAttribute() == ASTParamAttr::PARAM_OUT) &&
                (param->GetType()->IsInterfaceType() || paramType->HasInnerType(TypeKind::TYPE_INTERFACE))) {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "object_collector");
            }
        }
    }

    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_core_log");
}

void CppServiceStubCodeEmitter::GetSourceOtherFileInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        for (size_t paramIndex = 0; paramIndex < method->GetParameterNumber(); paramIndex++) {
            AutoPtr<ASTParameter> param = method->GetParameter(paramIndex);
            AutoPtr<ASTType> paramType = param->GetType();
            if ((param->GetAttribute() == ASTParamAttr::PARAM_IN) &&
                (param->GetType()->IsInterfaceType() || paramType->HasInnerType(TypeKind::TYPE_INTERFACE))) {
                AutoPtr<ASTInterfaceType> type = dynamic_cast<ASTInterfaceType *>(paramType.Get());
                std::string FileName = InterfaceToFilePath(paramType->ToString());
                headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, FileName);
            }
        }
    }
}

void CppServiceStubCodeEmitter::EmitInterfaceGetMethodImpl(StringBuilder &sb, const std::string &prefix) const
{
    if (!interface_->IsSerializable()) {
        EmitGetMethodImpl(sb, prefix);
        sb.Append(prefix).Append("\n");
        EmitGetInstanceMethodImpl(sb, prefix);
        sb.Append(prefix).Append("\n");
    }
}

void CppServiceStubCodeEmitter::EmitGetMethodImpl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("sptr<%s> %s::Get(bool isStub)\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("return %s::Get(\"%s\", isStub);\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str(), FileName(implName_).c_str());
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitGetInstanceMethodImpl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("sptr<%s> %s::Get(const std::string& serviceName, bool isStub)\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).Append("{\n");

    sb.Append(prefix + TAB).Append("if (!isStub) {\n");
    sb.Append(prefix + TAB + TAB).Append("return nullptr;\n");
    sb.Append(prefix + TAB).Append("}\n");

    sb.Append(prefix + TAB).AppendFormat("std::string desc = Str16ToStr8(%s::GetDescriptor());\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix + TAB).Append("void *impl = LoadHdiImpl(desc.c_str(), ");
    sb.AppendFormat("serviceName == \"%s\" ? \"service\" : serviceName.c_str());\n", FileName(implName_).c_str());
    sb.Append(prefix + TAB).Append("if (impl == nullptr) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"failed to load hdi impl %{public}s\", desc.c_str());\n");
    sb.Append(prefix + TAB + TAB).Append("return nullptr;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("return reinterpret_cast<%s *>(impl);\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubConstructorImpl(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s::%s(const sptr<%s> &impl)\n",
        EmitDefinitionByInterface(interface_, stubName_).c_str(), stubName_.c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix + TAB).AppendFormat(": IPCObjectStub(%s::GetDescriptor()), impl_(impl)\n",
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat(
        "%s::~%s()\n", EmitDefinitionByInterface(interface_, stubName_).c_str(), stubName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("HDF_LOGI(\"%{public}s enter\", __func__);\n");
    sb.Append(prefix + TAB).Append("ObjectCollector::GetInstance().RemoveObject(impl_);\n");
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix)
{
    sb.Append(prefix).AppendFormat(
        "int32_t %s::OnRemoteRequest(uint32_t code, ", EmitDefinitionByInterface(interface_, stubName_).c_str());
    sb.Append("MessageParcel& data, MessageParcel& reply, MessageOption& option)\n");
    sb.Append(prefix).Append("{\n");

    sb.Append(prefix + TAB).Append("switch (code) {\n");
    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(getVerMethod).c_str());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("return %sStub%s%s(data, reply, option);\n",
        baseName_.c_str(), getVerMethod->GetName().c_str(), getVerMethod->GetMethodIdentifier().c_str());
    AutoPtr<ASTInterfaceType> interface = interface_;
    while (interface != nullptr) {
        for (const auto &method : interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
            sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(method).c_str());
            sb.Append(prefix + TAB + TAB + TAB)
                .AppendFormat("return %sStub%s%s(data, reply, option);\n",
                baseName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str());
        }
        interface = interface->GetExtendsInterface();
    }

    sb.Append(prefix + TAB + TAB).Append("default: {\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .Append("HDF_LOGE(\"%{public}s: cmd %{public}d is not supported\", __func__, code);\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("return IPCObjectStub::OnRemoteRequest(code, data, reply, option);\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTInterfaceType> interface = interface_;
    AutoPtr<ASTInterfaceType> mataInterface = interface_;
    while (interface != nullptr) {
        for (const auto &method : interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
            EmitStubMethodImpl(interface, method, sb, prefix);
            sb.Append("\n");
        }
        interface = interface->GetExtendsInterface();
        if (interface != nullptr) {
            mataInterface = interface;
        }
    }
    AutoPtr<ASTMethod> verMethod = interface_->GetVersionMethod();
    EmitStubMethodImpl(mataInterface, verMethod, sb, prefix);
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        sb.Append("\n");
        EmitStubStaticMethodImpl(method, sb, prefix);
    }
    if (interface_->GetExtendsInterface() == nullptr) {
        sb.Append("\n");
        EmitStubStaticMethodImpl(verMethod, sb, prefix);
    }
    return;
}

void CppServiceStubCodeEmitter::EmitStubMethodImpl(AutoPtr<ASTInterfaceType> interface,
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("int32_t %s::%s%s%s(MessageParcel& %s, MessageParcel& %s, MessageOption& %s)\n",
        EmitDefinitionByInterface(interface_, stubName_).c_str(),
        stubName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str(),
        HdiTypeEmitter::dataParcelName_.c_str(), HdiTypeEmitter::replyParcelName_.c_str(), optionName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("return %s::%s%s%s_(%s, %s, %s, impl_);\n",
        EmitDefinitionByInterface(interface, stubName_).c_str(),
        stubName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str(),
        HdiTypeEmitter::dataParcelName_.c_str(), HdiTypeEmitter::replyParcelName_.c_str(),
        optionName_.c_str());
    sb.Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubStaticMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "int32_t %s::%s%s%s_(MessageParcel& %s, MessageParcel& %s, MessageOption& %s, sptr<%s> impl)\n",
        EmitDefinitionByInterface(interface_, stubName_).c_str(),
        stubName_.c_str(), method->GetName().c_str(), method->GetMethodIdentifier().c_str(),
        HdiTypeEmitter::dataParcelName_.c_str(), HdiTypeEmitter::replyParcelName_.c_str(), optionName_.c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix).Append("{\n");

    // read interface token and check it
    EmitStubReadInterfaceToken(HdiTypeEmitter::dataParcelName_, sb, prefix + TAB);
    sb.Append("\n");

    EmitStubReadMemFlag(method, HdiTypeEmitter::dataParcelName_, sb, prefix + TAB);

    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() == ASTParamAttr::PARAM_IN) {
            EmitReadMethodParameter(param, TypeMode::PARAM_IN, sb, prefix + TAB);
            sb.Append("\n");
        } else {
            EmitLocalVariable(param, HdiTypeEmitter::dataParcelName_, sb, prefix + TAB);
            sb.Append("\n");
        }
    }

    EmitStubCallMethod(method, sb, prefix + TAB);
    sb.Append("\n");

    if (!method->IsOneWay()) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() == ASTParamAttr::PARAM_OUT) {
                EmitWriteMethodParameter(param, HdiTypeEmitter::replyParcelName_, sb, prefix + TAB);
                sb.Append("\n");
            }
        }
    }

    sb.Append(prefix + TAB).AppendFormat("return %s;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubCallMethod(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("if (impl == nullptr) {\n");
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: impl is nullptr!\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("int32_t %s = impl->%s(", HdiTypeEmitter::errorCodeName_.c_str(),
        method->GetName().c_str());
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        sb.Append(param->GetName());
        if (i + 1 < method->GetParameterNumber()) {
            sb.Append(", ");
        }
    }
    sb.Append(");\n");

    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s failed, error code is %%{public}d\", __func__, %s);\n",
        HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubReadInterfaceToken(
    const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s.ReadInterfaceToken() != %s::GetDescriptor()) {\n", parcelName.c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: interface token check failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitStubReadMemFlag(const AutoPtr<ASTMethod> &method,
    const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const
{
    if (NeedFlag(method)) {
        sb.Append(prefix).AppendFormat("bool %s = false;\n", flagOfSetMemName_.c_str());
        sb.Append(prefix).AppendFormat("if (!%s.ReadBool(%s)) {\n", parcelName.c_str(), flagOfSetMemName_.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", flagOfSetMemName_.c_str());
        sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix).Append("}\n");
    }
}

void CppServiceStubCodeEmitter::EmitLocalVariable(const AutoPtr<ASTParameter> &param,
    const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const
{
    EmitCppLocalVar(param, sb, prefix);
    AutoPtr<ASTType> type = param->GetType();
    if (!type->IsStringType() && !type->IsArrayType() && !type->IsListType()) {
        return;
    }

    sb.Append(prefix).AppendFormat("if (%s) {\n", flagOfSetMemName_.c_str());
    std::string capacityName = "capacity";
    sb.Append(prefix + TAB).AppendFormat("uint32_t %s = 0;\n", capacityName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!%s.ReadUint32(%s)) {\n", parcelName.c_str(), capacityName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n",
        capacityName.c_str());
    sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB).Append("}\n");

    if (type->IsStringType()) {
        sb.Append(prefix + TAB).AppendFormat("%s(%s, >, %s / sizeof(char), HDF_ERR_INVALID_PARAM);\n",
            CHECK_VALUE_RETURN_MACRO, capacityName.c_str(), MAX_BUFF_SIZE_MACRO);
    } else {
        AutoPtr<ASTArrayType> arrayType = dynamic_cast<ASTArrayType *>(type.Get());
        sb.Append(prefix + TAB).AppendFormat("%s(%s, >, %s / sizeof(%s), HDF_ERR_INVALID_PARAM);\n",
            CHECK_VALUE_RETURN_MACRO, capacityName.c_str(), MAX_BUFF_SIZE_MACRO,
            GetTypeEmitter(arrayType->GetElementType())->EmitCppType().c_str());
    }
    sb.Append(prefix + TAB).AppendFormat("%s.reserve(%s);\n", param->GetName().c_str(), capacityName.c_str());
    sb.Append(prefix).Append("}\n");
}

void CppServiceStubCodeEmitter::EmitCppLocalVar(const AutoPtr<ASTParameter> &param,
    StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    sb.Append(prefix).AppendFormat("%s %s", typeEmitter->EmitCppType(TypeMode::LOCAL_VAR).c_str(),
        param->GetName().c_str());
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
            sb.Append(" = false");
            break;
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
            sb.Append(" = 0");
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(" = -1");
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(" = nullptr");
            break;
        default:
            break;
    }
    sb.Append(";\n");
}

void CppServiceStubCodeEmitter::EmitUtilMethods(StringBuilder &sb, bool isDecl)
{
    UtilMethodMap methods;
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        for (size_t paramIndex = 0; paramIndex < method->GetParameterNumber(); paramIndex++) {
            AutoPtr<ASTParameter> param = method->GetParameter(paramIndex);
            AutoPtr<ASTType> paramType = param->GetType();
            AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            if (param->GetAttribute() == ASTParamAttr::PARAM_IN) {
                typeEmitter->EmitCppReadMethods(methods, "", "", isDecl);
            } else {
                typeEmitter->EmitCppWriteMethods(methods, "", "", isDecl);
            }
        }
    }
    EmitUtilMethodMap(sb, methods);
}
} // namespace Idl
} // namespace OHOS
