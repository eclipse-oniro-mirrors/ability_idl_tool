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

#include "ast/ast_ptr_type.h"
#include "util/options.h"

namespace OHOS {
namespace Idl {
std::string ASTPtrType::GetSignature()
{
    if (elementType_ == nullptr) {
        return "";
    }
    return StringHelper::Format("%s<%s>", ptrTypeName_.c_str(), elementType_->GetSignature().c_str());
}

std::string ASTPtrType::ToString() const
{
    if (elementType_ == nullptr) {
        return StringHelper::Format("%s<>", ptrTypeName_.c_str());
    }
    return StringHelper::Format("%s<%s>", ptrTypeName_.c_str(), elementType_->ToString().c_str());
}

TypeKind ASTPtrType::GetTypeKind()
{
    return typeKind_;
}
} // namespace Idl
} // namespace OHOS