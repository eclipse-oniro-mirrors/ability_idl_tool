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

#include "ast/ast_union_type.h"
#include "util/options.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void ASTUnionType::AddMember(const AutoPtr<ASTType> &typeName, std::string name)
{
    for (auto member : members_) {
        if (std::get<0>(member) == name) {
            return;
        }
    }
    members_.emplace_back(name, typeName);
}

std::string ASTUnionType::GetSignature()
{
    return " ";
}

bool ASTUnionType::IsUnionType()
{
    return true;
}

std::string ASTUnionType::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix).Append(attr_->Dump(prefix));
    sb.AppendFormat("union %s {\n", name_.c_str());
    if (!members_.empty()) {
        for (auto member : members_) {
            sb.Append(prefix + "  ");
            sb.AppendFormat("%s %s;\n", std::get<1>(member)->ToString().c_str(), std::get<0>(member).c_str());
        }
    }
    sb.Append(prefix).Append("};\n");
    return sb.ToString();
}

TypeKind ASTUnionType::GetTypeKind()
{
    return TypeKind::TYPE_UNION;
}
} // namespace Idl
} // namespace OHOS