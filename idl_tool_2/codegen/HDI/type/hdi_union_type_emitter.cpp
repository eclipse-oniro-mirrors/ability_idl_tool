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

#include "hdi_union_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiUnionTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_UNION;
}

std::string HdiUnionTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("union %s", name_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const union %s*", name_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("union %s*", name_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("union %s*", name_.c_str());
        default:
            return "unknown type";
    }
}

std::string HdiUnionTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("%s", typeName_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const %s&", typeName_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s&", typeName_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("%s", typeName_.c_str());
        default:
            return "unknow type";
    }
}

std::string HdiUnionTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // unsupported type
    return "/";
}

std::string HdiUnionTypeEmitter::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("union %s {\n", name_.c_str());

    for (auto it : members_) {
        AutoPtr<HdiTypeEmitter> member = std::get<1>(it);
        std::string memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCType().c_str(), memberName.c_str());
        if (member->GetTypeKind() == TypeKind::TYPE_ARRAY || member->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.Append(TAB).AppendFormat("uint32_t %sLen;\n", memberName.c_str());
        }
    }

    sb.Append("}  __attribute__ ((aligned(8)));");
    return sb.ToString();
}

std::string HdiUnionTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("union %s {\n", name_.c_str());

    for (auto it : members_) {
        AutoPtr<HdiTypeEmitter> member = std::get<1>(it);
        std::string memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCppType().c_str(), memberName.c_str());
    }

    sb.Append("}  __attribute__ ((aligned(8)));");
    return sb.ToString();
}

std::string HdiUnionTypeEmitter::EmitJavaTypeDecl() const
{
    StringBuilder sb;

    return sb.ToString();
}

void HdiUnionTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteBuffer(%s, (const void *)%s, sizeof(%s))) {\n",
            parcelName.c_str(), name.c_str(), EmitCType().c_str());
    } else {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUnpadBuffer(%s, (const uint8_t *)%s, sizeof(%s))) {\n",
            parcelName.c_str(), name.c_str(), EmitCType().c_str());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().c_str(), name.c_str());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufReadBuffer(%s, (const void **)&%s, &len)) {\n", replyParcelName_.c_str(), name.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.c_str(), EmitCType().c_str());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (%s *)HdfSbufReadUnpadBuffer(%s, sizeof(%s));\n",
            EmitCType().c_str(), name.c_str(), EmitCType().c_str(), replyParcelName_.c_str(), EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().c_str(), name.c_str());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufReadBuffer(%s, (const void **)&%s, &len)) {\n", dataParcelName_.c_str(), name.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.c_str(), EmitCType().c_str());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (%s *)HdfSbufReadUnpadBuffer(%s, sizeof(%s));\n",
            EmitCType().c_str(), name.c_str(), EmitCType().c_str(), dataParcelName_.c_str(), EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUnpadBuffer((const uint8_t *)&%s, sizeof(%s))) {\n",
        parcelName.c_str(), name.c_str(), EmitCppType().c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("const %s *%sCp = reinterpret_cast<const %s *>(%s.ReadUnpadBuffer(sizeof(%s)));\n",
        EmitCppType().c_str(), name.c_str(), EmitCppType().c_str(), parcelName.c_str(), EmitCppType().c_str());
    sb.Append(prefix).AppendFormat("if (%sCp == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (memcpy_s(&%s, sizeof(%s), %sCp, sizeof(%s)) != EOK) {\n", name.c_str(),
        EmitCppType().c_str(), name.c_str(), EmitCppType().c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to memcpy %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat(
            "if (!HdfSbufWriteBuffer(data, (const void *)&%s, sizeof(%s))) {\n", name.c_str(), EmitCType().c_str());
    } else {
        sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUnpadBuffer(data, (const uint8_t *)&%s, sizeof(%s))) {\n",
            name.c_str(), EmitCType().c_str());
    }
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    if (Options::GetInstance().DoGenerateKernelCode()) {
        sb.Append(prefix).AppendFormat("%s *%s = NULL;\n", EmitCType().c_str(), name.c_str());
        sb.Append(prefix).Append("uint32_t len = 0;\n");
        sb.Append(prefix).AppendFormat("if (!HdfSbufReadBuffer(data, (const void **)&%s, &len)) {\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
        sb.Append(prefix).Append("}\n\n");
        sb.Append(prefix).AppendFormat("if (%s == NULL || sizeof(%s) != len) {\n", name.c_str(), EmitCType().c_str());
    } else {
        sb.Append(prefix).AppendFormat("const %s *%s = (const %s *)HdfSbufReadUnpadBuffer(data, sizeof(%s));\n",
            EmitCType().c_str(), name.c_str(), EmitCType().c_str(), EmitCType().c_str());
        sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    }

    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUnpadBuffer((const void*)&%s, sizeof(%s))) {\n", parcelName.c_str(),
        name.c_str(), EmitCppType().c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("const %s *%s = reinterpret_cast<const %s*>(%s.ReadUnpadBuffer(sizeof(%s)));\n",
        EmitCppType().c_str(), name.c_str(), EmitCppType().c_str(), parcelName.c_str(), EmitCppType().c_str());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUnionTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    if (ownership) {
        sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("OsalMemFree(%s);\n", name.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", name.c_str());
        sb.Append(prefix).Append("}\n");
    }
}
} // namespace Idl
} // namespace OHOS