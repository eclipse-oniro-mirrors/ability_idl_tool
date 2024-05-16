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

#include "ast/ast_smq_type.h"

namespace OHOS {
namespace Idl {
std::string ASTSmqType::GetSignature()
{
    return " ";
}

bool ASTSmqType::IsSmqType()
{
    return true;
}

bool ASTSmqType::HasInnerType(TypeKind innerTypeKind) const
{
    if (innerType_ == nullptr) {
        return false;
    }

    if (innerType_->GetTypeKind() == innerTypeKind) {
        return true;
    }

    return innerType_->HasInnerType(innerTypeKind);
}

std::string ASTSmqType::ToString() const
{
    return StringHelper::Format("SharedMemQueue<%s>", innerType_->ToString().c_str());
}

TypeKind ASTSmqType::GetTypeKind()
{
    return TypeKind::TYPE_SMQ;
}

std::string ASTAshmemType::GetSignature()
{
    return " ";
}
bool ASTAshmemType::IsAshmemType()
{
    return true;
}

std::string ASTAshmemType::ToString() const
{
    return "Ashmem";
}

TypeKind ASTAshmemType::GetTypeKind()
{
    return TypeKind::TYPE_ASHMEM;
}
} // namespace Idl
} // namespace OHOS