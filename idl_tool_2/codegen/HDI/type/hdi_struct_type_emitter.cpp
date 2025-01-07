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

#include "hdi_struct_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiStructTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_STRUCT;
}

std::string HdiStructTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("struct %s", name_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const struct %s*", name_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("struct %s*", name_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("struct %s*", name_.c_str());
        default:
            return "unknown type";
    }
}

std::string HdiStructTypeEmitter::EmitCppType(TypeMode mode) const
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

std::string HdiStructTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // currently, Java does not support the struct type.
    return "/";
}

std::string HdiStructTypeEmitter::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("struct %s {\n", name_.c_str());

    for (const auto& it : members_) {
        AutoPtr<HdiTypeEmitter> member = std::get<1>(it);
        std::string memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCType().c_str(), memberName.c_str());
        if (member->GetTypeKind() == TypeKind::TYPE_ARRAY || member->GetTypeKind() == TypeKind::TYPE_LIST) {
            sb.Append(TAB).AppendFormat("uint32_t %sLen;\n", memberName.c_str());
        }
    }

    sb.Append('}');
    if (IsPod()) {
        sb.Append(" __attribute__ ((aligned(8)))");
    }
    sb.Append(';');

    return sb.ToString();
}

std::string HdiStructTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("struct %s {\n", name_.c_str());

    for (const auto& it : members_) {
        AutoPtr<HdiTypeEmitter> member = std::get<1>(it);
        std::string memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCppType().c_str(), memberName.c_str());
    }

    sb.Append('}');
    if (IsPod()) {
        sb.Append(" __attribute__ ((aligned(8)))");
    }
    sb.Append(';');
    return sb.ToString();
}

void HdiStructTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", name_.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.c_str(), replyParcelName_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.c_str(), dataParcelName_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).AppendFormat("}\n");
}

void HdiStructTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", EmitCppType().c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockUnmarshalling(%s, %s)) {\n", name_.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockMarshalling(data, &%s)) {\n", name_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockUnmarshalling(data, %s)) {\n", name_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%sBlockMarshalling(%s, %s)) {\n", name_.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%sBlockUnmarshalling(data, %s)) {\n", EmitCppType().c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiStructTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    std::string varName = name;
    if (ownership) {
        sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", varName.c_str());
        sb.Append(prefix + TAB).AppendFormat("%sFree(%s, true);\n", name_.c_str(), varName.c_str());
        sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", varName.c_str());
        sb.Append(prefix).Append("}\n");
    } else {
        sb.Append(prefix).AppendFormat("%sFree(&%s, false);\n", name_.c_str(), name.c_str());
    }
}
} // namespace Idl
} // namespace OHOS