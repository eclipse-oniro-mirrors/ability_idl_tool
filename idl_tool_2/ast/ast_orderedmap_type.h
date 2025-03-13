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

#ifndef OHOS_IDL_ASTORDEREDMAPTYPE_H
#define OHOS_IDL_ASTORDEREDMAPTYPE_H

#include "ast/ast_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTOrderedMapType : public ASTType {
public:
    ASTOrderedMapType() : ASTType(false), keyType_(), valueType_() {}

    inline void SetKeyType(const AutoPtr<ASTType> &keyType)
    {
        keyType_ = keyType;
    }

    inline AutoPtr<ASTType> GetKeyType()
    {
        return keyType_;
    }

    inline void SetValueType(const AutoPtr<ASTType> &valueType)
    {
        valueType_ = valueType;
    }

    inline AutoPtr<ASTType> GetValueType()
    {
        return valueType_;
    }

    std::string GetSignature() override;

    bool IsOrderedMapType() override;

    bool HasInnerType(TypeKind innerTypeKind) const override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;
private:
    AutoPtr<ASTType> keyType_;
    AutoPtr<ASTType> valueType_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTORDEREDMAPTYPE_H