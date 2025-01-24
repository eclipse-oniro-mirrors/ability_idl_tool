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

#include "c_custom_types_code_emitter.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
bool CCustomTypesCodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    if (ast_->GetASTFileType() != ASTFileType::AST_TYPES) {
        return false;
    }

    directory_ = GetFileParentPath(targetDirectory);
    if (!File::CreateParentDir(directory_)) {
        Logger::E("CCustomTypesCodeEmitter", "Create '%s' failed!", directory_.c_str());
        return false;
    }

    return true;
}

void CCustomTypesCodeEmitter::EmitCode()
{
    switch (mode_) {
        case GenMode::LOW:
        case GenMode::PASSTHROUGH: {
            EmitPassthroughCustomTypesHeaderFile();
            break;
        }
        case GenMode::IPC:
        case GenMode::KERNEL: {
            EmitCustomTypesHeaderFile();
            EmitCustomTypesSourceFile();
            break;
        }
        default:
            break;
    }
}

void CCustomTypesCodeEmitter::EmitPassthroughCustomTypesHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(baseName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, baseName_);
    sb.Append("\n");
    EmitPassthroughHeaderInclusions(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitCustomTypeDecls(sb);
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, baseName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CCustomTypesCodeEmitter::EmitPassthroughHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::C_STD_HEADER_FILE, "stdint");
    headerFiles.emplace(HeaderFileType::C_STD_HEADER_FILE, "stdbool");
    GetStdlibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypesHeaderFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.h", directory_.c_str(), FileName(baseName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitHeadMacro(sb, baseName_);
    sb.Append("\n");
    EmitHeaderInclusions(sb);
    sb.Append("\n");
    EmitInterfaceBuffSizeMacro(sb);
    sb.Append("\n");
    EmitHeadExternC(sb);
    sb.Append("\n");
    EmitForwardDeclaration(sb);
    sb.Append("\n");
    EmitCustomTypeDecls(sb);
    sb.Append("\n");
    EmitCustomTypeFuncDecl(sb);
    sb.Append("\n");
    EmitTailExternC(sb);
    sb.Append("\n");
    EmitTailMacro(sb, baseName_);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CCustomTypesCodeEmitter::EmitHeaderInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::C_STD_HEADER_FILE, "stdint");
    headerFiles.emplace(HeaderFileType::C_STD_HEADER_FILE, "stdbool");
    GetStdlibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CCustomTypesCodeEmitter::EmitForwardDeclaration(StringBuilder &sb) const
{
    sb.Append("struct HdfSBuf;\n");
}

void CCustomTypesCodeEmitter::EmitCustomTypeDecls(StringBuilder &sb) const
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(ast_->GetTypeDefintion(i));
        if (typeEmitter == nullptr) {
            continue;
        }
        sb.Append(typeEmitter->EmitCTypeDecl()).Append("\n");
        if (i + 1 < ast_->GetTypeDefinitionNumber()) {
            sb.Append("\n");
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeFuncDecl(StringBuilder &sb) const
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

void CCustomTypesCodeEmitter::EmitCustomTypeMarshallFuncDecl(StringBuilder &sb, const AutoPtr<ASTType> &type) const
{
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr) {
        return;
    }
    std::string objName("dataBlock");
    sb.AppendFormat("bool %sBlockMarshalling(struct HdfSBuf *data, const %s *%s);\n\n", type->GetName().c_str(),
        typeEmitter->EmitCType().c_str(), objName.c_str());
    sb.AppendFormat("bool %sBlockUnmarshalling(struct HdfSBuf *data, %s *%s);\n\n", type->GetName().c_str(),
        typeEmitter->EmitCType().c_str(), objName.c_str());
    sb.AppendFormat("void %sFree(%s *%s, bool freeSelf);\n", type->GetName().c_str(),
        typeEmitter->EmitCType().c_str(), objName.c_str());
}

void CCustomTypesCodeEmitter::EmitCustomTypesSourceFile()
{
    std::string filePath =
        File::AdapterPath(StringHelper::Format("%s/%s.c", directory_.c_str(), FileName(baseName_).c_str()));
    File file(filePath, File::WRITE);
    StringBuilder sb;

    EmitLicense(sb);
    EmitSoucreInclusions(sb);
    sb.Append("\n");
    EmitUtilMethods(sb, true);
    sb.Append("\n");
    EmitUtilMethods(sb, false);
    sb.Append("\n");
    EmitCustomTypeDataProcess(sb);

    std::string data = sb.ToString();
    file.WriteData(data.c_str(), data.size());
    file.Flush();
    file.Close();
}

