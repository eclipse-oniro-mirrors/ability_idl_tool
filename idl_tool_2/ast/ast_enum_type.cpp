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

#include "ast/ast_enum_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void ASTEnumType::SetBaseType(const AutoPtr<ASTType> &baseType)
{
    if (baseType == nullptr) {
        return;
    }
    if (baseType->GetTypeKind() == TypeKind::TYPE_ENUM) {
        AutoPtr<ASTEnumType> parentEnumType = dynamic_cast<ASTEnumType *>(baseType.Get());
        std::vector<AutoPtr<ASTEnumValue>> parentMembers = parentEnumType->GetMembers();
        for (auto member : parentMembers) {
            members_.push_back(member);
        }
        parentType_= baseType;
        baseType_ = parentEnumType->GetBaseType();
    } else {
        baseType_ = baseType;
    }
}

bool ASTEnumType::AddMember(const AutoPtr<ASTEnumValue> &member)
{
    for (auto members : members_) {
        if (member->GetName() == members->GetName()) {
            return false;
        }
    }
    members_.push_back(member);
    return true;
}

AutoPtr<ASTType> ASTEnumType::GetBaseType()
{
    return baseType_;
}

std::string ASTEnumType::GetSignature()
{
    return " ";
}

bool ASTEnumType::IsEnumType()
{
    return true;
}

std::string ASTEnumType::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix).Append(attr_->Dump(prefix));
    if (baseType_ != nullptr) {
        if (parentType_ != nullptr) {
            sb.AppendFormat("enum %s : %s ", name_.c_str(), parentType_->ToString().c_str());
            sb.AppendFormat(" : %s {\n",  baseType_->ToString().c_str());
        } else {
            sb.AppendFormat("enum %s : %s {\n", name_.c_str(), baseType_->ToString().c_str());
        }
    } else {
        sb.AppendFormat("enum %s {\n", name_.c_str());
    }

    if (members_.size() > 0) {
        for (auto it : members_) {
            AutoPtr<ASTExpr> value = it->GetExprValue();
            if (value == nullptr) {
                sb.Append("  ").AppendFormat("%s,\n", it->GetName().c_str());
            } else {
                sb.Append("  ").AppendFormat("%s = %s,\n", it->GetName().c_str(), value->Dump("").c_str());
            }
        }
    }

    sb.Append(prefix).Append("};\n");

    return sb.ToString();
}

TypeKind ASTEnumType::GetTypeKind()
{
    return TypeKind::TYPE_ENUM;
}
} // namespace Idl
} // namespace OHOS
