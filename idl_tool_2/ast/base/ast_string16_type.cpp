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

#include "ast_string16_type.h"

namespace OHOS {
namespace Idl {
std::string ASTString16Type::GetSignature()
{
    return "Y";
}

bool ASTString16Type::IsStringType()
{
    return true;
}

std::string ASTString16Type::ToString() const
{
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        return "String";
    }
    return "std::string";
}

TypeKind ASTString16Type::GetTypeKind()
{
    return TypeKind::TYPE_STRING16;
}
} // namespace Idl
} // namespace OHOS