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

#include "sa_string_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaStringTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_STRING;
}

std::string SaStringTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "std::string";
        case TypeMode::PARAM_IN:
        case TypeMode::PARAM_INOUT:
            return "const std::string&";
        case TypeMode::PARAM_OUT:
            return "std::string&";
        case TypeMode::LOCAL_VAR:
            return "std::string";
        default:
            return "unknow type";
    }
}

std::string SaStringTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return pt ? "str" : "String";
}

std::string SaStringTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "string";
}

void SaStringTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteString16(Str8ToStr16(%s))) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).AppendFormat("HiLog::Error(LABEL, \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaStringTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = Str16ToStr8(%sReadString16());\n",
            EmitCppType(TypeMode::LOCAL_VAR).c_str(),
            name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = Str16ToStr8(%sReadString16());\n", name.c_str(), parcelName.c_str());
    }
}

void SaStringTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.writeString(%s);\n", parcelName.c_str(), name.c_str());
}

void SaStringTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("let %s = %s.readString();\n", name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS