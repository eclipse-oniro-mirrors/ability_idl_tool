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

#include "hdi_native_buffer_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiNativeBufferTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_NATIVE_BUFFER;
}

std::string HdiNativeBufferTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "BufferHandle*";
        case TypeMode::PARAM_IN:
            return "const BufferHandle*";
        case TypeMode::PARAM_OUT:
            return "BufferHandle**";
        case TypeMode::LOCAL_VAR:
            return "BufferHandle*";
        default:
            return "unknow type";
    }
}

std::string HdiNativeBufferTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "sptr<NativeBuffer>";
        case TypeMode::PARAM_IN:
            return "const sptr<NativeBuffer>&";
        case TypeMode::PARAM_OUT:
            return "sptr<NativeBuffer>&";
        case TypeMode::LOCAL_VAR:
            return "sptr<NativeBuffer>";
        default:
            return "unknow type";
    }
}

void HdiNativeBufferTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteNativeBufferHandle(%s, %s)) {\n", parcelName.c_str(),
        name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix);
    if (!isInnerType) {
        sb.Append('*');
    }
    sb.AppendFormat("%s = HdfSbufReadNativeBufferHandle(%s);\n", name.c_str(), replyParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = HdfSbufReadNativeBufferHandle(%s);\n", name.c_str(), dataParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    sb.Append(prefix);
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.AppendFormat("%s ", EmitCppType().c_str());
    }
    sb.AppendFormat("%s = %s.ReadStrongParcelable<NativeBuffer>();\n", name.c_str(), parcelName.c_str());
}

void HdiNativeBufferTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!HdfSbufWriteNativeBufferHandle(data, %s)) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("%s = HdfSbufReadNativeBufferHandle(data);\n", name.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    EmitFreeStatements(freeObjStatements, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiNativeBufferTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix);
    if (innerLevel > 0) {
        sb.AppendFormat("%s ", EmitCppType().c_str());
    }
    sb.AppendFormat("%s = %s.ReadStrongParcelable<NativeBuffer>();\n", name.c_str(), parcelName.c_str());
}

void HdiNativeBufferTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s != NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("FreeNativeBufferHandle(%s);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = NULL;\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS