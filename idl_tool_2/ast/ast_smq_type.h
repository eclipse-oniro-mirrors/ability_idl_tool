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

#ifndef OHOS_IDL_AST_SMQ_H
#define OHOS_IDL_AST_SMQ_H

#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
class ASTSmqType : public ASTType {
public:
    ASTSmqType() : ASTType(false), innerType_() {}

    inline void SetInnerType(const AutoPtr<ASTType> &innerType)
    {
        innerType_ = innerType;
    }

    inline AutoPtr<ASTType> GetInnerType()
    {
        return innerType_;
    }

    std::string GetSignature() override;

    bool IsSmqType() override;

    bool HasInnerType(TypeKind innerTypeKind) const override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;

private:
    AutoPtr<ASTType> innerType_;
};

class ASTAshmemType : public ASTType {
public:
    ASTAshmemType() : ASTType(false) {}

    std::string GetSignature() override;

    bool IsAshmemType() override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_AST_SMQ_H