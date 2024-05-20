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

#include "c_service_stub_code_emitter.h"

#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool CServiceStubCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFileParentPath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CServiceStubCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void CServiceStubCodeEmitter::EmitCode()
{
    switch (mode_) {
        case GenMode::IPC:
        case GenMode::KERNEL: {
            EmitServiceStubHeaderFile();
            EmitServiceStubSourceFile();
            break;
        }
        default:
            break;
    }
}

void CServiceStubCodeEmitter::EmitServiceStubHeaderFile()
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
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitCbServiceStubDef(sb);
    if (mode_ == GenMode::KERNEL) {
        sb.Append("\n");
        EmitCbServiceStubMethodsDcl(sb);
    }
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, stubFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CServiceStubCodeEmitter::EmitStubHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, EmitVersionHeaderName(interfaceName_));
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_sbuf");

    if (mode_ != GenMode::KERNEL) {
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_remote_service");
    }

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CServiceStubCodeEmitter::EmitCbServiceStubDef(StringBuilder &sb) const
{
    sb.AppendFormat("struct %sStub {\n", baseName_.c_str());
    if (mode_ == GenMode::KERNEL) {
        sb.Append(TAB).AppendFormat("struct %s interface;\n", interfaceName_.c_str());
        sb.Append(TAB).AppendFormat("int32_t (*OnRemoteRequest)(struct %s *serviceImpl, ", interfaceName_.c_str());
        sb.Append("int code, struct HdfSBuf *data, struct HdfSBuf *reply);\n");
    } else {
        sb.Append(TAB).Append("struct HdfRemoteService *remote;\n");
        sb.Append(TAB).AppendFormat("struct %s *interface;\n", interfaceName_.c_str());
        sb.Append(TAB).Append("struct HdfRemoteDispatcher dispatcher;\n");
    }
    sb.Append("};\n");
}

void CServiceStubCodeEmitter::EmitCbServiceStubMethodsDcl(StringBuilder &sb) const
{
    sb.AppendFormat("bool %sStubConstruct(struct %sStub *stub);\n", baseName_.c_str(), baseName_.c_str());
}

void CServiceStubCodeEmitter::EmitServiceStubSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.c", directory_.c_str(), FileName(stubName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitStubSourceInclusions(sb);
    sb.Append("\n");
    EmitLogTagMacro(sb, FileName(stubName_));
    if (mode_ != GenMode::KERNEL && !interface_->IsSerializable()) {
        sb.Append("\n");
        EmitExternalMethodImpl(sb);
    }
    sb.Append("\n");
    EmitUtilMethods(sb, true);
    sb.Append("\n");
    EmitUtilMethods(sb, false);
    EmitServiceStubMethodImpls(sb, "");

    if (mode_ == GenMode::KERNEL) {
        sb.Append("\n");
        EmitKernelStubOnRequestMethodImpl(sb, "");
        sb.Append("\n");
        EmitKernelStubConstruct(sb);
    } else {
        sb.Append("\n");
        EmitStubOnRequestMethodImpl(sb, "");
        if (!interface_->IsSerializable()) {
            sb.Append("\n");
            EmitStubRemoteDispatcher(sb);
        }

        sb.Append("\n");
        EmitStubNewInstance(sb);
        sb.Append("\n");
        EmitStubReleaseMethod(sb);
        sb.Append("\n");
        EmitStubConstructor(sb);
        sb.Append("\n");
        EmitStubRegAndUnreg(sb);
    }

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CServiceStubCodeEmitter::EmitStubSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(stubName_));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CServiceStubCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    if (mode_ != GenMode::KERNEL) {
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_dlist");
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "stub_collector");

        if (!interface_->IsSerializable()) {
            headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdi_support");
        }
    } else {
        const AST::TypeStringMap &types = ast_->GetTypes();
        for (const auto &pair : types) {
            AutoPtr<ASTType> type = pair.second;
            if (type->GetTypeKind() == TypeKind::TYPE_STRING || type->GetTypeKind() == TypeKind::TYPE_UNION) {
                headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
                break;
            }
        }
    }

    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_log");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem");
}

