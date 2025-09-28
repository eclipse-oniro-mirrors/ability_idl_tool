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

#include "sa_boolean_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaBooleanTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_BOOLEAN;
}

std::string SaBooleanTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return "bool";
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return "bool&";
        default:
            return "unknown type";
    }
}

std::string SaBooleanTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return "bool";
}

std::string SaBooleanTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "boolean";
}

void SaBooleanTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteInt32(%s ? 1 : 0)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).AppendFormat(", \"Write [%s] failed!\");\n",
            name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaBooleanTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = %sReadInt32() == 1 ? true : false;\n",
            EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %sReadInt32() == 1 ? true : false;\n", name.c_str(), parcelName.c_str());
    }
}

void SaBooleanTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s ? 1 : 0);\n", parcelName.c_str(), name.c_str());
}

void SaBooleanTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("let %s = %s.readInt() == 1 ? true : false;\n", name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS