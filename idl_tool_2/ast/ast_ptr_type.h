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

#ifndef OHOS_IDL_ASTPTRTYPE_H
#define OHOS_IDL_ASTPTRTYPE_H

#include "ast/ast_type.h"
#include "lexer/token.h"

namespace OHOS {
namespace Idl {
class ASTPtrType : public ASTType {
public:
    ASTPtrType(TokenType type) : ASTType(false), elementType_()
    {
        if (type == TokenType::SHAREDPTR) {
            typeKind_ = TypeKind::TYPE_SHAREDPTR;
            ptrTypeName_ = "std::shared_ptr";
        } else if (type == TokenType::UNIQUEPTR) {
            typeKind_ = TypeKind::TYPE_UNIQUEPTR;
            ptrTypeName_ = "std::unique_ptr";
        } else if (type == TokenType::SPTR) {
            typeKind_ = TypeKind::TYPE_SPTR;
            ptrTypeName_ = "sptr";
        }
    }

    inline void SetElementType(const AutoPtr<ASTType> &elementType)
    {
        elementType_ = elementType;
    }

    inline AutoPtr<ASTType> GetElementType()
    {
        return elementType_;
    }

    std::string GetSignature() override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;

protected:
    AutoPtr<ASTType> elementType_;

private:
    TypeKind typeKind_;
    std::string ptrTypeName_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTPTRTYPE_H