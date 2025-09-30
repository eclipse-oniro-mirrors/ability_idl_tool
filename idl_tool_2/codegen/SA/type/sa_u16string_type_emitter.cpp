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

#include "sa_u16string_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaU16stringTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_U16STRING;
}

std::string SaU16stringTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return "std::u16string";
        case TypeMode::PARAM_IN:
            return "const std::u16string&";
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return "std::u16string&";
        default:
            return "unknown type";
    }
}

void SaU16stringTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteString16(%s)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaU16stringTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = %sReadString16();\n",
            EmitCppType(TypeMode::LOCAL_VAR).c_str(),
            name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %sReadString16();\n", name.c_str(), parcelName.c_str());
    }
}

} // namespace Idl
} // namespace OHOS