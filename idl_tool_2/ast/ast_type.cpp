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

#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
void ASTType::SetName(const std::string &name)
{
    name_ = name;
}

std::string ASTType::GetName()
{
    return name_;
}

void ASTType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    namespace_ = nspace;
}

AutoPtr<ASTNamespace> ASTType::GetNamespace()
{
    return namespace_;
}

bool ASTType::IsVoidType()
{
    return false;
}

bool ASTType::IsBooleanType()
{
    return false;
}

bool ASTType::IsByteType()
{
    return false;
}

bool ASTType::IsShortType()
{
    return false;
}

bool ASTType::IsIntegerType()
{
    return false;
}

bool ASTType::IsLongType()
{
    return false;
}

bool ASTType::IsCharType()
{
    return false;
}

bool ASTType::IsUcharType()
{
    return false;
}

bool ASTType::IsUshortType()
{
    return false;
}

bool ASTType::IsUintType()
{
    return false;
}

bool ASTType::IsUlongType()
{
    return false;
}

bool ASTType::IsFloatType()
{
    return false;
}

bool ASTType::IsDoubleType()
{
    return false;
}

bool ASTType::IsStringType()
{
    return false;
}

bool ASTType::IsListType()
{
    return false;
}

bool ASTType::IsMapType()
{
    return false;
}

bool ASTType::IsEnumType()
{
    return false;
}

bool ASTType::IsStructType()
{
    return false;
}

bool ASTType::IsUnionType()
{
    return false;
}

bool ASTType::IsInterfaceType()
{
    return false;
}

bool ASTType::IsSequenceableType()
{
    return false;
}

bool ASTType::IsArrayType()
{
    return false;
}

bool ASTType::IsFdType()
{
    return false;
}

bool ASTType::IsSmqType()
{
    return false;
}

bool ASTType::IsAshmemType()
{
    return false;
}

bool ASTType::IsNativeBufferType()
{
    return false;
}

bool ASTType::IsPointerType()
{
    return false;
}

bool ASTType::IsPod() const
{
    return isPod_;
}

bool ASTType::IsTypeKind(TypeKind typekind) const
{
    return typekind == typeKind_;
}

bool ASTType::HasInnerType(TypeKind innerType) const
{
    return false;
}

std::string ASTType::ToShortString()
{
    return name_;
}

std::string ASTType::ToString() const
{
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        return name_;
    }
    return (namespace_ == nullptr) ? name_ : (namespace_->ToString() + name_);
}

TypeKind ASTType::GetTypeKind()
{
    return TypeKind::TYPE_UNKNOWN;
}
} // namespace Idl
} // namespace OHOS