void CServiceStubCodeEmitter::EmitExternalMethodImpl(StringBuilder &sb)
{
    EmitGetMethodImpl(sb);
    sb.Append("\n");
    EmitGetInstanceMehtodImpl(sb);
    sb.Append("\n");
    EmitReleaseMethodImpl(sb);
    sb.Append("\n");
    EmitReleaseInstanceMethodImpl(sb);
}

void CServiceStubCodeEmitter::EmitGetMethodImpl(StringBuilder &sb) const
{
    sb.AppendFormat("struct %s *%sGet(bool isStub)\n", interfaceName_.c_str(), interfaceName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat(
        "return %sGetInstance(\"%s\", isStub);\n", interfaceName_.c_str(), FileName(implName_).c_str());
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitGetInstanceMehtodImpl(StringBuilder &sb) const
{
    sb.AppendFormat("struct %s *%sGetInstance(const char *serviceName, bool isStub)\n", interfaceName_.c_str(),
        interfaceName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (!isStub) {\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n\n");
    sb.Append(TAB).AppendFormat("const char *instName = serviceName;\n");
    sb.Append(TAB).AppendFormat("if (strcmp(serviceName, \"%s\") == 0) {\n", FileName(implName_).c_str());
    sb.Append(TAB).Append(TAB).Append("instName = \"service\";\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat("return (struct %s *)LoadHdiImpl(%s, instName);\n",
        interfaceName_.c_str(), EmitDescMacroName().c_str());
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitReleaseMethodImpl(StringBuilder &sb) const
{
    sb.AppendFormat(
        "void %sRelease(struct %s *instance, bool isStub)\n", interfaceName_.c_str(), interfaceName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat(
        "%sReleaseInstance(\"%s\", instance, isStub);\n", interfaceName_.c_str(), FileName(implName_).c_str());
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitReleaseInstanceMethodImpl(StringBuilder &sb) const
{
    sb.AppendFormat("void %sReleaseInstance(const char *serviceName, struct %s *instance, bool isStub)\n",
        interfaceName_.c_str(), interfaceName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (serviceName == NULL || !isStub || instance == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).Append("const char *instName = serviceName;\n");
    sb.Append(TAB).AppendFormat("if (strcmp(serviceName, \"%s\") == 0) {\n", FileName(implName_).c_str());
    sb.Append(TAB).Append(TAB).Append("instName = \"service\";\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat("UnloadHdiImpl(%s, instName, instance);\n", EmitDescMacroName().c_str());
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitServiceStubMethodImpls(StringBuilder &sb, const std::string &prefix)
{
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        EmitServiceStubMethodImpl(method, sb, prefix);
        sb.Append("\n");
    }

    EmitStubGetVerMethodImpl(interface_->GetVersionMethod(), sb, prefix);
    if (mode_ != GenMode::KERNEL) {
        sb.Append("\n");
        EmitStubAsObjectMethodImpl(sb, prefix);
    }
}

void CServiceStubCodeEmitter::EmitServiceStubMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "static int32_t SerStub%s(struct %s *serviceImpl, struct HdfSBuf *%s, struct HdfSBuf *%s)\n",
        method->GetName().c_str(), interfaceName_.c_str(), HdiTypeEmitter::dataParcelName_.c_str(),
        HdiTypeEmitter::replyParcelName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = HDF_FAILURE;\n", HdiTypeEmitter::errorCodeName_.c_str());
    bool readFlag = NeedFlag(method);
    if (readFlag) {
        sb.Append(prefix + TAB).AppendFormat("bool %s = false;\n", flagOfSetMemName_.c_str());
    }

    // Local variable definitions must precede all execution statements.
    EmitMethodNeedLoopVar(method, true, true, sb, prefix + TAB);

    if (method->GetParameterNumber() > 0) {
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitStubLocalVariable(param, sb, prefix + TAB);
        }

        sb.Append("\n");
        EmitReadFlagVariable(readFlag, sb, prefix + TAB);
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            if (param->GetAttribute() == ASTParamAttr::PARAM_IN) {
                EmitReadStubMethodParameter(param, FINISHED_LABEL, sb, prefix + TAB);
                sb.Append("\n");
            } else {
                EmitOutVarMemInitialize(param, FINISHED_LABEL, sb, prefix + TAB);
            }
        }
    }

    EmitStubCallMethod(method, FINISHED_LABEL, sb, prefix + TAB);
    sb.Append("\n");

    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() == ASTParamAttr::PARAM_OUT) {
            AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            typeEmitter->EmitCWriteVar(TypeMode::PARAM_OUT, param->GetName(), FINISHED_LABEL, sb, prefix + TAB);
            sb.Append("\n");
        }
    }

    EmitErrorHandle(method, sb, prefix);
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitErrorHandle(const AutoPtr<ASTMethod> &method,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s:\n", FINISHED_LABEL);
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
        typeEmitter->EmitMemoryRecycle(param->GetName(), true, sb, prefix + TAB);
    }
}

void CServiceStubCodeEmitter::EmitReadFlagVariable(bool readFlag, StringBuilder &sb, const std::string &prefix) const
{
    if (!readFlag) {
        return;
    }

    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint8(%s, (uint8_t *)&%s)) {\n",
        HdiTypeEmitter::dataParcelName_.c_str(), flagOfSetMemName_.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: read flag of memory setting failed!\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", FINISHED_LABEL);
    sb.Append(prefix).Append("}\n\n");
}

void CServiceStubCodeEmitter::EmitStubLocalVariable(
    const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    EmitParamLocalVar(param, sb, prefix);
    if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST ||
        (type->GetTypeKind() == TypeKind::TYPE_STRING && (param->GetAttribute() == ASTParamAttr::PARAM_OUT))) {
        sb.Append(prefix).AppendFormat("uint32_t %sLen = 0;\n", param->GetName().c_str());
    }
}

void CServiceStubCodeEmitter::EmitParamLocalVar(const AutoPtr<ASTParameter> &param, StringBuilder &sb,
    const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    sb.Append(prefix).AppendFormat("%s %s", typeEmitter->EmitCType(TypeMode::LOCAL_VAR).c_str(),
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
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_NATIVE_BUFFER:
            sb.Append(" = NULL");
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(" = -1");
            break;
        default:
            break;
    }
    sb.Append(";\n");
}

void CServiceStubCodeEmitter::EmitReadStubMethodParameter(const AutoPtr<ASTParameter> &param,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);

    if (type->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitReadCStringStubMethodParameter(param, gotoLabel, sb, prefix, typeEmitter);
    } else if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s));\n", param->GetName().c_str(),
            typeEmitter->EmitCType().c_str(), typeEmitter->EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", HdiTypeEmitter::errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", FINISHED_LABEL);
        sb.Append(prefix).Append("}\n");
        typeEmitter->EmitCStubReadVar(param->GetName(), gotoLabel, sb, prefix);
    } else if (type->GetTypeKind() == TypeKind::TYPE_UNION) {
        std::string cpName = StringHelper::Format("%sCp", param->GetName().c_str());
        typeEmitter->EmitCStubReadVar(cpName, gotoLabel, sb, prefix);
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s));\n", param->GetName().c_str(),
            typeEmitter->EmitCType().c_str(), typeEmitter->EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", HdiTypeEmitter::errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", FINISHED_LABEL);
        sb.Append(prefix).Append("}\n");
        sb.Append(prefix).AppendFormat("if (memcpy_s(%s, sizeof(%s), %s, sizeof(%s)) != EOK) {\n",
            param->GetName().c_str(), typeEmitter->EmitCType().c_str(), cpName.c_str(),
            typeEmitter->EmitCType().c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to memcpy %s\", __func__);\n",
            param->GetName().c_str());
        sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix).Append("}\n");
    } else if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST ||
               type->GetTypeKind() == TypeKind::TYPE_FILEDESCRIPTOR ||
               type->GetTypeKind() == TypeKind::TYPE_NATIVE_BUFFER || type->GetTypeKind() == TypeKind::TYPE_ENUM ||
               type->GetTypeKind() == TypeKind::TYPE_INTERFACE) {
        typeEmitter->EmitCStubReadVar(param->GetName(), gotoLabel, sb, prefix);
    } else {
        std::string name = StringHelper::Format("&%s", param->GetName().c_str());
        typeEmitter->EmitCStubReadVar(name, gotoLabel, sb, prefix);
    }
}

void CServiceStubCodeEmitter::EmitReadCStringStubMethodParameter(const AutoPtr<ASTParameter> &param,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix,
    AutoPtr<HdiTypeEmitter> &typeEmitter) const
{
    std::string cloneName = StringHelper::Format("%sCp", param->GetName().c_str());
    typeEmitter->EmitCStubReadVar(cloneName, gotoLabel, sb, prefix);
    if (mode_ == GenMode::KERNEL) {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat(
            "%s = (char*)OsalMemCalloc(strlen(%s) + 1);\n", param->GetName().c_str(), cloneName.c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", HdiTypeEmitter::errorCodeName_.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (strcpy_s(%s, (strlen(%s) + 1), %s) != HDF_SUCCESS) {\n",
            param->GetName().c_str(), cloneName.c_str(), cloneName.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", HdiTypeEmitter::errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("%s = strdup(%s);\n", param->GetName().c_str(), cloneName.c_str());
    }
}

void CServiceStubCodeEmitter::EmitOutVarMemInitialize(const AutoPtr<ASTParameter> &param,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTType> type = param->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (type->IsStructType() || type->IsUnionType()) {
        sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s));\n", param->GetName().c_str(),
            typeEmitter->EmitCType().c_str(), typeEmitter->EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", param->GetName().c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", param->GetName().c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", HdiTypeEmitter::errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n\n");
    } else if (type->IsStringType() || type->IsArrayType() || type->IsListType()) {
        typeEmitter->EmitCStubReadOutVar(flagOfSetMemName_, param->GetName(), gotoLabel, sb, prefix);
        sb.Append("\n");
    }
}

void CServiceStubCodeEmitter::EmitStubCallMethod(
    const AutoPtr<ASTMethod> &method, const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("if (serviceImpl == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid serviceImpl object\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_OBJECT;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("if (serviceImpl->%s == NULL) {\n", method->GetName().c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: invalid interface function %s \", __func__);\n",
        method->GetName().c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_NOT_SUPPORT;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");

    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat(
            "%s = serviceImpl->%s(serviceImpl);\n", HdiTypeEmitter::errorCodeName_.c_str(), method->GetName().c_str());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = serviceImpl->%s(serviceImpl, ", HdiTypeEmitter::errorCodeName_.c_str(), method->GetName().c_str());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitCallParameter(sb, param->GetType(), param->GetAttribute(), param->GetName());
            if (i + 1 < method->GetParameterNumber()) {
                sb.Append(", ");
            }
        }
        sb.AppendFormat(");\n", method->GetName().c_str());
    }

    sb.Append(prefix).AppendFormat("if (%s != HDF_SUCCESS) {\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s failed, error code is %%{public}d\", __func__, %s);\n",
        HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitCallParameter(
    StringBuilder &sb, const AutoPtr<ASTType> &type, ASTParamAttr::ParamAttr attribute, const std::string &name) const
{
    if (attribute == ASTParamAttr::PARAM_OUT) {
        if (type->GetTypeKind() == TypeKind::TYPE_STRING || type->GetTypeKind() == TypeKind::TYPE_ARRAY ||
            type->GetTypeKind() == TypeKind::TYPE_LIST || type->GetTypeKind() == TypeKind::TYPE_STRUCT ||
            type->GetTypeKind() == TypeKind::TYPE_UNION) {
            sb.AppendFormat("%s", name.c_str());
        } else {
            sb.AppendFormat("&%s", name.c_str());
        }

        if (type->GetTypeKind() == TypeKind::TYPE_STRING) {
            sb.AppendFormat(", %sLen", name.c_str());
        } else if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.AppendFormat(", &%sLen", name.c_str());
        }
    }
    if (attribute == ASTParamAttr::PARAM_IN) {
        sb.AppendFormat("%s", name.c_str());
        if (type->GetTypeKind() == TypeKind::TYPE_ARRAY || type->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.AppendFormat(", %sLen", name.c_str());
        }
    }
}

void CServiceStubCodeEmitter::EmitStubGetVerMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "static int32_t SerStub%s(struct %s *serviceImpl, struct HdfSBuf *%s, struct HdfSBuf *%s)\n",
        method->GetName().c_str(), interfaceName_.c_str(), HdiTypeEmitter::dataParcelName_.c_str(),
        HdiTypeEmitter::replyParcelName_.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = HDF_SUCCESS;\n", HdiTypeEmitter::errorCodeName_.c_str());

    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(new ASTUintType());
    typeEmitter->EmitCWriteVar(TypeMode::PARAM_OUT, majorVerName_, FINISHED_LABEL, sb, prefix + TAB);
    sb.Append("\n");
    typeEmitter->EmitCWriteVar(TypeMode::PARAM_OUT, minorVerName_, FINISHED_LABEL, sb, prefix + TAB);
    sb.Append("\n");

    sb.Append(FINISHED_LABEL).Append(":\n");
    sb.Append(prefix + TAB).AppendFormat("return %s;\n", HdiTypeEmitter::errorCodeName_.c_str());
    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubAsObjectMethodImpl(StringBuilder &sb, const std::string &prefix) const
{
    std::string objName = "self";
    sb.Append(prefix).AppendFormat("static struct HdfRemoteService *%sStubAsObject(struct %s *%s)\n", baseName_.c_str(),
        interfaceName_.c_str(), objName.c_str());
    sb.Append(prefix).Append("{\n");

    if (interface_->IsSerializable()) {
        sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", objName.c_str());
        sb.Append(prefix + TAB + TAB).Append("return NULL;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).AppendFormat(
            "struct %sStub *stub = CONTAINER_OF(%s, struct %sStub, interface);\n", baseName_.c_str(),
            objName.c_str(), baseName_.c_str());
        sb.Append(prefix + TAB).Append("return stub->remote;\n");
    } else {
        sb.Append(prefix + TAB).Append("return NULL;\n");
    }

    sb.Append(prefix).Append("}\n");
}

void CServiceStubCodeEmitter::EmitKernelStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix)
{
    std::string implName = "serviceImpl";
    std::string codeName = "code";
    std::string funcName = StringHelper::Format("%sOnRemoteRequest", baseName_.c_str());
    sb.Append(prefix).AppendFormat("static int32_t %s(struct %s *%s, ", funcName.c_str(), interfaceName_.c_str(),
        implName.c_str());
    sb.AppendFormat("int %s, struct HdfSBuf *data, struct HdfSBuf *reply)\n", codeName.c_str());
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("switch (%s) {\n", codeName.c_str());
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(method).c_str());
        sb.Append(prefix + TAB + TAB + TAB)
            .AppendFormat("return SerStub%s(%s, data, reply);\n", method->GetName().c_str(), implName.c_str());
    }

    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(getVerMethod).c_str());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("return SerStub%s(%s, data, reply);\n", getVerMethod->GetName().c_str(), implName.c_str());

    sb.Append(prefix + TAB + TAB).Append("default: {\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: not support cmd %%{public}d\", __func__, %s);\n", codeName.c_str());
    sb.Append(prefix + TAB + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitKernelStubConstruct(StringBuilder &sb) const
{
    std::string stubTypeName = StringHelper::Format("%sStub", baseName_.c_str());
    std::string objName = "stub";
    std::string funcName = StringHelper::Format("%sOnRemoteRequest", baseName_.c_str());

    sb.AppendFormat(
        "bool %sConstruct(struct %s *%s)\n", stubTypeName.c_str(), stubTypeName.c_str(), objName.c_str());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.c_str());
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: stub is null!\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return false;\n");
    sb.Append(TAB).Append("}\n\n");
    sb.Append(TAB).AppendFormat("%s->OnRemoteRequest = %s;\n", objName.c_str(), funcName.c_str());
    sb.Append(TAB).Append("return true;\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix)
{
    std::string remoteName = "remote";
    std::string codeName = "code";
    std::string funcName = StringHelper::Format("%sOnRemoteRequest", baseName_.c_str());
    sb.Append(prefix).AppendFormat("static int32_t %s(struct HdfRemoteService *%s, ", funcName.c_str(),
        remoteName.c_str());
    sb.AppendFormat("int %s, struct HdfSBuf *data, struct HdfSBuf *reply)\n", codeName.c_str());
    sb.Append(prefix).Append("{\n");

    sb.Append(prefix + TAB).AppendFormat("struct %s *stub = (struct %s*)%s;\n", stubName_.c_str(),
        stubName_.c_str(), remoteName.c_str());
    sb.Append(prefix + TAB).Append("if (stub == NULL || stub->remote == NULL || stub->interface == NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid stub object\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_OBJECT;\n");
    sb.Append(prefix + TAB).Append("}\n");

    sb.Append(prefix + TAB).AppendFormat("if (!HdfRemoteServiceCheckInterfaceToken(stub->%s, data)) {\n",
        remoteName.c_str());
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: interface token check failed\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append(prefix + TAB).AppendFormat("switch (%s) {\n", codeName.c_str());
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(method).c_str());
        sb.Append(prefix + TAB + TAB + TAB)
            .AppendFormat("return SerStub%s(stub->interface, data, reply);\n", method->GetName().c_str());
    }

    AutoPtr<ASTMethod> getVerMethod = interface_->GetVersionMethod();
    sb.Append(prefix + TAB + TAB).AppendFormat("case %s:\n", EmitMethodCmdID(getVerMethod).c_str());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("return SerStub%s(stub->interface, data, reply);\n", getVerMethod->GetName().c_str());

    sb.Append(prefix + TAB + TAB).Append("default: {\n");
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: not support cmd %%{public}d\", __func__, %s);\n", codeName.c_str());
    sb.Append(prefix + TAB + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubRemoteDispatcher(StringBuilder &sb) const
{
    std::string dispatcherName = StringHelper::Format("g_%sDispatcher", StringHelper::StrToLower(baseName_).c_str());
    sb.AppendFormat("static struct HdfRemoteDispatcher %s = {\n", dispatcherName.c_str());
    sb.Append(TAB).AppendFormat(".Dispatch = %sOnRemoteRequest,\n", baseName_.c_str());
    sb.Append(TAB).Append(".DispatchAsync = NULL,\n");
    sb.Append("};\n");
}

void CServiceStubCodeEmitter::EmitStubNewInstance(StringBuilder &sb) const
{
    std::string dispatcherName = StringHelper::Format("g_%sDispatcher", StringHelper::StrToLower(baseName_).c_str());
    sb.AppendFormat("static struct HdfRemoteService **%sNewInstance(void *impl)\n", stubName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (impl == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: impl is null\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n\n");

    sb.Append(TAB).AppendFormat("struct %s *serviceImpl = (struct %s *)impl;\n", interfaceName_.c_str(),
        interfaceName_.c_str());
    sb.Append(TAB).AppendFormat("struct %s *stub = OsalMemCalloc(sizeof(struct %s));\n", stubName_.c_str(),
        stubName_.c_str());
    sb.Append(TAB).Append("if (stub == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc stub object\", __func__);\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n");

    if (interface_->IsSerializable()) {
        sb.Append(TAB).Append("stub->remote = HdfRemoteServiceObtain((struct HdfObject *)stub, &stub->dispatcher);\n");
    } else {
        sb.Append(TAB).AppendFormat("stub->remote = HdfRemoteServiceObtain((struct HdfObject *)stub, &%s);\n",
            dispatcherName.c_str());
    }

    sb.Append(TAB).Append("if (stub->remote == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("OsalMemFree(stub);\n");
    sb.Append(TAB).Append(TAB).Append("return NULL;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat("(void)HdfRemoteServiceSetInterfaceDesc(stub->remote, %s);\n",
        EmitDescMacroName().c_str());
    sb.Append(TAB).AppendFormat("stub->dispatcher.Dispatch = %sOnRemoteRequest;\n", baseName_.c_str());
    sb.Append(TAB).Append("stub->interface = serviceImpl;\n");
    sb.Append(TAB).AppendFormat("stub->interface->AsObject = %sStubAsObject;\n", baseName_.c_str());
    sb.Append(TAB).Append("return &stub->remote;\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubReleaseMethod(StringBuilder &sb) const
{
    sb.AppendFormat("static void %sRelease(struct HdfRemoteService **remote)\n", stubName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).Append("if (remote == NULL) {\n");
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append(TAB).AppendFormat("struct %s *stub = CONTAINER_OF(remote, struct %s, remote);\n",
        stubName_.c_str(), stubName_.c_str());
    sb.Append(TAB).Append("HdfRemoteServiceRecycle(stub->remote);\n");
    sb.Append(TAB).Append("OsalMemFree(stub);\n");
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitStubConstructor(StringBuilder &sb) const
{
    std::string constructorName = StringHelper::Format("g_%sConstructor", StringHelper::StrToLower(baseName_).c_str());
    sb.AppendFormat("__attribute__((unused)) static struct StubConstructor %s = {\n", constructorName.c_str());
    sb.Append(TAB).AppendFormat(".constructor = %sNewInstance,\n", stubName_.c_str());
    sb.Append(TAB).AppendFormat(".destructor = %sRelease,\n", stubName_.c_str());
    sb.Append("};\n");
}

void CServiceStubCodeEmitter::EmitStubRegAndUnreg(StringBuilder &sb) const
{
    std::string constructorName = StringHelper::Format("g_%sConstructor", StringHelper::StrToLower(baseName_).c_str());
    sb.AppendFormat("__attribute__((constructor)) static void %sRegister(void)\n", stubName_.c_str());
    sb.Append("{\n");
    sb.Append(TAB).AppendFormat(
        "HDF_LOGI(\"%%{public}s: register stub constructor of '%%{public}s'\", __func__, %s);\n",
        EmitDescMacroName().c_str());
    sb.Append(TAB).AppendFormat("StubConstructorRegister(%s, &%s);\n", EmitDescMacroName().c_str(),
        constructorName.c_str());
    sb.Append("}\n");
}

void CServiceStubCodeEmitter::EmitUtilMethods(StringBuilder &sb, bool isDecl)
{
    UtilMethodMap methods;
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        for (size_t paramIndex = 0; paramIndex < method->GetParameterNumber(); paramIndex++) {
            AutoPtr<ASTParameter> param = method->GetParameter(paramIndex);
            AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            if (param->GetAttribute() == ASTParamAttr::PARAM_IN) {
                typeEmitter->EmitCStubReadMethods(methods, "", "", isDecl);
            } else {
                typeEmitter->EmitCWriteMethods(methods, "", "", isDecl);
            }
        }
    }
    EmitUtilMethodMap(sb, methods);
}
} // namespace Idl
} // namespace OHOS
