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

#include "sa_byte_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaByteTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_BYTE;
}

std::string SaByteTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return "int8_t";
        case TypeMode::PARAM_OUT:
        case TypeMode::PARAM_INOUT:
            return "int8_t&";
        default:
            return "unknown type";
    }
}

std::string SaByteTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return "i8";
}

std::string SaByteTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "number";
}

void SaByteTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteInt32(%s)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaByteTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = (%s)%sReadInt32();\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(),
            name.c_str(), EmitCppType(TypeMode::LOCAL_VAR).c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = (%s)%sReadInt32();\n", name.c_str(),
            EmitCppType(TypeMode::LOCAL_VAR).c_str(), parcelName.c_str());
    }
}

void SaByteTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s);\n", parcelName.c_str(), name.c_str());
}

void SaByteTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("let %s = %s.readInt();\n", name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS