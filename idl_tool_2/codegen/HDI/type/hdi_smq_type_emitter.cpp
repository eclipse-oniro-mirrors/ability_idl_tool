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

#include "hdi_smq_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiSmqTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_SMQ;
}

std::string HdiSmqTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("std::shared_ptr<SharedMemQueue<%s>>",
                innerTypeEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format(
                "const std::shared_ptr<SharedMemQueue<%s>>&", innerTypeEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("std::shared_ptr<SharedMemQueue<%s>>&",
                innerTypeEmitter_->EmitCppType().c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("std::shared_ptr<SharedMemQueue<%s>>",
                innerTypeEmitter_->EmitCppType().c_str());
        default:
            return "unknow type";
    }
}

void HdiSmqTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (%s == nullptr || !%s->IsGood() || %s->GetMeta() == nullptr || ", name.c_str(), name.c_str(), name.c_str());
    sb.AppendFormat("!%s->GetMeta()->Marshalling(%s)) {\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiSmqTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    std::string metaVarName = StringHelper::Format("%sMeta_", name.c_str());
    sb.Append(prefix).AppendFormat(
        "std::shared_ptr<SharedMemQueueMeta<%s>> %s = ", innerTypeEmitter_->EmitCppType().c_str(), metaVarName.c_str());
    sb.AppendFormat("SharedMemQueueMeta<%s>::UnMarshalling(%s);\n", innerTypeEmitter_->EmitCppType().c_str(),
        parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", metaVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: SharedMemQueueMeta is nullptr\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");

    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s = ", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = ", name.c_str());
    }

    sb.AppendFormat(
        "std::make_shared<SharedMemQueue<%s>>(*%s);\n", innerTypeEmitter_->EmitCppType().c_str(), metaVarName.c_str());
}

void HdiSmqTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (%s == nullptr || !%s->IsGood() || %s->GetMeta() == nullptr || ", name.c_str(), name.c_str(), name.c_str());
    sb.AppendFormat("!%s->GetMeta()->Marshalling(%s)) {\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiSmqTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    size_t index = name.find('.');
    std::string memberName = (index == std::string::npos) ? name : StringHelper::SubStr(name, index + 1);
    std::string metaVarName = StringHelper::Format("%sMeta_", memberName.c_str());

    sb.Append(prefix).AppendFormat(
        "std::shared_ptr<SharedMemQueueMeta<%s>> %s = ", innerTypeEmitter_->EmitCppType().c_str(), metaVarName.c_str());
    sb.AppendFormat(
        "SharedMemQueueMeta<%s>::UnMarshalling(%s);\n", innerTypeEmitter_->EmitCppType().c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", metaVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: SharedMemQueueMeta is nullptr\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");

    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s = ", EmitCppType(TypeMode::LOCAL_VAR).c_str(), memberName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = ", name.c_str());
    }

    sb.AppendFormat(
        "std::make_shared<SharedMemQueue<%s>>(*%s);\n", innerTypeEmitter_->EmitCppType().c_str(), metaVarName.c_str());
}

TypeKind HdiAshmemTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ASHMEM;
}

std::string HdiAshmemTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("sptr<Ashmem>");
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const sptr<Ashmem>&");
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("sptr<Ashmem>&");
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("sptr<Ashmem>");
        default:
            return "unknow type";
    }
}

void HdiAshmemTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (%s == nullptr || !%s.WriteAshmem(%s)) {\n", name.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiAshmemTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadAshmem();\n", EmitCppType().c_str(), name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadAshmem();\n", name.c_str(), parcelName.c_str());
    }

    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read ashmem object\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiAshmemTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat(
        "if (%s == nullptr || !%s.WriteAshmem(%s)) {\n", name.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: failed to write %s\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiAshmemTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %s.ReadAshmem();\n", EmitCppType().c_str(), name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %s.ReadAshmem();\n", name.c_str(), parcelName.c_str());
    }

    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read ashmem object\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS