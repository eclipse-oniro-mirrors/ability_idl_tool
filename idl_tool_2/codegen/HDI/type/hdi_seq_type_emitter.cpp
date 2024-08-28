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

#include "hdi_seq_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiSeqTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_SEQUENCEABLE;
}

std::string HdiSeqTypeEmitter::EmitCType(TypeMode mode) const
{
    // c language has no Sequenceable type
    return "/";
}

std::string HdiSeqTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("sptr<%s>", typeName_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const sptr<%s>&", typeName_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("sptr<%s>&", typeName_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("sptr<%s>", typeName_.c_str());
        default:
            return "unknown type";
    }
}

std::string HdiSeqTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return name_;
}

void HdiSeqTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiSeqTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("sptr<%s> %s = %s.ReadStrongParcelable<%s>();\n", name_.c_str(), name.c_str(),
            parcelName.c_str(), name_.c_str());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = %s.ReadStrongParcelable<%s>();\n", name.c_str(), parcelName.c_str(), name_.c_str());
    }
}

void HdiSeqTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteStrongParcelable(%s)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiSeqTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s = %s.ReadStrongParcelable<%s>();\n", EmitCppType().c_str(), name.c_str(),
            parcelName.c_str(), name_.c_str());
    } else {
        sb.Append(prefix).AppendFormat(
            "%s = %s.ReadStrongParcelable<%s>();\n", name.c_str(), parcelName.c_str(), name_.c_str());
    }
}

void HdiSeqTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    if (EmitJavaType(TypeMode::NO_MODE) == "IRemoteObject") {
        sb.Append(prefix).AppendFormat("%s.writeRemoteObject(%s);\n", parcelName.c_str(), name.c_str());
        return;
    }
    sb.Append(prefix).AppendFormat("%s.writeSequenceable(%s);\n", parcelName.c_str(), name.c_str());
}

void HdiSeqTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    if (EmitJavaType(TypeMode::NO_MODE) == "IRemoteObject") {
        sb.Append(prefix).AppendFormat("%s = %s.readRemoteObject();\n", name.c_str(), parcelName.c_str());
        return;
    }
    sb.Append(prefix).AppendFormat("%s.readSequenceable(%s);\n", parcelName.c_str(), name.c_str());
}

void HdiSeqTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    if (!isInner && EmitJavaType(TypeMode::NO_MODE) == "IRemoteObject") {
        sb.Append(prefix).AppendFormat("IRemoteObject %s = %s.readRemoteObject();\n", name.c_str(), parcelName.c_str());
        return;
    }
    if (!isInner) {
        sb.Append(prefix).AppendFormat("%s %s = new %s();\n", EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(),
            EmitJavaType(TypeMode::NO_MODE).c_str());
    }
    sb.Append(prefix).AppendFormat("%s.readSequenceable(%s);\n", parcelName.c_str(), name.c_str());
}
} // namespace Idl
} // namespace OHOS