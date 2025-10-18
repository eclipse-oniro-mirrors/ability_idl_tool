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

#include "sa_union_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaUnionTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_UNION;
}

std::string SaUnionTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return typeName_;
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const %s&", typeName_.c_str());
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("%s&", typeName_.c_str());
        default:
            return "unknown type";
    }
}

std::string SaUnionTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    std::string typeName = typeName_;
    size_t pos = typeName_.rfind("::");
    if (SaTypeEmitter::usingOn_ && (std::string::npos != pos)) {
        typeName = typeName_.substr(pos + strlen("::"));
    }
    sb.AppendFormat("union %s {\n", typeName.c_str());

    for (auto it : members_) {
        AutoPtr<SaTypeEmitter> member = std::get<1>(it);
        std::string memberName = std::get<0>(it);
        sb.Append(TAB).AppendFormat("%s %s;\n", member->EmitCppType().c_str(), memberName.c_str());
    }

    sb.Append("} __attribute__ ((aligned(8)));");
    return sb.ToString();
}

void SaUnionTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteUnpadBuffer(&%s, sizeof(%s))) {\n",
        parcelName.c_str(), name.c_str(), EmitCppType().c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaUnionTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("const %s *%sCp = reinterpret_cast<const %s *>(%sReadUnpadBuffer(sizeof(%s)));\n",
        EmitCppType().c_str(), name.c_str(), EmitCppType().c_str(), parcelName.c_str(), EmitCppType().c_str());
    sb.Append(prefix).AppendFormat("if (%sCp == nullptr) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Read [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("if (memcpy_s(&%s, sizeof(%s), %sCp, sizeof(%s)) != EOK) {\n", name.c_str(),
        EmitCppType().c_str(), name.c_str(), EmitCppType().c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"Memcpy [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS