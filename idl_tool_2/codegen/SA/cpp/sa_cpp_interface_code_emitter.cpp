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

#include "sa_cpp_interface_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
void SaCppInterfaceCodeEmitter::EmitCode()
{
    EmitInterfaceHeaderFile();
}

std::string SaCppInterfaceCodeEmitter::GetFilePath(const std::string &fpnp) const
{
    size_t pos = fpnp.find("..");
    if (pos == std::string::npos) {
        return "";
    }

    return fpnp.substr(0, pos + 1);
}

std::string SaCppInterfaceCodeEmitter::GetFilePathNoPoint(const std::string &fpnp) const
{
    size_t pos = fpnp.find("..");
    if (pos == std::string::npos) {
        return "";
    }

    return fpnp.substr(0, pos);
}

std::string SaCppInterfaceCodeEmitter::CppFullName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    return StringHelper::Replace(name, ".", "::");
}

void SaCppInterfaceCodeEmitter::EmitInterfaceHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(interfaceName_).c_str()));
    File file(filePath, File::WRITE);

    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, interfaceFullName_);
    sb.Append("\n");
    EmitInterfaceInclusions(sb);
    sb.Append("\n");
    if (EmitInterfaceUsings(sb)) {
        sb.Append("\n");
    }
    EmitInterfaceDefinition(sb);
    EmitTailMacro(sb, interfaceFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
    return;
}

void SaCppInterfaceCodeEmitter::EmitInterfaceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    GetStdlibInclusions(headerFiles);
    EmitInterfaceDBinderInclusions(headerFiles);
    EmitInterfaceSelfDefinedTypeInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void SaCppInterfaceCodeEmitter::EmitInterfaceDBinderInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "iremote_broker");
    if (logOn_) {
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, "hilog/log");
    }
    return;
}

void SaCppInterfaceCodeEmitter::EmitInterfaceSelfDefinedTypeInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    std::string filePath;
    std::string fileName;
    int sequenceableNumber = static_cast<int>(ast_->GetSequenceableDefNumber());
    for (int i = 0; i < sequenceableNumber; i++) {
        AutoPtr<ASTSequenceableType> seqType = ast_->GetSequenceableDef(i);
        filePath = GetFilePathNoPoint(seqType->GetNamespace()->ToString());
        fileName = filePath.empty() ? FileName(seqType->GetName()) : FileName(filePath);
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName);
    }

    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == false) {
            continue;
        }
        filePath = GetFilePath(interface->GetNamespace()->ToString());
        fileName = FileName(filePath + interface->GetName());
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, fileName);
    }
    return;
}

bool SaCppInterfaceCodeEmitter::EmitInterfaceUsings(StringBuilder &sb) const
{
    std::string np;
    std::string fullName;
    bool ret = false;

    int sequenceableNumber = static_cast<int>(ast_->GetSequenceableDefNumber());
    for (int i = 0; i < sequenceableNumber; i++) {
        AutoPtr<ASTSequenceableType> seqType = ast_->GetSequenceableDef(i);
        np = GetNamespace(seqType->GetNamespace()->ToString());
        if (np.empty()) {
            continue;
        }
        fullName = CppFullName(np + seqType->GetName());
        sb.Append("using ").AppendFormat("%s;\n", fullName.c_str());
        ret = true;
    }

    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == false) {
            continue;
        }
        np = GetNamespace(interface->GetNamespace()->ToString());
        if (np.empty()) {
            continue;
        }
        fullName = CppFullName(np + interface->GetName());
        sb.Append("using ").AppendFormat("%s;\n", fullName.c_str());
        ret = true;
    }

    return ret;
}

void SaCppInterfaceCodeEmitter::EmitInterfaceDefinition(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    sb.AppendFormat("class %s : public IRemoteBroker {\n", interface_->GetName().c_str());
    sb.Append("public:\n");
    EmitInterfaceBody(sb, TAB);
    EmitInterfaceMemberVariables(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void SaCppInterfaceCodeEmitter::EmitInterfaceBody(StringBuilder &sb, const std::string &prefix) const
{
    std::string nameWithoutPath = GetNamespace(interfaceFullName_);
    sb.Append(prefix).AppendFormat("DECLARE_INTERFACE_DESCRIPTOR(u\"%s\");\n", nameWithoutPath.c_str());
    sb.Append("\n");

    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        EmitInterfaceMethod(method, sb, prefix);
        if (i != methodNumber - 1) {
            sb.Append("\n");
        }
    }
}

void SaCppInterfaceCodeEmitter::EmitInterfaceMethod(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("virtual ErrCode %s(", method->GetName().c_str());
    EmitInterfaceMethodParams(method, sb, prefix + TAB);
    sb.Append(") = 0;\n");
}

void SaCppInterfaceCodeEmitter::EmitInterfaceMemberVariables(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append("protected:\n");
    if (!domainId_.empty() && !logTag_.empty()) {
        sb.Append(prefix).AppendFormat(
            "static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, %s, \"%s\"};\n",
            domainId_.c_str(), logTag_.c_str());
    }
    sb.Append(prefix).Append("const int VECTOR_MAX_SIZE = 102400;\n");
    sb.Append(prefix).Append("const int LIST_MAX_SIZE = 102400;\n");
    sb.Append(prefix).Append("const int MAP_MAX_SIZE = 102400;\n");
}

} // namespace Idl
} // namespace OHOS