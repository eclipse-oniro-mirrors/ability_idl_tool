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

#include "sa_map_type_emitter.h"

#include <sstream>

namespace OHOS {
namespace Idl {
TypeKind SaMapTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_MAP;
}

std::string SaMapTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format(
                "std::unordered_map<%s, %s>", keyEmitter_->EmitCppType().c_str(), valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const std::unordered_map<%s, %s>&", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_OUT:
        case TypeMode::PARAM_INOUT:
            return StringHelper::Format("std::unordered_map<%s, %s>&", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("std::unordered_map<%s, %s>", keyEmitter_->EmitCppType().c_str(),
                valueEmitter_->EmitCppType().c_str());
        default:
            return "unknown type";
    }
}

std::string SaMapTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return StringHelper::Format("HashMap<%s, %s>", keyEmitter_->EmitRustType(false, mode).c_str(),
        valueEmitter_->EmitRustType(false, mode).c_str());
}

std::string SaMapTypeEmitter::EmitTsType(TypeMode mode) const
{
    if ((keyEmitter_->EmitTsType(mode) == "unknown type") || (valueEmitter_->EmitTsType(mode) == "unknown type")) {
        return "unknown type";
    }
    return StringHelper::Format("Map<%s, %s>", keyEmitter_->EmitTsType(mode).c_str(),
        valueEmitter_->EmitTsType(mode).c_str());
}

void SaMapTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
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

void SaMapTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
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
    sb.Append(prefix).AppendFormat("for (int32_t i%d = 0; i%d < %sSize; ++i%d) {\n", circleCount_,
        circleCount_, useName.c_str(), circleCount_);
    keyEmitter_->EmitCppReadVar(parcelName, keyStr.c_str(), sb, prefix + TAB);
    valueEmitter_->EmitCppReadVar(parcelName, valueStr.c_str(), sb, prefix + TAB);
    if (valueEmitter_->GetTypeKind() == TypeKind::TYPE_SEQUENCEABLE) {
        valueStr = "*" + valueStr;
    }
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.c_str(), keyStr.c_str(), valueStr.c_str());
    sb.Append(prefix).Append("}\n");
}

void SaMapTypeEmitter::EmitRustReadVar(const std::string &result, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("let mut %s = HashMap::new();\n", name.c_str());
    sb.Append(prefix).AppendFormat("let len = %s.read()?;\n", result.c_str());
    sb.Append(prefix).Append("for i in 0..len {\n");
    StringBuilder k;
    StringBuilder v;
    k.Append(name).Append("k");
    v.Append(name).Append("v");
    keyEmitter_->EmitRustReadVar(result, k.ToString(), sb, prefix + "    ");
    valueEmitter_->EmitRustReadVar(result, v.ToString(), sb, prefix + "    ");
    sb.Append(prefix + "    ").AppendFormat("%s.insert(%s, %s);\n", name.c_str(), k.ToString().c_str(),
        v.ToString().c_str());
    sb.Append(prefix).Append("}\n");
}

void SaMapTypeEmitter::EmitRustWriteVar(const std::string &result, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.write(&(%s.len() as u32))?;\n", result.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("for (key, value) in %s.iter() {\n", name.c_str());
    keyEmitter_->EmitRustWriteVar(result.c_str(), "key", sb, prefix + "    ");
    valueEmitter_->EmitRustWriteVar(result.c_str(), "value", sb, prefix + "    ");
    sb.Append(prefix).Append("}\n");
}

void SaMapTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s.size);\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("for (let [key, value] of %s) {\n", name.c_str());
    keyEmitter_->EmitTsWriteVar(parcelName, "key", sb, prefix + TAB);
    valueEmitter_->EmitTsWriteVar(parcelName, "value", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void SaMapTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    std::string useName = name;
    size_t dotPos = useName.find('.');
    if (dotPos != std::string::npos) {
        useName = useName.substr(dotPos + 1);
    }
    sb.Append(prefix).AppendFormat("let %s = new Map();\n", name.c_str());
    sb.Append(prefix).AppendFormat("let %sSize = %s.readInt();\n", useName.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("for (let i = 0; i < %sSize; ++i) {\n", useName.c_str());
    keyEmitter_->EmitTsReadVar(parcelName, "key", sb, prefix + TAB, TypeMode::PARAM_IN);
    valueEmitter_->EmitTsReadVar(parcelName, "value", sb, prefix + TAB, TypeMode::PARAM_IN);
    sb.Append(prefix).Append(TAB).AppendFormat("%s.set(key, value);\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS