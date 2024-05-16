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

#include "hdi_uint_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiUintTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_UINT;
}

std::string HdiUintTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint32_t";
        case TypeMode::PARAM_IN:
            return "uint32_t";
        case TypeMode::PARAM_OUT:
            return "uint32_t*";
        case TypeMode::LOCAL_VAR:
            return "uint32_t";
        default:
            return "unknow type";
    }
}

std::string HdiUintTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "uint32_t";
        case TypeMode::PARAM_IN:
            return "uint32_t";
        case TypeMode::PARAM_OUT:
            return "uint32_t&";
        case TypeMode::LOCAL_VAR:
            return "uint32_t";
        default:
            return "unknow type";
    }
}

std::string HdiUintTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    // unsupported type
    return "/";
}

void HdiUintTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(%s, %s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, %s)) {\n", replyParcelName_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(%s, %s)) {\n", dataParcelName_.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s = 0;\n", EmitCppType().c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteUint32(data, %s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufReadUint32(data, &%s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiUintTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s = 0;\n", EmitCppType().c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS