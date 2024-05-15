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

#include "cpp_service_impl_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool CppServiceImplCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    if (ast_->GetASTFileType() == ASTFileType::AST_IFACE || ast_->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
        directory_ = GetFileParentPath(targetDirectory);
    } else {
        return false;
    }

    if (!File::CreateParentDir(directory_)) {
        Logger::E("CppServiceImplCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void CppServiceImplCodeEmitter::EmitCode()
{
    switch (mode_) {
        case GenMode::PASSTHROUGH:
        case GenMode::IPC: {
            EmitImplHeaderFile();
            EmitImplSourceFile();
            break;
        }
        default:
            break;
    }
}

void CppServiceImplCodeEmitter::EmitImplHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(implName_).c_str()));
    File file(filePath, File::write_);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, implFullName_);
    sb.Append("\n");
    EmitServiceImplInclusions(sb);
    sb.Append("\n");
    EmitServiceImplDecl(sb);
    sb.Append("\n");
    EmitTailMacro(sb, implFullName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CppServiceImplCodeEmitter::EmitServiceImplInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(interfaceName_));

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CppServiceImplCodeEmitter::EmitServiceImplDecl(StringBuilder &sb)
{
    EmitBeginNamespace(sb);
    sb.AppendFormat("class %sService : public %s {\n", baseName_.c_str(),
        EmitDefinitionByInterface(interface_, interfaceName_).c_str());
    sb.Append("public:\n");
    EmitServiceImplBody(sb, TAB);
    sb.Append("};\n");
    EmitEndNamespace(sb);
}

void CppServiceImplCodeEmitter::EmitServiceImplBody(StringBuilder &sb, const std::string &prefix)
{
    (void)prefix;
    EmitServiceImplConstructor(sb, TAB);
    sb.Append("\n");
    EmitServiceImplMethodDecls(sb, TAB);
}

void CppServiceImplCodeEmitter::EmitServiceImplConstructor(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s() = default;\n", implName_.c_str());
    sb.Append(prefix).AppendFormat("virtual ~%s() = default;\n", implName_.c_str());
}

void CppServiceImplCodeEmitter::EmitServiceImplMethodDecls(StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTInterfaceType> interface = interface_;
    while (interface != nullptr) {
        for (const auto &method : interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
            EmitServiceImplMethodDecl(method, sb, prefix);
            sb.Append("\n");
        }
        interface = interface->GetExtendsInterface();
    }
}

void CppServiceImplCodeEmitter::EmitServiceImplMethodDecl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat("int32_t %s() override;\n", method->GetName().c_str());
    } else {
        StringBuilder paramStr;
        paramStr.Append(prefix).AppendFormat("int32_t %s(", method->GetName().c_str());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.Append(") override;");

        sb.Append(SpecificationParam(paramStr, prefix + TAB));
        sb.Append("\n");
    }
}

void CppServiceImplCodeEmitter::EmitImplSourceFile()
{
    std::string filePath = File::AdapterPath(
        StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(implName_).c_str()));
    File file(filePath, File::write_);
    StringBuilder sb;

    EmitLicense(sb);
    EmitImplSourceInclusions(sb);
    sb.Append("\n");
    EmitLogTagMacro(sb, FileName(implName_));
    sb.Append("\n");
    EmitBeginNamespace(sb);
    EmitServiceImplGetMethodImpl(sb, "");
    EmitServiceImplMethodImpls(sb, "");
    EmitEndNamespace(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CppServiceImplCodeEmitter::EmitImplSourceInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(implName_));
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base");

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CppServiceImplCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_base");
}

void CppServiceImplCodeEmitter::EmitServiceImplMethodImpls(StringBuilder &sb, const std::string &prefix) const
{
    AutoPtr<ASTInterfaceType> interface = interface_;
    while (interface != nullptr) {
        for (const auto &method : interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
            EmitServiceImplMethodImpl(method, sb, prefix);
            sb.Append("\n");
        }
        interface = interface->GetExtendsInterface();
    }
}

void CppServiceImplCodeEmitter::EmitServiceImplMethodImpl(
    const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const
{
    if (method->GetParameterNumber() == 0) {
        sb.Append(prefix).AppendFormat("int32_t %sService::%s()\n", baseName_.c_str(), method->GetName().c_str());
    } else {
        StringBuilder paramStr;
        paramStr.Append(prefix).AppendFormat("int32_t %sService::%s(", baseName_.c_str(), method->GetName().c_str());
        for (size_t i = 0; i < method->GetParameterNumber(); i++) {
            AutoPtr<ASTParameter> param = method->GetParameter(i);
            EmitInterfaceMethodParameter(param, paramStr, "");
            if (i + 1 < method->GetParameterNumber()) {
                paramStr.Append(", ");
            }
        }

        paramStr.AppendFormat(")");

        sb.Append(SpecificationParam(paramStr, prefix + TAB));
        sb.Append("\n");
    }

    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).Append("return HDF_SUCCESS;\n");
    sb.Append(prefix).Append("}\n");
}

void CppServiceImplCodeEmitter::EmitServiceImplGetMethodImpl(StringBuilder &sb, const std::string &prefix) const
{
    if (!interface_->IsSerializable()) {
        sb.Append(prefix).AppendFormat(
            "extern \"C\" %s *%sImplGetInstance(void)\n", interfaceName_.c_str(), baseName_.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).AppendFormat("return new (std::nothrow) %s();\n", implName_.c_str());
        sb.Append(prefix).Append("}\n\n");
    }
}
} // namespace Idl
} // namespace OHOS