void CCustomTypesCodeEmitter::EmitSoucreInclusions(StringBuilder &sb)
{
    HeaderFile::HeaderFileSet headerFiles;
    headerFiles.emplace(HeaderFileType::OWN_HEADER_FILE, EmitVersionHeaderName(baseName_));
    GetSourceOtherLibInclusions(headerFiles);

    for (const auto &file : headerFiles) {
        sb.AppendFormat("%s\n", file.ToString().c_str());
    }
}

void CCustomTypesCodeEmitter::GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const
{
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_core_log");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "hdf_sbuf");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "osal_mem");
    headerFiles.emplace(HeaderFileType::OTHER_MODULES_HEADER_FILE, "securec");
}

void CCustomTypesCodeEmitter::EmitCustomTypeDataProcess(StringBuilder &sb)
{
    for (size_t i = 0; i < ast_->GetTypeDefinitionNumber(); i++) {
        AutoPtr<ASTType> type = ast_->GetTypeDefintion(i);
        if (type->GetTypeKind() == TypeKind::TYPE_STRUCT) {
            AutoPtr<ASTStructType> structType = dynamic_cast<ASTStructType *>(type.Get());
            EmitCustomTypeMarshallingImpl(sb, structType);
            EmitCustomTypeUnmarshallingImpl(sb, structType);
            sb.Append("\n");
            EmitCustomTypeFreeImpl(sb, structType);
            if (i + 1 < ast_->GetTypeDefinitionNumber()) {
                sb.Append("\n");
            }
        }
    }
}

void CCustomTypesCodeEmitter::EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    std::string typeName = GetTypeEmitter(type.Get())->EmitCType();
    std::string objName("dataBlock");
    sb.AppendFormat("bool %sBlockMarshalling(struct HdfSBuf *data, const %s *%s)\n", type->GetName().c_str(),
        typeName.c_str(), objName.c_str());
    sb.Append("{\n");
    EmitMarshallingVarDecl(type, objName, sb, TAB);
    EmitParamCheck(objName, sb, TAB);
    sb.Append("\n");
    if (type->IsPod()) {
        if (Options::GetInstance().DoGenerateKernelCode()) {
            sb.Append(TAB).AppendFormat("if (!HdfSbufWriteBuffer(data, (const void *)%s, sizeof(%s))) {\n",
                objName.c_str(), typeName.c_str());
        } else {
            sb.Append(TAB).AppendFormat("if (!HdfSbufWriteUnpadBuffer(data, (const uint8_t *)%s, sizeof(%s))) {\n",
                objName.c_str(), typeName.c_str());
        }
        sb.Append(TAB).Append(TAB).Append("HDF_LOGE(\"%{public}s: failed to write buffer data\", __func__);\n");
        sb.Append(TAB).Append(TAB).Append("return false;\n");
        sb.Append(TAB).Append("}\n");
    } else {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            std::string memberName = type->GetMemberName(i);
            std::string name = StringHelper::Format("%s->%s", objName.c_str(), memberName.c_str());
            GetTypeEmitter(type->GetMemberType(i))->EmitCMarshalling(name, sb, TAB);
            sb.Append("\n");
        }
    }

    sb.Append(TAB).Append("return true;\n");
    sb.Append("}\n\n");
}

void CCustomTypesCodeEmitter::EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type)
{
    std::string typeName = GetTypeEmitter(type.Get())->EmitCType();
    std::string objName("dataBlock");
    freeObjStatements_.clear();
    sb.AppendFormat("bool %sBlockUnmarshalling(struct HdfSBuf *data, %s *%s)\n", type->GetName().c_str(),
        typeName.c_str(), objName.c_str());
    sb.Append("{\n");
    EmitUnmarshallingVarDecl(type, objName, sb, TAB);
    EmitParamCheck(objName, sb, TAB);
    sb.Append("\n");
    if (type->IsPod()) {
        EmitPodTypeUnmarshalling(typeName, objName, sb, TAB);
    } else {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            AutoPtr<ASTType> memberType = type->GetMemberType(i);
            EmitMemberUnmarshalling(memberType, objName, type->GetMemberName(i), sb, TAB);
        }
    }
    sb.Append(TAB).Append("return true;\n");
    sb.AppendFormat("%s:\n", ERRORS_LABEL  );
    EmitCustomTypeMemoryRecycle(type, objName, sb, TAB);
    sb.Append(TAB).Append("return false;\n");
    sb.Append("}\n");
}

void CCustomTypesCodeEmitter::EmitMarshallingVarDecl(const AutoPtr<ASTStructType> &type,
    const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    (void)name;
    if (!Options::GetInstance().DoGenerateKernelCode()) {
        return;
    }

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        if (EmitNeedLoopVar(type->GetMemberType(i), true, false)) {
            sb.Append(prefix).Append("uint32_t i = 0;\n");
            break;
        }
    }
}

void CCustomTypesCodeEmitter::EmitUnmarshallingVarDecl(
    const AutoPtr<ASTStructType> &type, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    if (!Options::GetInstance().DoGenerateKernelCode()) {
        return;
    }

    if (type->IsPod()) {
        AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type.Get());
        if (typeEmitter != nullptr) {
            sb.Append(prefix).AppendFormat("%s *%sPtr = NULL;\n",
                typeEmitter->EmitCType().c_str(), name.c_str());
        }
        sb.Append(prefix).AppendFormat("uint32_t %sLen = 0;\n\n", name.c_str());
        return;
    }

    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        if (EmitNeedLoopVar(type->GetMemberType(i), true, true)) {
            sb.Append(prefix).Append("uint32_t i = 0;\n");
            break;
        }
    }
}

void CCustomTypesCodeEmitter::EmitParamCheck(
    const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("if (data == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid sbuf\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid data block\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void CCustomTypesCodeEmitter::EmitPodTypeUnmarshalling(
    const std::string &typeName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    std::string objPtrName = StringHelper::Format("%sPtr", name.c_str());
    if (Options::GetInstance().DoGenerateKernelCode()) {
        std::string lenName = StringHelper::Format("%sLen", name.c_str());
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufReadBuffer(data, (const void**)&%s, &%s)) {\n", objPtrName.c_str(), lenName.c_str());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read buffer data\", __func__);\n");
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat(
            "if (%s == NULL || %s != sizeof(%s)) {\n", objPtrName.c_str(), lenName.c_str(), typeName.c_str());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid data from reading buffer\", __func__);\n");
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (const %s *)HdfSbufReadUnpadBuffer(data, sizeof(%s));\n",
            typeName.c_str(), objPtrName.c_str(), typeName.c_str(), typeName.c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", objPtrName.c_str());
        sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read buffer data\", __func__);\n");
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
        sb.Append(prefix).Append("}\n\n");
    }

    sb.Append(prefix).AppendFormat("if (memcpy_s(%s, sizeof(%s), %s, sizeof(%s)) != EOK) {\n", name.c_str(),
        typeName.c_str(), objPtrName.c_str(), typeName.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to memcpy data\", __func__);\n");
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
    sb.Append(prefix).Append("}\n\n");
}

void CCustomTypesCodeEmitter::EmitMemberUnmarshalling(const AutoPtr<ASTType> &type, const std::string &name,
    const std::string &memberName, StringBuilder &sb, const std::string &prefix)
{
    std::string varName = StringHelper::Format("%s->%s", name.c_str(), memberName.c_str());
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr) {
        return;
    }
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_STRING: {
            EmitStringMemberUnmarshalling(typeEmitter, memberName, varName, sb, prefix);
            break;
        }
        case TypeKind::TYPE_STRUCT: {
            std::string paramName = StringHelper::Format("&%s", varName.c_str());
            typeEmitter->EmitCUnMarshalling(paramName, ERRORS_LABEL, sb, prefix, freeObjStatements_);
            sb.Append("\n");
            break;
        }
        case TypeKind::TYPE_UNION: {
            std::string tmpName = StringHelper::Format("%sCp", memberName.c_str());
            typeEmitter->EmitCUnMarshalling(tmpName, ERRORS_LABEL, sb, prefix, freeObjStatements_);
            sb.Append(prefix).AppendFormat("if (memcpy_s(&%s, sizeof(%s), %s, sizeof(%s)) != EOK) {\n", varName.c_str(),
                typeEmitter->EmitCType().c_str(), tmpName.c_str(), typeEmitter->EmitCType().c_str());
            sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to memcpy data\", __func__);\n");
            sb.Append(prefix + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
            sb.Append(prefix).Append("}\n");
            break;
        }
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST: {
            EmitArrayMemberUnmarshalling(type, memberName, varName, sb, prefix);
            sb.Append("\n");
            break;
        }
        default: {
            typeEmitter->EmitCUnMarshalling(varName, ERRORS_LABEL, sb, prefix, freeObjStatements_);
            sb.Append("\n");
        }
    }
}

void CCustomTypesCodeEmitter::EmitStringMemberUnmarshalling(const AutoPtr<HdiTypeEmitter> &typeEmitter,
    const std::string &memberName, const std::string &varName, StringBuilder &sb, const std::string &prefix)
{
    std::string tmpName = StringHelper::Format("%sCp", memberName.c_str());
    sb.Append(prefix).Append("{\n");
    typeEmitter->EmitCUnMarshalling(tmpName, ERRORS_LABEL, sb, prefix + TAB, freeObjStatements_);
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB)
            .AppendFormat("%s = (char*)OsalMemCalloc(strlen(%s) + 1);\n", varName.c_str(), tmpName.c_str());
        sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", varName.c_str());
        sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).AppendFormat("if (strcpy_s(%s, (strlen(%s) + 1), %s) != EOK) {\n",
            varName.c_str(), tmpName.c_str(), tmpName.c_str());
        sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
        sb.Append(prefix + TAB).Append("}\n");
    } else {
        sb.Append(prefix + TAB).AppendFormat("%s = strdup(%s);\n", varName.c_str(), tmpName.c_str());
    }

    sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", varName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", ERRORS_LABEL  );
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n");
    sb.Append("\n");
}

void CCustomTypesCodeEmitter::EmitArrayMemberUnmarshalling(const AutoPtr<ASTType> &type, const std::string &memberName,
    const std::string &varName, StringBuilder &sb, const std::string &prefix)
{
    std::string tmpName = StringHelper::Format("%sCp", memberName.c_str());
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    if (typeEmitter == nullptr) {
        return;
    }
    AutoPtr<ASTType> elementType = nullptr;
    if (type->GetTypeKind() == TypeKind::TYPE_ARRAY) {
        AutoPtr<ASTArrayType> arrayType = dynamic_cast<ASTArrayType *>(type.Get());
        elementType = arrayType->GetElementType();
    } else {
        AutoPtr<ASTListType> listType = dynamic_cast<ASTListType *>(type.Get());
        elementType = listType->GetElementType();
    }

    if (elementType->IsStringType()) {
        typeEmitter->EmitCUnMarshalling(varName, ERRORS_LABEL, sb, prefix, freeObjStatements_);
        return;
    }

    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("%s* %s = NULL;\n",
        GetTypeEmitter(elementType)->EmitCType().c_str(), tmpName.c_str());
    sb.Append(prefix + TAB).AppendFormat("uint32_t %sLen = 0;\n", tmpName.c_str());
    typeEmitter->EmitCUnMarshalling(tmpName, ERRORS_LABEL, sb, prefix + TAB, freeObjStatements_);
    sb.Append(prefix + TAB).AppendFormat("%s = %s;\n", varName.c_str(), tmpName.c_str());
    sb.Append(prefix + TAB).AppendFormat("%sLen = %sLen;\n", varName.c_str(), tmpName.c_str());
    sb.Append(prefix).Append("}\n");
}

void CCustomTypesCodeEmitter::EmitCustomTypeFreeImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type) const
{
    std::string objName("dataBlock");
    sb.AppendFormat("void %sFree(%s *%s, bool freeSelf)\n", type->GetName().c_str(),
        GetTypeEmitter(type.Get())->EmitCType().c_str(), objName.c_str());
    sb.Append("{\n");

    if (mode_ == GenMode::KERNEL) {
        for (size_t i = 0; i < type->GetMemberNumber(); i++) {
            AutoPtr<ASTType> memberType = type->GetMemberType(i);
            if (EmitNeedLoopVar(memberType, false, true)) {
                sb.Append(TAB).Append("uint32_t i = 0;\n");
                break;
            }
        }
    }

    sb.Append(TAB).AppendFormat("if (%s == NULL) {\n", objName.c_str());
    sb.Append(TAB).Append(TAB).Append("return;\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("\n");

    EmitCustomTypeMemoryRecycle(type, objName, sb, TAB);

    sb.Append(TAB).Append("if (freeSelf) {\n");
    sb.Append(TAB).Append(TAB).Append("OsalMemFree(dataBlock);\n");
    sb.Append(TAB).Append("}\n");
    sb.Append("}\n");
}

void CCustomTypesCodeEmitter::EmitCustomTypeMemoryRecycle(
    const AutoPtr<ASTStructType> &type, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    for (size_t i = 0; i < type->GetMemberNumber(); i++) {
        AutoPtr<ASTType> memberType = type->GetMemberType(i);
        std::string memberName = type->GetMemberName(i);
        std::string varName = StringHelper::Format("%s->%s", name.c_str(), memberName.c_str());
        switch (memberType->GetTypeKind()) {
            case TypeKind::TYPE_STRING:
            case TypeKind::TYPE_STRUCT:
            case TypeKind::TYPE_ARRAY:
            case TypeKind::TYPE_LIST:
                GetTypeEmitter(memberType)->EmitMemoryRecycle(varName, false, sb, prefix);
                sb.Append("\n");
                break;
            default:
                break;
        }
    }
}

void CCustomTypesCodeEmitter::EmitUtilMethods(StringBuilder &sb, bool isDecl)
{
    UtilMethodMap methods;
    for (const auto &typePair : ast_->GetTypes()) {
        AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(typePair.second);
        if (typeEmitter != nullptr) {
            typeEmitter->EmitCWriteMethods(methods, "", "", isDecl);
            typeEmitter->EmitCStubReadMethods(methods, "", "", isDecl);
        }
    }
    EmitUtilMethodMap(sb, methods);
}
} // namespace Idl
} // namespace OHOS