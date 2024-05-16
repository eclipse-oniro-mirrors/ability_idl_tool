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

#include "hdi_string_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiStringTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_STRING;
}

std::string HdiStringTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "char*";
        case TypeMode::PARAM_IN:
            return "const char*";
        case TypeMode::PARAM_OUT:
            return "char*";
        case TypeMode::LOCAL_VAR:
            return "char*";
        default:
            return "unknow type";
    }
}

std::string HdiStringTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "std::string";
        case TypeMode::PARAM_IN:
            return "const std::string&";
        case TypeMode::PARAM_OUT:
            return "std::string&";
        case TypeMode::LOCAL_VAR:
            return "std::string";
        default:
            return "unknow type";
    }
}

std::string HdiStringTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return "std::string";
}

void HdiStringTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteString(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCProxyWriteOutVar(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL || %s == 0) {\n", name.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: %s is invalid\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n", dataParcelName_.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");
}

void HdiStringTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(%s);\n", name.c_str(), replyParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(%s);\n",
        name.c_str(), dataParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCStubReadOutVar(const std::string &memFlagName, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());

    sb.Append(prefix).AppendFormat("if (%s) {\n", memFlagName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n", dataParcelName_.c_str(),
        lenName.c_str());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).AppendFormat("%s(%s, >, %s / sizeof(char), %s, HDF_ERR_INVALID_PARAM, %s);\n",
        CHECK_VALUE_RET_GOTO_MACRO, lenName.c_str(), MAX_BUFF_SIZE_MACRO, errorCodeName_.c_str(), gotoLabel.c_str());

    sb.Append(prefix + TAB).AppendFormat("if (%s > 0) {\n", lenName.c_str());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("%s = (%s)OsalMemCalloc(%s);\n", name.c_str(), EmitCType().c_str(), lenName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("} else {\n");
    sb.Append(prefix + TAB)
        .AppendFormat("%s = (%s)OsalMemCalloc(%s);\n", name.c_str(), EmitCType().c_str(), MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n");

    sb.Append(prefix + TAB).AppendFormat("%sLen = %s;\n", name.c_str(), MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix).Append("}\n\n");
}

void HdiStringTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteCString(%s.c_str())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("const char* %sCp = %s.ReadCString();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%sCp == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s = %sCp;\n", EmitCppType().c_str(), name.c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %sCp;\n", name.c_str(), name.c_str());
    }
}

void HdiStringTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteString(data, %s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("const char *%s = HdfSbufReadString(data);\n", name.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteCString(%s.c_str())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("const char* %s = %s.ReadCString();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("OsalMemFree(%s);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStringTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("%s.writeString(%s);\n", parcelName.c_str(), name.c_str());
}

void HdiStringTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = %s.readString();\n", name.c_str(), parcelName.c_str());
}

void HdiStringTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s %s = %s.readString();\n", EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS