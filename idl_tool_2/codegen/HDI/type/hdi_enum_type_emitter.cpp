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

#include "hdi_enum_type_emitter.h"

namespace OHOS {
namespace Idl {
void HdiEnumTypeEmitter::SetBaseTypeName(const std::string &name)
{
    baseTypeName_ = name;
}

TypeKind HdiEnumTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ENUM;
}

std::string HdiEnumTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("enum %s", name_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("enum %s", name_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("enum %s*", name_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("enum %s", name_.c_str());
        default:
            return "unknow type";
    }
}

std::string HdiEnumTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("%s", typeName_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s&", typeName_.c_str());
        default:
            return "unknow type";
    }
}

std::string HdiEnumTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // currently, Java does not support the enum type.
    return "/";
}

std::string HdiEnumTypeEmitter::EmitCTypeDecl() const
{
    StringBuilder sb;
    sb.AppendFormat("enum %s {\n", name_.c_str());

    for (auto it : members_) {
        if (it->GetValue().empty()) {
            sb.Append(TAB).AppendFormat("%s,\n", it->GetValueName().c_str());
        } else {
            sb.Append(TAB).AppendFormat("%s = %s,\n", it->GetValueName().c_str(), it->GetValue().c_str());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

std::string HdiEnumTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    if (baseTypeName_.empty()) {
        sb.AppendFormat("enum %s {\n", name_.c_str());
    } else {
        sb.AppendFormat("enum %s : %s {\n", name_.c_str(), baseTypeName_.c_str());
    }

    for (auto it : members_) {
        if (it->GetValue().empty()) {
            sb.Append(TAB).AppendFormat("%s,\n", it->GetValueName().c_str());
        } else {
            sb.Append(TAB).AppendFormat("%s = %s,\n", it->GetValueName().c_str(), it->GetValue().c_str());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

void HdiEnumTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint64(%s, (uint64_t)%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    std::string tmpVarName = "enumTmp";
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("uint64_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("if (!HdfSbufReadUint64(%s, &%s)) {\n", replyParcelName_.c_str(), tmpVarName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: invlid parameter %s\", __func__);\n",
        name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n\n");
    sb.Append(prefix + TAB).AppendFormat("*%s = (%s)%s;\n", name.c_str(), EmitCType().c_str(), tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    std::string tmpVarName = "enumTmp";
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("uint64_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("if (!HdfSbufReadUint64(%s, &%s)) {\n", dataParcelName_.c_str(), tmpVarName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("%s = (%s)%s;\n", name.c_str(), EmitCType().c_str(), tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint64(static_cast<uint64_t>(%s))) {\n", parcelName.c_str(),
        name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string tmpVarName = "enumTmp";
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s = static_cast<%s>(0);\n", EmitCppType().c_str(), name.c_str(),
            EmitCType().c_str());
    }
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("uint64_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!%s.ReadUint64(%s)) {\n", parcelName.c_str(), tmpVarName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n",
        name.c_str());
    sb.Append(prefix + TAB + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("%s = static_cast<%s>(%s);\n", name.c_str(), EmitCType().c_str(),
        tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint64(data, (uint64_t)%s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    std::string tmpVarName = "enumTmp";
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("uint64_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!HdfSbufReadUint64(data, &%s)) {\n", tmpVarName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("%s = (%s)%s;\n", name.c_str(), EmitCType().c_str(), tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (!%s.WriteUint64(static_cast<uint64_t>(%s))) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiEnumTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    std::string tmpVarName = "enumTmp";
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s = static_cast<%s>(0);\n", EmitCppType().c_str(), name.c_str(),
            EmitCType().c_str());
    }
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("uint64_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!%s.ReadUint64(%s)) {\n", parcelName.c_str(), tmpVarName.c_str());
    sb.Append(prefix + TAB + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n",
        name.c_str());
    sb.Append(prefix + TAB + TAB).Append("return false;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("%s = static_cast<%s>(%s);\n", name.c_str(), EmitCType().c_str(),
        tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS
