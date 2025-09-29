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

#include "sa_rawdata_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaRawDataTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_RAWDATA;
}

std::string SaRawDataTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const %s&", typeName_.c_str());
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s&", typeName_.c_str());
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return typeName_;
        default:
            return "unknown type";
    }
}

void SaRawDataTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteUint32(%s.size)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");

    sb.Append(prefix).AppendFormat("if (!%sWriteRawData(%s.data, %s.size)) {\n",
        parcelName.c_str(), name.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            Append(", \"Write [%s RawData] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaRawDataTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat(
        "if (!%sReadUint32(%s.size)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).AppendFormat(", \"Read [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");

    sb.Append(prefix).AppendFormat(
        "auto read%s = %sReadRawData(%s.size);\n", name.c_str(), parcelName.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("if (read%s == nullptr) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Read [%s RawData] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");

    sb.Append(prefix).AppendFormat("ErrCode %soutError = %s.RawDataCpy(read%s);\n",
        name.c_str(), name.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("if (%soutError) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"RawDataCpy [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).AppendFormat("return %soutError;\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS