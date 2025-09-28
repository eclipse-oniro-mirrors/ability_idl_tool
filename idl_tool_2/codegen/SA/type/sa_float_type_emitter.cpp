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

#include "sa_float_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaFloatTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_FLOAT;
}

std::string SaFloatTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return "float";
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return "float&";
        default:
            return "unknown type";
    }
}

std::string SaFloatTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return "f32";
}

std::string SaFloatTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "number";
}

void SaFloatTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteFloat(%s)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaFloatTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat(
            "%s %s = %sReadFloat();\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %sReadFloat();\n", name.c_str(), parcelName.c_str());
    }
}

void SaFloatTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeFloat(%s);\n", parcelName.c_str(), name.c_str());
}

void SaFloatTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("let %s = %s.readFloat();\n", name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS