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

#include "ast/ast_map_type.h"

namespace OHOS {
namespace Idl {
std::string ASTMapType::GetSignature()
{
    return StringHelper::Format("LMap[%s%s];", keyType_->GetSignature().c_str(), valueType_->GetSignature().c_str());
}

bool ASTMapType::IsMapType()
{
    return true;
}

bool ASTMapType::HasInnerType(TypeKind innerTypeKind) const
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

std::string ASTMapType::ToString() const
{
    return StringHelper::Format("Map<%s, %s>", keyType_->ToString().c_str(), valueType_->ToString().c_str());
}

TypeKind ASTMapType::GetTypeKind()
{
    return TypeKind::TYPE_MAP;
}
} // namespace Idl
} // namespace OHOS