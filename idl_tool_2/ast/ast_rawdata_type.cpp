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

#include "ast/ast_rawdata_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void ASTRawDataType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    ASTType::SetNamespace(nspace);
    if (namespace_ != nullptr) {
        namespace_->AddRawData(this);
    }
}

std::string ASTRawDataType::GetSignature()
{
    std::string fullName = namespace_ != nullptr ?
            namespace_->ToString() + name_ : name_;
    return "L" + StringHelper::Replace(fullName, '.', '/') + ";";
}

bool ASTRawDataType::IsRawDataType()
{
    return true;
}

TypeKind ASTRawDataType::GetTypeKind()
{
    return TypeKind::TYPE_RAWDATA;
}

std::string ASTRawDataType::Dump(const std::string &prefix)
{
    StringBuilder sb;

    sb.Append(prefix).Append("rawdata ");
    if (namespace_ != nullptr) {
        sb.Append(namespace_->ToString());
    }
    sb.Append(name_);
    sb.Append(";\n");

    return sb.ToString();
}

std::string ASTRawDataType::GetFullName() const
{
    if (namespace_ == nullptr) {
        return "";
    }
    return namespace_->ToString() + name_;
}
} // namespace Idl
} // namespace OHOS