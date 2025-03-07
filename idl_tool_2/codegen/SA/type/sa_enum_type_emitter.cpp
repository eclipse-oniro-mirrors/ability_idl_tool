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

#include "sa_enum_type_emitter.h"

namespace OHOS {
namespace Idl {
void SaEnumTypeEmitter::SetBaseTypeName(const std::string &name)
{
    baseTypeName_ = name;
}

TypeKind SaEnumTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ENUM;
}

std::string SaEnumTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return typeName_;
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s&", typeName_.c_str());
        default:
            return "unknown type";
    }
}

std::string SaEnumTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    if (baseTypeName_.empty()) {
        sb.AppendFormat("enum class %s {\n", typeName_.c_str());
    } else {
        sb.AppendFormat("enum class %s : %s {\n", typeName_.c_str(), baseTypeName_.c_str());
    }

    for (auto it : members_) {
        if (it->GetValue().empty()) {
            sb.Append(TAB).AppendFormat("%s,\n", it->GetValueName().c_str());
        } else {
            sb.Append(TAB).AppendFormat("%s = %s,\n", it->GetValueName().c_str(), it->GetValue().c_str());
        }
    }

    sb.Append("};");
    return sb.ToString();
}

void SaEnumTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteInt32(static_cast<int32_t>(%s))) {\n", parcelName.c_str(),
        name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).AppendFormat("HiLog::Error(LABEL, \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaEnumTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    std::string tmpVarName = "enumTmp";
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(),
            name.c_str(), typeName_.c_str());
    }
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + TAB).AppendFormat("int32_t %s = 0;\n", tmpVarName.c_str());
    sb.Append(prefix + TAB).AppendFormat("if (!%sReadInt32(%s)) {\n", parcelName.c_str(), tmpVarName.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(TAB).AppendFormat("HiLog::Error(LABEL, \"Read [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix + TAB).Append("}\n");
    sb.Append(prefix + TAB).AppendFormat("%s = static_cast<%s>(%s);\n", name.c_str(), EmitCppType().c_str(),
        tmpVarName.c_str());
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS
