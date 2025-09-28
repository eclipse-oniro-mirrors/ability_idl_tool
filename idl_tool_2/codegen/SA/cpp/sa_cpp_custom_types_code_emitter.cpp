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

#include "sa_cpp_custom_types_code_emitter.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {

void SaCppCustomTypesCodeEmitter::EmitCode()
{
    SaTypeEmitter::circleCount_ = 0;
    EmitCustomTypesHeaderFile();
    EmitCustomTypesSourceFile();
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypesHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(baseName_).c_str()));
    File file(filePath, File::WRITE);
    std::string marcoName = StringHelper::Format("%s.%s", ast_->GetPackageName().c_str(), baseName_.c_str());
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, marcoName);
    sb.Append("\n");
    EmitHeaderFileInclusions(sb);
    sb.Append("\n");
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitCustomTypeDecls(sb);
    sb.Append("\n");
    EmitCustomTypeFuncDecl(sb);
    sb.Append("\n");
    EmitEndNamespace(sb);
    sb.Append("\n");
    EmitTailMacro(sb, marcoName);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppCustomTypesCodeEmitter::EmitHeaderFileInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "cstdbool");
    headerFiles.emplace(HeaderFileType::CPP_STD_HEADER_FILE, "cstdint");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "message_parcel");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "iremote_proxy");
    GetStdlibInclusions(headerFiles);
    GetImportInclusions(headerFiles);

    bool needLogh = false;
    bool needSecurec = false;
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = static_cast<ASTStructType *>(type.Get());
            if (!type->IsPod() && logOn_) {
                needLogh = true;
            }
            if (EmitCustomTypeNeedSecurec(structType)) {
                needSecurec = true;
            }
            if (needLogh && needSecurec) {
                break;
            }
        }
    }

    if (needSecurec) {
        headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
    }
    if (needLogh) {
        headerFiles.emplace(HeaderFileType::OWN_MODULE_HEADER_FILE, "hilog/log");
    }
    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
    if (needLogh && !domainId_.empty() && !logTag_.empty()) {
        if (ast_ != nullptr && ast_->GetOptionParcelHooksOn()) {
            sb.AppendFormat(
                "static constexpr OHOS::HiviewDFX::HiLogLabel %s_LABEL = {LOG_CORE, %s, \"%s\"};\n",
                StringHelper::StrToUpper(baseName_).c_str(), domainId_.c_str(), logTag_.c_str());
        }
    }
}

bool SaCppCustomTypesCodeEmitter::EmitCustomTypeNeedSecurec(const AutoPtr<ASTStructType> &type) const
{
    if (type->IsPod()) {
        return true;
    } else {
        for (size_t j = 0; j < type->GetMemberNumber(); j++) {
            AutoPtr<ASTType> memberType = type->GetMemberType(j);
            if (memberType->GetTypeKind() == TypeKind::TYPE_UNION) {
                return true;
            }
        }
    }
    return false;
}

void SaCppCustomTypesCodeEmitter::EmitDefineConstant(StringBuilder &sb) const
{
    sb.Append(
        "[[maybe_unused]] static constexpr int VECTOR_MAX_SIZE = 102400;\n"
        "[[maybe_unused]] static constexpr int LIST_MAX_SIZE = 102400;\n"
        "[[maybe_unused]] static constexpr int SET_MAX_SIZE = 102400;\n"
        "[[maybe_unused]] static constexpr int MAP_MAX_SIZE = 102400;\n");
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeDecls(StringBuilder &sb) const
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(ast_->GetTypeDefintion(i));
        if (typeEmitter == nullptr) {
            return;
        }
        sb.Append(typeEmitter->EmitCppTypeDecl()).Append("\n");
        if (i + 1 < ast_->GetTypeDefinitionNumber()) {
            sb.Append("\n");
        }
    }
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeFuncDecl(StringBuilder &sb) const
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            EmitCustomTypeMarshallFuncDecl(sb, type);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeMarshallFuncDecl(StringBuilder &sb, const AutoPtr<ASTType> &type) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr) {
        return;
    }
    std::string objName("dataBlock");
    sb.AppendFormat("ErrCode %sBlockMarshalling(OHOS::MessageParcel &data, const %s& %s);\n\n", type->GetName().c_str(),
        typeEmitter->EmitCppType().c_str(), objName.c_str());
    sb.AppendFormat("ErrCode %sBlockUnmarshalling(OHOS::MessageParcel &data, %s& %s);\n", type->GetName().c_str(),
        typeEmitter->EmitCppType().c_str(), objName.c_str());
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypesSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.cpp", directory_.c_str(), FileName(baseName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitSourceFileInclusions(sb);
    sb.Append("\n");
    if (hitraceOn_) {
        sb.Append("#include \"hitrace_meter.h\"\n");
    }
    if (logOn_) {
        sb.Append("#include \"hilog/log.h\"\n").Append(macroDefine_.c_str());
    }
    EmitBeginNamespace(sb);
    sb.Append("\n");
    EmitDefineConstant(sb);
    sb.Append("\n");
    EmitCustomTypeDataProcess(sb);
    EmitEndNamespace(sb);

    std::string data = sb.ToString();

    std::string customLabel = "(" + StringHelper::StrToUpper(baseName_) + "_LABEL,";
    std::string oldLabel = "(LABEL,";

    size_t pos = 0;
    while ((pos = data.find(oldLabel, pos)) != std::string::npos) {
        data.replace(pos, oldLabel.size(), customLabel);
        pos += customLabel.length();
    }

    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void SaCppCustomTypesCodeEmitter::EmitSourceFileInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;

    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(baseName_));

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeDataProcess(StringBuilder &sb) const
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = static_cast<ASTStructType *>(type.Get());
            if (structType == nullptr) {
                continue;
            }
            EmitCustomTypeMarshallingImpl(sb, structType);
            EmitCustomTypeUnmarshallingImpl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeMarshallingImpl(
    StringBuilder &sb, const AutoPtr<ASTStructType> &type) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(type.Get());
    if (typeEmitter == nullptr) {
        return;
    }
    std::string typeName = typeEmitter->EmitCppType();
    std::string objName("dataBlock");

    sb.AppendFormat("ErrCode %sBlockMarshalling(OHOS::MessageParcel& data, const %s& %s)\n", type->GetName().c_str(),
        typeName.c_str(), objName.c_str());
    sb.Append("{\n");

    if (type->IsPod()) {
        sb.Append(TAB).AppendFormat("if (!data.WriteUnpadBuffer(&%s, sizeof(%s))) {\n",
            objName.c_str(), typeName.c_str());
        sb.Append(TAB).Append(TAB).Append("return ERR_INVALID_DATA;\n");
        sb.Append(TAB).Append("}\n");
    } else {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            std::string memberName = type->GetMemberName(i);
            std::string name = StringHelper::Format("%s.%s", objName.c_str(), memberName.c_str());
            typeEmitter = GetTypeEmitter(type->GetMemberType(i));
            if (typeEmitter == nullptr) {
                continue;
            }
            typeEmitter->EmitCppWriteVar("data.", name, sb, TAB);
            if (i + 1 < type->GetMemberNumber()) {
                sb.Append("\n");
            }
        }
    }

    sb.Append(TAB).Append("return ERR_NONE;\n");
    sb.Append("}\n\n");
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingImpl(
    StringBuilder &sb, const AutoPtr<ASTStructType> &type) const
{
    AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(type.Get());
    if (typeEmitter == nullptr) {
        return;
    }
    std::string typeName = typeEmitter->EmitCppType();
    std::string objName("dataBlock");

    sb.AppendFormat("ErrCode %sBlockUnmarshalling(OHOS::MessageParcel& data, %s& %s)\n", type->GetName().c_str(),
        typeName.c_str(), objName.c_str());
    sb.Append("{\n");

    if (type->IsPod()) {
        std::string objPtrName = StringHelper::Format("%sPtr", objName.c_str());
        sb.Append(TAB).AppendFormat("const %s *%s = reinterpret_cast<const %s *>(data.ReadUnpadBuffer(sizeof(%s)));\n",
            typeName.c_str(), objPtrName.c_str(), typeName.c_str(), typeName.c_str());
        sb.Append(TAB).AppendFormat("if (%s == nullptr) {\n", objPtrName.c_str());
        sb.Append(TAB).Append(TAB).Append("return ERR_INVALID_DATA;\n");
        sb.Append(TAB).Append("}\n\n");
        sb.Append(TAB).AppendFormat("if (memcpy_s(&%s, sizeof(%s), %s, sizeof(%s)) != EOK) {\n", objName.c_str(),
            typeName.c_str(), objPtrName.c_str(), typeName.c_str());
        sb.Append(TAB).Append(TAB).Append("return ERR_INVALID_DATA;\n");
        sb.Append(TAB).Append("}\n");
    } else {
        EmitCustomTypeUnmarshallingImplNoPod(sb, type);
    }
    sb.Append(TAB).AppendFormat("return ERR_NONE;\n");
    sb.Append("}\n");
}

void SaCppCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingImplNoPod(
    StringBuilder &sb, const AutoPtr<ASTStructType> &type) const
{
    std::string objName("dataBlock");
    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        std::string memberName = type->GetMemberName(i);
        std::string name = StringHelper::Format("%s.%s", objName.c_str(), memberName.c_str());
        if (i > 0) {
            sb.Append("\n");
        }

        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(memberType);
        if (typeEmitter == nullptr) {
            return;
        }
        if (memberType->GetTypeKind() == TypeKind::TYPE_UNION) {
            std::string cpName = StringHelper::Format("%sCp", memberName.c_str());
            typeEmitter->EmitCppReadVar("data.", cpName, sb, TAB);
            sb.Append(TAB).AppendFormat("if (memcpy_s(&%s, sizeof(%s), %s, sizeof(%s)) != EOK) {\n",
                name.c_str(), typeEmitter->EmitCppType().c_str(), cpName.c_str(),
                typeEmitter->EmitCppType().c_str());
            if (logOn_) {
                sb.Append(TAB).Append(TAB).Append(macroError_.c_str()).
                    AppendFormat(", \"memcpy [%s] failed!\");\n", name.c_str());
            }
            sb.Append(TAB).Append(TAB).Append("return false;\n");
            sb.Append(TAB).Append("}\n");
        } else if (memberType->GetTypeKind() == TypeKind::TYPE_STRING) {
            std::string cpName = StringHelper::Format("%sCp", memberName.c_str());
            typeEmitter->EmitCppReadVar("data.", cpName, sb, TAB);
            sb.Append(TAB).AppendFormat("%s = %s;\n", name.c_str(), cpName.c_str());
        } else {
            typeEmitter->EmitCppReadVar("data.", name, sb, TAB, false);
        }
    }
}

void SaCppCustomTypesCodeEmitter::EmitBeginNamespace(StringBuilder &sb)
{
    std::string fullnamespace = ast_->GetPackageName();
    std::vector<std::string> namespaceVec = StringHelper::Split(fullnamespace, ".");

    for (const auto &nspace : namespaceVec) {
        sb.AppendFormat("namespace %s {\n", nspace.c_str());
    }
}

void SaCppCustomTypesCodeEmitter::EmitEndNamespace(StringBuilder &sb)
{
    std::string fullnamespace = ast_->GetPackageName();
    std::vector<std::string> namespaceVec = StringHelper::Split(fullnamespace, ".");

    for (std::vector<std::string>::const_reverse_iterator nspaceIter = namespaceVec.rbegin();
        nspaceIter != namespaceVec.rend(); ++nspaceIter) {
        sb.AppendFormat("} // namespace %s\n", nspaceIter->c_str());
    }
}

} // namespace Idl
} // namespace OHOS
