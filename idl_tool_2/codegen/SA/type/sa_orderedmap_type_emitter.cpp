/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "sa_orderedmap_type_emitter.h"

#include <sstream>

namespace OHOS {
namespace Idl {
TypeKind SaOrderedMapTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ORDEREDMAP;
}

std::string SaOrderedMapTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("std::map<%s, %s>", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const std::map<%s, %s>&", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_OUT:
        case TypeMode::PARAM_INOUT:
            return StringHelper::Format("std::map<%s, %s>&", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        default:
            return "unknown type";
    }
}

void SaOrderedMapTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s.size() > static_cast<size_t>(MAP_MAX_SIZE)) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).AppendFormat(
            "HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n");
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append("\n");
    sb.Append(prefix).AppendFormat("%sWriteInt32(%s.size());\n", parcelName.c_str(), name.c_str());

    circleCount_++;
    std::stringstream circleCountStr;
    circleCountStr << circleCount_;
    std::string itStr = "it" + circleCountStr.str();
    sb.Append(prefix).AppendFormat("for (auto %s = %s.begin(); %s != %s.end(); ++%s) {\n", itStr.c_str(),
        name.c_str(), itStr.c_str(), name.c_str(), itStr.c_str());
    keyEmitter_->EmitCppWriteVar(parcelName, "(" + itStr + "->first)", sb, prefix + TAB);
    valueEmitter_->EmitCppWriteVar(parcelName, "(" + itStr + "->second)", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void SaOrderedMapTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    std::string useName = name;
    size_t dotPos = useName.find('.');
    if (dotPos != std::string::npos) {
        useName = useName.substr(dotPos + 1);
    }

    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    }

    circleCount_++;
    std::stringstream circleCountStr;
    circleCountStr << circleCount_;
    std::string keyStr = "key" + circleCountStr.str();
    std::string valueStr = "value" + circleCountStr.str();
    sb.Append(prefix).AppendFormat("int32_t %sSize = %sReadInt32();\n", useName.c_str(), parcelName.c_str());

    sb.Append(prefix).AppendFormat("if (%sSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n", useName.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).AppendFormat(
            "HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n");
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append("\n");

    sb.Append(prefix).AppendFormat("for (int32_t i%d = 0; i%d < %sSize; ++i%d) {\n", circleCount_,
        circleCount_, useName.c_str(), circleCount_);
    keyEmitter_->EmitCppReadVar(parcelName, keyStr, sb, prefix + TAB);
    valueEmitter_->EmitCppReadVar(parcelName, valueStr, sb, prefix + TAB);
    if (keyEmitter_->GetTypeKind() == TypeKind::TYPE_SEQUENCEABLE &&
        keyEmitter_->GetTypeName() != "IRemoteObject") {
        keyStr = "*" + keyStr;
    }
    if (valueEmitter_->GetTypeKind() == TypeKind::TYPE_SEQUENCEABLE &&
        valueEmitter_->GetTypeName() != "IRemoteObject") {
        valueStr = "*" + valueStr;
    }
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.c_str(), keyStr.c_str(), valueStr.c_str());
    sb.Append(prefix).Append("}\n");
}

} // namespace Idl
} // namespace OHOS