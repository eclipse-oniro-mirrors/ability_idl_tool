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

#include "ast_u16string_type.h"

namespace OHOS {
namespace Idl {
std::string ASTU16stringType::GetSignature()
{
    return "U";
}

bool ASTU16stringType::IsU16stringType()
{
    return true;
}

std::string ASTU16stringType::ToString() const
{
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        return "U16string";
    }
    return "std::u16string";
}

TypeKind ASTU16stringType::GetTypeKind()
{
    return TypeKind::TYPE_U16STRING;
}
} // namespace Idl
} // namespace OHOS