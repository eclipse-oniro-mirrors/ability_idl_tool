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

#include "ast/ast_orderedmap_type.h"

namespace OHOS {
namespace Idl {
std::string ASTOrderedMapType::GetSignature()
{
    if (keyType_ == nullptr || valueType_ == nullptr) {
        return StringHelper::Format("LMap[];");
    }
    return StringHelper::Format("LMap[%s%s];", keyType_->GetSignature().c_str(), valueType_->GetSignature().c_str());
}

bool ASTOrderedMapType::IsOrderedMapType()
{
    return true;
}

bool ASTOrderedMapType::HasInnerType(TypeKind innerTypeKind) const
{
    if (keyType_ != nullptr) {
        if (keyType_->GetTypeKind() == innerTypeKind) {
            return true;
        }

        if (keyType_->HasInnerType(innerTypeKind)) {
            return true;
        }
    }

    if (valueType_ != nullptr) {
        if (valueType_->GetTypeKind() == innerTypeKind) {
            return true;
        }

        if (valueType_->HasInnerType(innerTypeKind)) {
            return true;
        }
    }

    return false;
}

std::string ASTOrderedMapType::ToString() const
{
    return StringHelper::Format("OrderedMap<%s, %s>", keyType_->ToString().c_str(), valueType_->ToString().c_str());
}

TypeKind ASTOrderedMapType::GetTypeKind()
{
    return TypeKind::TYPE_ORDEREDMAP;
}
} // namespace Idl
} // namespace OHOS