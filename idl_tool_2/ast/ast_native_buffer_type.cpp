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

#include "ast/ast_native_buffer_type.h"

namespace OHOS {
namespace Idl {
std::string ASTNativeBufferType::GetSignature()
{
    return " ";
}

bool ASTNativeBufferType::IsNativeBufferType()
{
    return true;
}

std::string ASTNativeBufferType::ToString() const
{
    return "NativeBuffer";
}

TypeKind ASTNativeBufferType::GetTypeKind()
{
    return TypeKind::TYPE_NATIVE_BUFFER;
}
} // namespace Idl
} // namespace OHOS