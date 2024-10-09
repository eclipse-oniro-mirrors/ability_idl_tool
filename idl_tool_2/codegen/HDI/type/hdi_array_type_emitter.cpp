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

#include "hdi_array_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiArrayTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ARRAY;
}

std::string HdiArrayTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("%s*", elementEmitter_->EmitCType(TypeMode::NO_MODE).c_str());
        case TypeMode::PARAM_IN: {
            TypeKind elemType = elementEmitter_->GetTypeKind();
            if ((elemType == TypeKind::TYPE_STRING) || (elemType == TypeKind::TYPE_NATIVE_BUFFER)) {
                return StringHelper::Format("%s*", elementEmitter_->EmitCType(TypeMode::NO_MODE).c_str());
            }
            return StringHelper::Format("const %s*", elementEmitter_->EmitCType(TypeMode::NO_MODE).c_str());
        }
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s*", elementEmitter_->EmitCType(TypeMode::NO_MODE).c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("%s*", elementEmitter_->EmitCType(TypeMode::NO_MODE).c_str());
        default:
            return "unknown type";
    }
}

std::string HdiArrayTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("std::vector<%s>", elementEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const std::vector<%s>&", elementEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("std::vector<%s>&", elementEmitter_->EmitCppType().c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("std::vector<%s>", elementEmitter_->EmitCppType().c_str());
        default:
            return "unknow type";
    }
}

std::string HdiArrayTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return StringHelper::Format("%s[]", elementEmitter_->EmitJavaType(TypeMode::NO_MODE, false).c_str());
}

void HdiArrayTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!WritePodArray(%s, %s, sizeof(%s), %s)) {\n", parcelName.c_str(),
            name.c_str(), elementEmitter_->EmitCType().c_str(), lenName.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    } else if (elemType == TypeKind::TYPE_STRING) {
        sb.Append(prefix).AppendFormat(
            "if (!WriteStringArray(%s, %s, %s)) {\n", parcelName.c_str(), name.c_str(), lenName.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if ((%s == NULL && %s != 0) || (%s != NULL && %s == 0)) {\n",
        name.c_str(), lenName.c_str(), name.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: %s is invalid\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n", parcelName.c_str(), lenName.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.c_str());
    }

    std::string elementName = "";
    if ((elemType == TypeKind::TYPE_STRUCT) || (elemType == TypeKind::TYPE_UNION)) {
        elementName = StringHelper::Format("&%s[i]", name.c_str());
    } else {
        elementName = StringHelper::Format("%s[i]", name.c_str());
    }
    elementEmitter_->EmitCWriteVar(mode, elementName, gotoLabel, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCProxyWriteOutVar(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL || %s == NULL || *%s == 0) {\n", name.c_str(), lenName.c_str(),
        lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: %s is invalid\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, *%s)) {\n", dataParcelName_.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");
}

void HdiArrayTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!ReadPodArray(%s, %s, sizeof(%s), %s)) {\n", replyParcelName_.c_str(),
            name.c_str(), elementEmitter_->EmitCType().c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    } else if (elemType == TypeKind::TYPE_STRING) {
        sb.Append(prefix).AppendFormat(
            "if (!ReadStringArray(%s, %s, %s)) {\n", replyParcelName_.c_str(), name.c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, %s)) {\n", replyParcelName_.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("%s(*%s, >, %s / sizeof(%s), %s, HDF_ERR_INVALID_PARAM, %s);\n",
        CHECK_VALUE_RET_GOTO_MACRO, lenName.c_str(), MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCType().c_str(),
        errorCodeName_.c_str(), gotoLabel.c_str());

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < *%s; i++) {\n", lenName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < *%s; i++) {\n", lenName.c_str());
    }

    if (elemType == TypeKind::TYPE_STRUCT) {
        std::string element = StringHelper::Format("&%s[i]", name.c_str());
        elementEmitter_->EmitCProxyReadVar(element, true, gotoLabel, sb, prefix + TAB);
    } else if ((elemType == TypeKind::TYPE_FILEDESCRIPTOR) || (elemType == TypeKind::TYPE_NATIVE_BUFFER)) {
        std::string element = StringHelper::Format("%s[i]", name.c_str());
        elementEmitter_->EmitCProxyReadVar(element, true, gotoLabel, sb, prefix + TAB);
    } else {
        std::string element = StringHelper::Format("&%s[i]", name.c_str());
        elementEmitter_->EmitCProxyReadVar(element, true, gotoLabel, sb, prefix + TAB);
    }
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!ReadPodArray(%s, (void **)&%s, sizeof(%s), &%s)) {\n",
            dataParcelName_.c_str(), name.c_str(), elementEmitter_->EmitCType().c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    } else if (elemType == TypeKind::TYPE_STRING) {
        sb.Append(prefix).AppendFormat(
            "if (!ReadStringArray(%s, &%s, &%s)) {\n", dataParcelName_.c_str(), name.c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n", dataParcelName_.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("%s(%s, >, %s / sizeof(%s), %s, HDF_ERR_INVALID_PARAM, %s);\n",
        CHECK_VALUE_RET_GOTO_MACRO, lenName.c_str(), MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCType().c_str(),
        errorCodeName_.c_str(), gotoLabel.c_str());

    sb.Append(prefix).AppendFormat("if (%s > 0) {\n", lenName.c_str());
    EmitCMallocVar(name, lenName, gotoLabel, sb, prefix + TAB);
    sb.Append("\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.c_str());
    } else {
        sb.Append(prefix + TAB).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.c_str());
    }
    if (elemType == TypeKind::TYPE_STRUCT) {
        std::string element = StringHelper::Format("&%s[i]", name.c_str());
        elementEmitter_->EmitCStubReadVar(element, gotoLabel, sb, prefix + TAB + TAB);
    } else if ((elemType == TypeKind::TYPE_FILEDESCRIPTOR) || (elemType == TypeKind::TYPE_NATIVE_BUFFER)) {
        std::string element = StringHelper::Format("%s[i]", name.c_str());
        elementEmitter_->EmitCStubReadVar(element, gotoLabel, sb, prefix + TAB + TAB);
    } else {
        std::string element = StringHelper::Format("&%s[i]", name.c_str());
        elementEmitter_->EmitCStubReadVar(element, gotoLabel, sb, prefix + TAB + TAB);
    }
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCStubReadOutVar(const std::string &memFlagName, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());

    sb.Append(prefix).AppendFormat("if (%s) {\n", memFlagName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!HdfSbufReadUint32(%s, &%s)) {\n",
        dataParcelName_.c_str(), lenName.c_str());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: read %s size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).AppendFormat("%s(%s, >, %s / sizeof(%s), %s, HDF_ERR_INVALID_PARAM, %s);\n",
        CHECK_VALUE_RET_GOTO_MACRO, lenName.c_str(), MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCType().c_str(),
        errorCodeName_.c_str(), gotoLabel.c_str());

    sb.Append(prefix + TAB).AppendFormat("if (%s > 0) {\n", lenName.c_str());
    EmitCMallocVar(name, lenName, gotoLabel, sb, prefix + TAB + TAB);
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("} else {\n");
    sb.Append(prefix + TAB).AppendFormat("%s = (%s*)OsalMemCalloc(%s);\n", name.c_str(),
        elementEmitter_->EmitCType().c_str(), MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n");

    sb.Append(prefix + TAB).AppendFormat("%sLen = (%s / sizeof(%s));\n", name.c_str(),
        MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCType().c_str());
    sb.Append(prefix).Append("}\n\n");
}

void HdiArrayTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!WritePodArray(%s, %s)) {\n", parcelName.c_str(), name.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");

    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!%s.empty()) {\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("if (!%s.WriteUnpadBuffer(", parcelName.c_str());
        sb.AppendFormat("(const void*)%s.data(), sizeof(%s) * %s.size())) {\n", name.c_str(),
            elementEmitter_->EmitCppType().c_str(), name.c_str());
        sb.Append(prefix + TAB + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix + TAB).Append("}\n");
        sb.Append(prefix).Append("}\n");
    } else {
        std::string elementName = StringHelper::Format("it%d", innerLevel++);
        sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.c_str(), name.c_str());
        elementEmitter_->EmitCppWriteVar(parcelName, elementName, sb, prefix + TAB, innerLevel);
        sb.Append(prefix).Append("}\n");
    }
}

void HdiArrayTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), name.c_str());
    }

    if ((elementEmitter_->GetTypeKind() != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!ReadPodArray(%s, %s)) {\n", parcelName.c_str(), name.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("uint32_t %sSize = 0;\n", name.c_str());
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%sSize)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read size\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("%s(%sSize, >, %s / sizeof(%s), HDF_ERR_INVALID_PARAM);\n",
        CHECK_VALUE_RETURN_MACRO, name.c_str(), MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCppType().c_str());
    sb.Append(prefix).AppendFormat("%s.clear();\n", name.c_str());
    sb.Append(prefix).AppendFormat("%s.reserve(%sSize);\n", name.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat(
        "for (uint32_t i%d = 0; i%d < %sSize; ++i%d) {\n", innerLevel, innerLevel, name.c_str(), innerLevel);
    std::string valueName = StringHelper::Format("value%d", innerLevel++);
    elementEmitter_->EmitCppReadVar(valueName, sb, prefix + TAB, mode, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.c_str(), valueName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!WritePodArray(data, %s, sizeof(%s), %s)) {\n", name.c_str(),
            elementEmitter_->EmitCType().c_str(), lenName.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return false;\n");
        sb.Append(prefix).Append("}\n");
        return;
    } else if (elemType == TypeKind::TYPE_STRING) {
        sb.Append(prefix).AppendFormat("if (!WriteStringArray(data, %s, %s)) {\n", name.c_str(), lenName.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return false;\n");
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if (%s > %s / sizeof(%s) || !HdfSbufWriteUint32(data, %s)) {\n", lenName.c_str(),
        MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCType().c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", lenName.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("if ((%s == NULL && %s != 0) || (%s != NULL && %s == 0)) {\n",
        name.c_str(), lenName.c_str(), name.c_str(), lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: %s is invalid\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.c_str());
    }

    std::string elementName = StringHelper::Format("(%s)[i]", name.c_str());
    elementEmitter_->EmitCMarshalling(elementName, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    std::string lenName = StringHelper::Format("%sLen", name.c_str());
    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!ReadPodArray(data, (void**)&%s, sizeof(%s), &%s)) {\n", name.c_str(),
            elementEmitter_->EmitCType().c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    } else if (elemType == TypeKind::TYPE_STRING) {
        sb.Append(prefix).AppendFormat("if (!ReadStringArray(data, &%s, &%s)) {\n", name.c_str(), lenName.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    }

    EmitCElementUnMarshallingReadLen(lenName, gotoLabel, sb, prefix);
    sb.Append(prefix).AppendFormat("if (%s > 0) {\n", lenName.c_str());
    std::string newPrefix = prefix + TAB;

    sb.Append(newPrefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s) * %s);\n", name.c_str(),
        elementEmitter_->EmitCType().c_str(), elementEmitter_->EmitCType().c_str(), lenName.c_str());
    sb.Append(newPrefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(newPrefix).Append("}\n");
    freeObjStatements.push_back(StringHelper::Format("OsalMemFree(%s);\n", name.c_str()));

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(newPrefix).AppendFormat("for (i = 0; i < %s; i++) {\n", lenName.c_str());
    } else {
        sb.Append(newPrefix).AppendFormat("for (uint32_t i = 0; i < %s; i++) {\n", lenName.c_str());
    }

    if (elemType == TypeKind::TYPE_STRING) {
        EmitCStringElementUnMarshalling(name, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    } else if (elemType == TypeKind::TYPE_STRUCT) {
        std::string element = StringHelper::Format("&%s[i]", name.c_str());
        elementEmitter_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    } else {
        std::string element = StringHelper::Format("%s[i]", name.c_str());
        elementEmitter_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix + TAB, freeObjStatements);
    }
    sb.Append(newPrefix).Append("}\n");
    sb.Append(prefix).Append("}\n");
    freeObjStatements.pop_back();
}

void HdiArrayTypeEmitter::EmitCElementUnMarshallingReadLen(const std::string &lenName, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(data, &%s)) {\n", lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");

    sb.Append(prefix).AppendFormat("if (%s > %s / sizeof(%s)) {\n", lenName.c_str(), MAX_BUFF_SIZE_MACRO,
        elementEmitter_->EmitCType().c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: %s is invalid data\", __func__);\n", lenName.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCStringElementUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &newPrefix, std::vector<std::string> &freeObjStatements) const
{
    std::string element = StringHelper::Format("%sElement", name.c_str());
    elementEmitter_->EmitCUnMarshalling(element, gotoLabel, sb, newPrefix, freeObjStatements);
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(newPrefix).AppendFormat(
            "%s[i] = (char*)OsalMemCalloc(strlen(%s) + 1);\n", name.c_str(), element.c_str());
        sb.Append(newPrefix).AppendFormat("if (%s[i] == NULL) {\n", name.c_str());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(newPrefix).Append("}\n\n");
        sb.Append(newPrefix).AppendFormat(
            "if (strcpy_s((%s)[i], (strlen(%s) + 1), %s) != EOK) {\n", name.c_str(), element.c_str(), element.c_str());
        sb.Append(newPrefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", element.c_str());
        sb.Append(newPrefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(newPrefix).Append("}\n");
    } else {
        sb.Append(newPrefix).Append(TAB).AppendFormat("%s[i] = strdup(%s);\n", name.c_str(), element.c_str());
    }
}

void HdiArrayTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    if ((elementEmitter_->GetTypeKind() != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!WritePodArray(%s, %s)) {\n", parcelName.c_str(), name.c_str());
        sb.Append(prefix + TAB)
            .AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return false;\n");
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed write %s.size\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
    std::string elementName = StringHelper::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.c_str(), name.c_str());
    elementEmitter_->EmitCppMarshalling(parcelName, elementName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    size_t index = name.find('.');
    std::string memberName = (index == std::string::npos) ? name : StringHelper::SubStr(name, index + 1);
    std::string sizeName = StringHelper::Format("%sSize", memberName.c_str());
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), memberName.c_str());
    }

    TypeKind elemType = elementEmitter_->GetTypeKind();
    if ((elemType != TypeKind::TYPE_BOOLEAN) && elementEmitter_->IsPod()) {
        sb.Append(prefix).AppendFormat("if (!ReadPodArray(%s, %s)) {\n", parcelName.c_str(), name.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to read %s\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).Append("return false;\n");
        sb.Append(prefix).Append("}\n");
        return;
    }

    sb.Append(prefix).AppendFormat("uint32_t %s = 0;\n", sizeName.c_str());
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%s)) {\n", parcelName.c_str(), sizeName.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read size\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("%s(%s, >, %s / sizeof(%s), false);\n", CHECK_VALUE_RETURN_MACRO, sizeName.c_str(),
        MAX_BUFF_SIZE_MACRO, elementEmitter_->EmitCppType().c_str());
    sb.Append(prefix).AppendFormat("%s.clear();\n", name.c_str());
    sb.Append(prefix).AppendFormat("%s.reserve(%s);\n", name.c_str(), sizeName.c_str());
    sb.Append(prefix).AppendFormat(
        "for (uint32_t i%d = 0; i%d < %s; ++i%d) {\n", innerLevel, innerLevel, sizeName.c_str(), innerLevel);
    std::string valueName = StringHelper::Format("value%d", innerLevel++);
    if (elemType == TypeKind::TYPE_STRUCT) {
        sb.Append(prefix + TAB).AppendFormat("%s %s;\n", elementEmitter_->EmitCppType().c_str(), valueName.c_str());
    }
    elementEmitter_->EmitCppUnMarshalling(parcelName, valueName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.c_str(), valueName.c_str());

    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    std::string varName = name;
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", varName.c_str());
    auto elementTypeNeedFree = [this]() -> bool {
        if (elementEmitter_->IsPod()) {
            return false;
        }

        TypeKind elemType = elementEmitter_->GetTypeKind();
        if ((elemType == TypeKind::TYPE_STRUCT) || (elemType == TypeKind::TYPE_STRING) ||
            (elemType == TypeKind::TYPE_NATIVE_BUFFER)) {
            return true;
        }
        return false;
    };
    if (elementTypeNeedFree()) {
        if (Options::GetInstance().DoGenerateKernelCode()) {
            sb.Append(prefix + TAB).AppendFormat("for (i = 0; i < %sLen; i++) {\n", name.c_str());
        } else {
            sb.Append(prefix + TAB).AppendFormat("for (uint32_t i = 0; i < %sLen; i++) {\n", name.c_str());
        }

        std::string elementName = StringHelper::Format("%s[i]", varName.c_str());
        elementEmitter_->EmitMemoryRecycle(elementName, false, sb, prefix + TAB + TAB);
        sb.Append(prefix + TAB).Append("}\n");
    }

    sb.Append(prefix + TAB).AppendFormat("OsalMemFree(%s);\n", varName.c_str());
    if (!ownership) {
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", varName.c_str());
    }
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    if (mode == TypeMode::PARAM_OUT) {
        sb.Append(prefix).AppendFormat("if (%s == null) {\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s.writeInt(-1);\n", parcelName.c_str());
        sb.Append(prefix).Append("} else {\n");
        sb.Append(prefix + TAB).AppendFormat("%s.writeInt(%s.length);\n", parcelName.c_str(), name.c_str());
        sb.Append(prefix).Append("}\n");
        return;
    }
    sb.Append(prefix).AppendFormat("if (%s == null) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s.writeInt(-1);\n", parcelName.c_str());
    sb.Append(prefix).Append("} else {\n");
    EmitJavaWriteArrayVar(parcelName, name, sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    switch (elementEmitter_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s.read%sArray(%s);\n",
                StringHelper::FirstToUpper(elementEmitter_->GetTypeName()).c_str(),
                parcelName.c_str(), name.c_str());
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s.readIntArray(%s);\n", parcelName.c_str(), name.c_str());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("%s.readSequenceableArray(%s);\n", parcelName.c_str(), name.c_str());
            break;
        default:
            break;
    }
}

void HdiArrayTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    switch (elementEmitter_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.read%sArray();\n",
                StringHelper::FirstToUpper(elementEmitter_->GetTypeName()).c_str(),
                elementEmitter_->EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(), parcelName.c_str());
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s[] %s = %s.readIntArray();\n",
                elementEmitter_->EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(), parcelName.c_str());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("int size = %s.readInt();\n", parcelName.c_str());
            sb.Append(prefix).AppendFormat("%s %s = new %s[size];\n",
                elementEmitter_->EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(),
                elementEmitter_->EmitJavaType(TypeMode::NO_MODE).c_str());
            sb.Append(prefix).AppendFormat("for (int i = 0; i < size; ++i) {\n");
                elementEmitter_->EmitJavaReadInnerVar(parcelName, "value", true, sb, prefix + TAB);
            sb.Append(prefix + TAB).AppendFormat("%s[i] = value;\n", name.c_str());
            sb.Append(prefix).Append("}\n");
            break;
        default:
            break;
    }
}

void HdiArrayTypeEmitter::EmitJavaWriteArrayVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    switch (elementEmitter_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s.write%sArray(%s);\n",
                StringHelper::FirstToUpper(elementEmitter_->GetTypeName()).c_str(), parcelName.c_str(), name.c_str());
            break;
        case TypeKind::TYPE_FILEDESCRIPTOR:
            sb.Append(prefix).AppendFormat("%s.writeIntArray(%s);\n", parcelName.c_str(), name.c_str());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("%s.writeSequenceableArray(%s);\n", parcelName.c_str(), name.c_str());
            break;
        default:
            break;
    }
}

