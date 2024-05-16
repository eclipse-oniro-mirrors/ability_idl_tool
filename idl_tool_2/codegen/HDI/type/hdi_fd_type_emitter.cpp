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

#include "hdi_fd_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiFdTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_FILEDESCRIPTOR;
}

std::string HdiFdTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int";
        case TypeMode::PARAM_IN:
            return "int";
        case TypeMode::PARAM_OUT:
            return "int*";
        case TypeMode::LOCAL_VAR:
            return "int";
        default:
            return "unknow type";
    }
}

std::string HdiFdTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "int";
        case TypeMode::PARAM_IN:
            return "int";
        case TypeMode::PARAM_OUT:
            return "int&";
        case TypeMode::LOCAL_VAR:
            return "int";
        default:
            return "unknow type";
    }
}

std::string HdiFdTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return isInnerType ? "Integer" : "int";
}

void HdiFdTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!WriteFileDescriptor(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    if (isInnerType) {
        sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(%s, &%s)) {\n", replyParcelName_.c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(%s, %s)) {\n", replyParcelName_.c_str(), name.c_str());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(%s, &%s)) {\n", dataParcelName_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!WriteFileDescriptor(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s = -1;\n", EmitCppType().c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = -1;\n", name.c_str());
    }

    sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).AppendFormat("}\n");
}

void HdiFdTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!WriteFileDescriptor(data, %s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(data, &%s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!WriteFileDescriptor(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiFdTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s = -1;\n", EmitCppType().c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = -1;\n", name.c_str());
    }

    sb.Append(prefix).AppendFormat("if (!ReadFileDescriptor(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).AppendFormat("}\n");
}

void HdiFdTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s);\n", parcelName.c_str(), name.c_str());
}

void HdiFdTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readInt();\n", name.c_str(), parcelName.c_str());
}

void HdiFdTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s %s = %s.readInt();\n", EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(), parcelName.c_str());
}

void HdiFdTypeEmitter::EmitCWriteMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWrite%s", methodPrefix.c_str(), typeName_.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *data, int fd);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *data, int fd)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (!HdfSbufWriteInt8(data, fd >= 0 ? 1 : 0)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write fd vailed\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("if (!HdfSbufWriteFileDescriptor(data, fd)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write fd\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiFdTypeEmitter::EmitCReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sRead%s", methodPrefix.c_str(), typeName_.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *data, int *fd);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *data, int *fd)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (data == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid HdfSBuf obj\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (fd == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid fd pointer\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("*fd = -1;\n");
        sb.Append(prefix + TAB).Append("bool fdValied = false;\n");
        sb.Append(prefix + TAB).Append("if (!HdfSbufReadInt8(data, (int8_t*)&fdValied)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read fdValied\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (!fdValied) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("*fd = HdfSbufReadFileDescriptor(data);\n");
        sb.Append(prefix + TAB).Append("if (*fd < 0) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read fd\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiFdTypeEmitter::EmitCStubReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    EmitCReadMethods(methods, prefix, methodPrefix, isDecl);
}

void HdiFdTypeEmitter::EmitCppWriteMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWrite%s", methodPrefix.c_str(), typeName_.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static bool %s(MessageParcel &data, int fd);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat("static bool %s(MessageParcel &data, int fd)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (!data.WriteBool(fd >= 0 ? true : false)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write fd vailed\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (fd < 0) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (!data.WriteFileDescriptor(fd)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write fd\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiFdTypeEmitter::EmitCppReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sRead%s", methodPrefix.c_str(), typeName_.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static bool %s(MessageParcel &data, int &fd);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.Append(prefix).AppendFormat("static bool %s(MessageParcel &data, int &fd)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("fd = -1;\n");
        sb.Append(prefix + TAB).Append("bool fdValid = false;\n");
        sb.Append(prefix + TAB).Append("if (!data.ReadBool(fdValid)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read fdValid\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("if (fdValid) {\n");
        sb.Append(prefix + TAB + TAB).Append("fd = data.ReadFileDescriptor();\n");
        sb.Append(prefix + TAB + TAB).Append("if (fd < 0) {\n");
        sb.Append(prefix + TAB + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read fd\", __func__);\n");
        sb.Append(prefix + TAB + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}
} // namespace Idl
} // namespace OHOS