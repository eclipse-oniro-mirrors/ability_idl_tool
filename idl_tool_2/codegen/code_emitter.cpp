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

#include "codegen/code_emitter.h"

#include <cctype>

#include "util/file.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
void CodeEmitter::EmitLicense(StringBuilder &sb)
{
    if (ast_->GetLicense().empty()) {
        return;
    }
    sb.Append(ast_->GetLicense()).Append("\n\n");
}

bool CodeEmitter::NeedFlag(const AutoPtr<ASTMethod> &method) const
{
    for (size_t i = 0; i < method->GetParameterNumber(); i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param == nullptr) {
            return false;
        }
        AutoPtr<ASTType> type = param->GetType();
        if ((param->GetAttribute() & ASTParamAttr::PARAM_OUT) &&
            (type->IsStringType() || type->IsArrayType() || type->IsListType())) {
            return true;
        }
    }
    return false;
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * outDir: ./out
 * package: ohos.hdi.foo.v1_0
 * subPackage: foo.v1_0
 * subPath: foo/v1_0
 * GenPath: ./out/foo/v1_0/
 */
std::string CodeEmitter::GetFileParentPath(const std::string &outDir) const
{
    if (outDir.size() > SIZE_MAX) {
        return std::string("");
    }
    std::string outPath = StringHelper::EndWith(outDir, SEPARATOR) ? outDir.substr(0, outDir.size() - 1) : outDir;
    std::string subPackage = Options::GetInstance().GetSubPackage(ast_->GetPackageName());
    std::string subPath = StringHelper::Replace(subPackage, '.', SEPARATOR);
    if (subPath.empty()) {
        return File::AdapterPath(StringHelper::Format("%s/", outPath.c_str(), subPath.c_str()));
    } else {
        return File::AdapterPath(StringHelper::Format("%s/%s/", outPath.c_str(), subPath.c_str()));
    }
}

std::string CodeEmitter::PackageToFilePath(const std::string &packageName) const
{
    std::vector<std::string> packageVec = StringHelper::Split(Options::GetInstance().GetSubPackage(packageName), ".");
    StringBuilder filePath;
    for (auto iter = packageVec.begin(); iter != packageVec.end(); ++iter) {
        filePath.Append(FileName(*iter));
        if (iter != packageVec.end() - 1) {
            filePath.Append(SEPARATOR);
        }
    }

    return filePath.ToString();
}

std::string CodeEmitter::InterfaceToFilePath(const std::string &interfaceName) const
{
    std::string fullName = interfaceName;
    size_t index;
    if (StringHelper::EndWith(fullName, "]")) {
        index = fullName.find('[');
        fullName = fullName.substr(0, index);
    }

    index = fullName.rfind('.');
    std::string prefix = fullName.substr(0, index + 1);
    std::string suffix = fullName.substr(index + 1);
    std::string fileName = prefix + (StringHelper::StartWith(suffix, "I") ? suffix.substr(1) : suffix) + "Proxy";
    return PackageToFilePath(fileName);
}

std::string CodeEmitter::EmitMethodCmdID(const AutoPtr<ASTMethod> &method)
{
    return StringHelper::Format("CMD_%s_%s%s",
        ConstantName(baseName_).c_str(), ConstantName(method->GetName()).c_str(),
        method->GetMethodIdentifier().c_str());
}

void CodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    sb.Append(prefix).AppendFormat("enum {\n");
    sb.Append(prefix + TAB).Append(EmitMethodCmdID(interface_->GetVersionMethod())).Append(" = 0,\n");
    int i = 0;
    for (const auto &method : interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel())) {
        sb.Append(prefix + TAB).Append(EmitMethodCmdID(method)).AppendFormat(" = %d", i + 1).Append(",\n");
        i++;
    }
    sb.Append(prefix).Append("};\n");
}

std::string CodeEmitter::EmitVersionHeaderName(const std::string &name) const
{
    InterfaceType interfaceType = Options::GetInstance().GetInterfaceType();
    if (interfaceType == InterfaceType::SA) {
        return StringHelper::Format("%s", FileName(name).c_str());
    }
    return StringHelper::Format("v%u_%u/%s", ast_->GetMajorVer(), ast_->GetMinorVer(), FileName(name).c_str());
}

void CodeEmitter::EmitLogTagMacro(StringBuilder &sb, const std::string &name) const
{
    sb.AppendFormat("#define HDF_LOG_TAG    %s\n", name.c_str());
}

std::string CodeEmitter::ConstantName(const std::string &name) const
{
    StringBuilder sb;

    if (name.empty()) {
        return name;
    }

    for (size_t i = 0; i < name.size(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            if (i > 1) {
                sb.Append('_');
            }
            sb.Append(c);
        } else {
            sb.Append(static_cast<char>(toupper(c)));
        }
    }

    return sb.ToString();
}

std::string CodeEmitter::PascalName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    StringBuilder sb;
    for (size_t i = 0; i < name.size(); i++) {
        char c = name[i];
        if (i == 0) {
            if (islower(c)) {
                c = static_cast<char>(toupper(c));
            }
            sb.Append(c);
        } else {
            if (c == '_') {
                continue;
            }

            if (islower(c) && name[i - 1] == '_') {
                c = static_cast<char>(toupper(c));
            }
            sb.Append(c);
        }
    }

    return sb.ToString();
}

std::string CodeEmitter::FileName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    StringBuilder sb;
    for (size_t i = 0; i < name.size(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            // 2->Index of the last char array.
            if ((i > 1) && (name[i - 1] != '.') && (name[i - 2] != '.')) {
                sb.Append('_');
            }
            sb.Append(tolower(c));
        } else {
            sb.Append(c);
        }
    }

    return StringHelper::Replace(sb.ToString(), '.', '/');
}

std::string CodeEmitter::GetNamespace(const std::string &fpnp) const
{
    size_t pos = fpnp.find("..");
    if (pos == std::string::npos) {
        return fpnp;
    }

    return fpnp.substr(pos + strlen(".."));
}

void CodeEmitter::EmitHeadMacro(StringBuilder &sb, const std::string &fullName) const
{
    std::string macroName = MacroName(fullName);
    sb.Append("#ifndef ").Append(macroName).Append('\n');
    sb.Append("#define ").Append(macroName).Append('\n');
}

void CodeEmitter::EmitTailMacro(StringBuilder &sb, const std::string &fullName) const
{
    std::string macroName = MacroName(fullName);
    sb.Append("#endif // ").Append(macroName.c_str()).Append("\n\n");
}

void CodeEmitter::EmitHeadExternC(StringBuilder &sb) const
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("extern \"C\" {\n");
    sb.Append("#endif /* __cplusplus */\n");
}

void CodeEmitter::EmitTailExternC(StringBuilder &sb) const
{
    sb.Append("#ifdef __cplusplus\n");
    sb.Append("}\n");
    sb.Append("#endif /* __cplusplus */\n");
}

std::string CodeEmitter::MacroName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    return StringHelper::StrToUpper(StringHelper::Replace(name, '.', '_')) + "_H";
}
} // namespace Idl
} // namespace OHOS