void HdiArrayTypeEmitter::EmitCMallocVar(const std::string &name, const std::string &lenName,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = (%s*)OsalMemCalloc(sizeof(%s) * (%s));\n", name.c_str(),
        elementEmitter_->EmitCType().c_str(), elementEmitter_->EmitCType().c_str(), lenName.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: malloc %s failed\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_MALLOC_FAIL;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCWriteMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    elementEmitter_->EmitCWriteMethods(methods, prefix, methodPrefix, isDecl);
    if (elementEmitter_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCWriteStrArrayMethods(methods, prefix, methodPrefix, isDecl);
        return;
    } else if (!elementEmitter_->IsPod()) {
        return;
    }
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWritePodArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, const void *data, uint32_t elementSize, "
            "uint32_t count);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, const void *data, uint32_t elementSize, "
            "uint32_t count)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (!HdfSbufWriteUint32(parcel, count)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write array size\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");

        sb.Append(prefix + TAB).Append("if (data == NULL && count == 0) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n\n");

        if (Options::GetInstance().DoGenerateKernelCode()) {
            sb.Append(prefix + TAB).Append("if (!HdfSbufWriteBuffer(");
            sb.Append("parcel, (const void *)data, elementSize * count)) {\n");
        } else {
            sb.Append(prefix + TAB).Append("if (!HdfSbufWriteUnpadBuffer(");
            sb.Append("parcel, (const uint8_t *)data, elementSize * count)) {\n");
        }

        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write array\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");

        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    elementEmitter_->EmitCReadMethods(methods, prefix, methodPrefix, isDecl);
    if (elementEmitter_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCReadStrArrayMethods(methods, prefix, methodPrefix, isDecl);
        return;
    } else if (!elementEmitter_->IsPod()) {
        return;
    }
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sReadPodArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, void *data, uint32_t elementSize, uint32_t *count);\n",
            methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, void *data, uint32_t elementSize, uint32_t *count)\n",
            methodName.c_str());
        EmitCReadMethodsBody(sb, prefix);
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCReadMethodsBody(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("{\n");
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB).Append("void *dataPtr = NULL;\n");
        sb.Append(prefix + TAB).Append("uint32_t dataLen = 0;\n");
    }
    sb.Append(prefix + TAB).Append("uint32_t elementCount = 0;\n");
    sb.Append(prefix + TAB).Append("if (!HdfSbufReadUint32(parcel, &elementCount)) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read array size\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return false;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append(prefix + TAB).AppendFormat("if (elementCount > %s / elementSize) {\n", MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid elementCount\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return false;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append(prefix + TAB).Append("if (elementCount == 0) {\n");
    sb.Append(prefix + TAB + TAB).Append("goto FINISHED;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix + TAB).Append("if (!HdfSbufReadBuffer(parcel, (const void **)&dataPtr, &dataLen)) {\n");
    } else {
        sb.Append(prefix + TAB).Append("const void * dataPtr = HdfSbufReadUnpadBuffer(parcel, "
            "elementSize * elementCount);\n");
        sb.Append(prefix + TAB).Append("if (dataPtr == NULL) {\n");
    }

    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return false;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append(prefix + TAB).Append("if (memcpy_s(data, elementSize * elementCount, dataPtr, "
        "elementSize * elementCount) != EOK) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to copy array data\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("return false;\n");
    sb.Append(prefix + TAB).Append("}\n\n");

    sb.Append("FINISHED:\n");
    sb.Append(prefix + TAB).Append("*count = elementCount;\n");
    sb.Append(prefix + TAB).Append("return true;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiArrayTypeEmitter::EmitCStubReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    elementEmitter_->EmitCStubReadMethods(methods, prefix, methodPrefix, isDecl);
    if (elementEmitter_->GetTypeKind() == TypeKind::TYPE_STRING) {
        EmitCStubReadStrArrayMethods(methods, prefix, methodPrefix, isDecl);
        return;
    } else if (!elementEmitter_->IsPod()) {
        return;
    }
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sReadPodArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, void **data, uint32_t elementSize, uint32_t *count);\n",
            methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, void **data, uint32_t elementSize, uint32_t *count)\n",
            methodName.c_str());
        sb.Append(prefix).Append("{\n");
        EmitCStubReadMethodBody(sb, prefix + TAB);
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCStubReadMethodBody(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("const void * dataPtr = NULL;\n");
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).Append("uint32_t dataLen = 0;\n");
    }
    sb.Append(prefix).Append("void *memPtr = NULL;\n");
    sb.Append(prefix).Append("uint32_t elementCount = 0;\n");
    sb.Append(prefix).Append("if (count == NULL || data == NULL || elementSize == 0) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid param\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("if (!HdfSbufReadUint32(parcel, &elementCount)) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read element count\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).AppendFormat("if (elementCount > %s / elementSize) {\n", MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid elementCount\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    sb.Append(prefix).Append("if (elementCount == 0) {\n");
    sb.Append(prefix + TAB).Append("*count = elementCount;\n");
    sb.Append(prefix + TAB).Append("return true;\n");
    sb.Append(prefix).Append("}\n\n");
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).Append("if (!HdfSbufReadBuffer(parcel, (const void **)&dataPtr, &dataLen)) {\n");
    } else {
        sb.Append(prefix).Append("dataPtr = HdfSbufReadUnpadBuffer(parcel, elementSize * elementCount);\n");
        sb.Append(prefix).Append("if (dataPtr == NULL) {\n");
    }
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read buffer data\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("memPtr = OsalMemCalloc(elementSize * elementCount);\n");
    sb.Append(prefix).Append("if (memPtr == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc buffer\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append(
        "if (memcpy_s(memPtr, elementSize * elementCount, dataPtr, elementSize * elementCount) != EOK) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to memcpy buffer\", __func__);\n");
    sb.Append(prefix + TAB).Append("OsalMemFree(memPtr);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("*data = memPtr;\n");
    sb.Append(prefix).Append("*count = elementCount;\n");
    sb.Append(prefix).Append("return true;\n");
}

void HdiArrayTypeEmitter::EmitCWriteStrArrayMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWriteStringArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char **data, uint32_t count);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char **data, uint32_t count)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("uint32_t i = 0;\n");
        sb.Append(prefix + TAB).Append("if (parcel == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid sbuf\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).AppendFormat("if (count > %s / sizeof(data[0])) {\n", MAX_BUFF_SIZE_MACRO);
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid count\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("if (!HdfSbufWriteUint32(parcel, count)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write count of array\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("if (count == 0) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("if (data == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid array object\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("for (i = 0; i < count; i++) {\n");
        sb.Append(prefix + TAB + TAB).Append("if (!HdfSbufWriteString(parcel, data[i])) {\n");
        sb.Append(prefix + TAB + TAB + TAB)
            .Append("HDF_LOGE(\"%{public}s: failed to write element of array\", __func__);\n");
        sb.Append(prefix + TAB + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB + TAB).Append("}\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCReadStrArrayMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sReadStringArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char **data, uint32_t *count);\n", methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char **data, uint32_t *count)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        EmitCReadStrArrayMethodBody(sb, prefix + TAB);
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCReadStrArrayMethodBody(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("uint32_t i = 0;\n");
    sb.Append(prefix).Append("uint32_t dataCount = 0;\n");
    EmitCCheckParamOfReadStringArray(sb, prefix);
    sb.Append(prefix).Append("if (!HdfSbufReadUint32(parcel, &dataCount)) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read count of array\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (dataCount > %s / sizeof(data[0])) {\n", MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid dataCount\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("if (dataCount == 0) {\n");
    sb.Append(prefix + TAB).Append("*count = dataCount;\n");
    sb.Append(prefix + TAB).Append("return true;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("for (i = 0; i < dataCount; i++) {\n");
    sb.Append(prefix + TAB).Append("char *elementStr = NULL;\n");
    sb.Append(prefix + TAB).Append("uint32_t strLen = 0;\n");
    sb.Append(prefix + TAB).Append("const char *str = HdfSbufReadString(parcel);\n");
    sb.Append(prefix + TAB).Append("if (str == NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read string\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("strLen = strlen(str);\n");
    sb.Append(prefix + TAB).Append("elementStr = (char *)OsalMemCalloc(strLen + 1);\n");
    sb.Append(prefix + TAB).Append("if (elementStr == NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc element of array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("if (strcpy_s(elementStr, strLen + 1, str) != EOK) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed copy element of array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("OsalMemFree(elementStr);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("data[i] = elementStr;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("*count = dataCount;\n");
    sb.Append(prefix).Append("return true;\n\n");
    sb.Append("ERROR:\n");
    sb.Append(prefix).Append("for (i = 0; i < dataCount; ++i) {\n");
    sb.Append(prefix + TAB).Append("if (data[i] != NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("OsalMemFree(data[i]);\n");
    sb.Append(prefix + TAB + TAB).Append("data[i] = NULL;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).Append("*count = 0;\n");
    sb.Append(prefix).Append("return false;\n");
}

void HdiArrayTypeEmitter::EmitCCheckParamOfReadStringArray(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("if (parcel == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid sbuf\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("if (data == NULL || count == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid array object\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
}

void HdiArrayTypeEmitter::EmitCStubReadStrArrayMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sReadStringArray", methodPrefix.c_str());
    if (isDecl) {
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char ***data, uint32_t *count);\n",
            methodName.c_str());
    } else {
        sb.Append("\n");
        sb.AppendFormat("static bool %s(struct HdfSBuf *parcel, char ***data, uint32_t *count)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        EmitCStubReadStrArrayMethodBody(sb, prefix + TAB);
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCStubReadStrArrayMethodBody(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).Append("uint32_t i = 0;\n");
    sb.Append(prefix).Append("char **dataPtr = NULL;\n");
    sb.Append(prefix).Append("uint32_t dataCount = 0;\n");
    EmitCCheckParamOfReadStringArray(sb, prefix);
    sb.Append(prefix).Append("if (!HdfSbufReadUint32(parcel, &dataCount)) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read count of array\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (dataCount > %s / sizeof(data[0])) {\n", MAX_BUFF_SIZE_MACRO);
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: invalid dataCount\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("if (dataCount == 0) {\n");
    sb.Append(prefix + TAB).Append("*count = dataCount;\n");
    sb.Append(prefix + TAB).Append("return true;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("dataPtr = (char **)OsalMemCalloc(sizeof(char *) * dataCount);\n");
    sb.Append(prefix).Append("if (dataPtr == NULL) {\n");
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc array\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("for (i = 0; i < dataCount; i++) {\n");
    sb.Append(prefix + TAB).Append("char *elementStr = NULL;\n");
    sb.Append(prefix + TAB).Append("uint32_t strLen = 0;\n");
    sb.Append(prefix + TAB).Append("const char *str = HdfSbufReadString(parcel);\n");
    sb.Append(prefix + TAB).Append("if (str == NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).Append("strLen = strlen(str);\n");
    sb.Append(prefix + TAB).Append("elementStr = (char *)OsalMemCalloc(strLen + 1);\n");
    sb.Append(prefix + TAB).Append("if (elementStr == NULL) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to malloc element of array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).Append("if (strcpy_s(elementStr, strLen + 1, str) != EOK) {\n");
    sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed copy element of array\", __func__);\n");
    sb.Append(prefix + TAB + TAB).Append("OsalMemFree(elementStr);\n");
    sb.Append(prefix + TAB + TAB).Append("goto ERROR;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("dataPtr[i] = elementStr;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("*count = dataCount;\n");
    sb.Append(prefix).Append("*data = dataPtr;\n");
    sb.Append(prefix).Append("return true;\n\n");
    EmitCStubReadStrArrayFree(sb, prefix);
}

void HdiArrayTypeEmitter::EmitCStubReadStrArrayFree(StringBuilder &sb, const std::string &prefix) const
{
    sb.Append("ERROR:\n");
    sb.Append(prefix).Append("if (dataPtr != NULL) {\n");
    sb.Append(prefix + TAB).Append("for (i = 0; i < dataCount; i++) {\n");
    sb.Append(prefix + TAB + TAB).Append("if (dataPtr[i] != NULL) {\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("OsalMemFree(dataPtr[i]);\n");
    sb.Append(prefix + TAB + TAB + TAB).Append("dataPtr[i] = NULL;\n");
    sb.Append(prefix + TAB + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).Append("OsalMemFree(dataPtr);\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).Append("*count = 0;\n");
    sb.Append(prefix).Append("return false;\n");
}

void HdiArrayTypeEmitter::EmitCppWriteMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    elementEmitter_->EmitCppWriteMethods(methods, prefix, methodPrefix, isDecl);
    if (!elementEmitter_->IsPod()) {
        return;
    }
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWritePodArray", methodPrefix.c_str());
    if (isDecl) {
        sb.Append("template<typename ElementType>\n");
        sb.AppendFormat("static bool %s(MessageParcel &parcel, const std::vector<ElementType> &data);\n",
            methodName.c_str());
    } else {
        sb.Append("\n").Append("template<typename ElementType>\n");
        sb.AppendFormat("static bool %s(MessageParcel &parcel, const std::vector<ElementType> &data)\n",
            methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (!parcel.WriteUint32(data.size())) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write data size\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (data.empty()) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("if (!parcel.WriteUnpadBuffer(");
        sb.Append("(const void*)data.data(), sizeof(ElementType) * data.size())) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write array\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiArrayTypeEmitter::EmitCppReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    elementEmitter_->EmitCppReadMethods(methods, prefix, methodPrefix, isDecl);
    if (!elementEmitter_->IsPod()) {
        return;
    }
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sReadPodArray", methodPrefix.c_str());
    if (isDecl) {
        sb.Append("template<typename ElementType>\n");
        sb.AppendFormat("static bool %s(MessageParcel &parcel, std::vector<ElementType> &data);\n",
            methodName.c_str());
    } else {
        sb.Append("\n").Append("template<typename ElementType>\n");
        sb.AppendFormat("static bool %s(MessageParcel &parcel, std::vector<ElementType> &data)\n", methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("data.clear();\n");
        sb.Append(prefix + TAB).Append("uint32_t size = 0;\n");
        sb.Append(prefix + TAB).Append("if (!parcel.ReadUint32(size)) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to read size\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");

        sb.Append(prefix + TAB).Append("if (size == 0) {\n");
        sb.Append(prefix + TAB + TAB).Append("return true;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("const ElementType *dataPtr = reinterpret_cast<const ElementType*>(");
        sb.Append("parcel.ReadUnpadBuffer(sizeof(ElementType) * size));\n");
        sb.Append(prefix + TAB).Append("if (dataPtr == nullptr) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGI(\"%{public}s: failed to read data\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n");

        sb.Append(prefix + TAB).Append("data.assign(dataPtr, dataPtr + size);\n");
        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

TypeKind HdiListTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_LIST;
}

std::string HdiListTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return StringHelper::Format("List<%s>", elementEmitter_->EmitJavaType(mode, true).c_str());
}

void HdiListTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s.size());\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat(
        "for (%s element : %s) {\n", elementEmitter_->EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str());
    elementEmitter_->EmitJavaWriteVar(parcelName, "element", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void HdiListTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.c_str());

    elementEmitter_->EmitJavaReadInnerVar(parcelName, "value", false, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("%s.add(value);\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiListTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s %s = new Array%s();\n", EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(),
        EmitJavaType(TypeMode::NO_MODE).c_str());
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.c_str());
    elementEmitter_->EmitJavaReadInnerVar(parcelName, "value", true, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("%s.add(value);\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS