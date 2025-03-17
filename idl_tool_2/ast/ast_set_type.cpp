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

#include "ast/ast_set_type.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
std::string ASTSetType::GetSignature()
{
    return StringHelper::Format("LSet<%s>;", elementType_->GetSignature().c_str());
}

bool ASTSetType::IsSetType()
{
    return true;
}

bool ASTSetType::HasInnerType(TypeKind innerTypeKind) const
{
    if (elementType_ == nullptr) {
        return false;
    }

    if (elementType_->GetTypeKind() == innerTypeKind) {
        return true;
    }
    return elementType_->HasInnerType(innerTypeKind);
}

std::string ASTSetType::ToString() const
{
    if (elementType_ == nullptr) {
        return StringHelper::Format("Set<>");
    }
    return StringHelper::Format("Set<%s>", elementType_->ToString().c_str());
}

TypeKind ASTSetType::GetTypeKind()
{
    return TypeKind::TYPE_SET;
}

} // namespace Idl
} // namespace OHOS