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

#ifndef OHOS_IDL_ASTENUMTYPE_H
#define OHOS_IDL_ASTENUMTYPE_H

#include "ast/ast_attribute.h"
#include "ast/ast_expr.h"
#include "ast/ast_type.h"
#include "util/autoptr.h"

#include <vector>

namespace OHOS {
namespace Idl {
class ASTEnumValue : public ASTNode {
public:
    explicit ASTEnumValue(const std::string &name) : mName_(name), value_(nullptr) {}

    inline ~ASTEnumValue() override {}

    inline std::string GetName()
    {
        return mName_;
    }

    inline void SetType(const AutoPtr<ASTType> &type)
    {
        mType_ = type;
    }

    inline AutoPtr<ASTType> GetType()
    {
        return mType_;
    }

    inline void SetExprValue(const AutoPtr<ASTExpr> &value)
    {
        value_ = value;
    }

    inline AutoPtr<ASTExpr> GetExprValue()
    {
        return value_;
    }

private:
    std::string mName_;
    AutoPtr<ASTType> mType_;
    AutoPtr<ASTExpr> value_;
};

class ASTEnumType : public ASTType {
public:
    ASTEnumType() : ASTType(true), attr_(new ASTAttr()), baseType_(), members_() {}

    inline void SetName(const std::string &name) override
    {
        name_ = name;
    }

    inline std::string GetName() override
    {
        return name_;
    }

    inline void SetAttribute(const AutoPtr<ASTAttr> &attr)
    {
        if (attr != nullptr) {
            attr_ = attr;
        }
    }

    inline bool IsFull()
    {
        return attr_ != nullptr ? attr_->HasValue(ASTAttr::FULL) : false;
    }

    inline bool IsLite()
    {
        return attr_ != nullptr ? attr_->HasValue(ASTAttr::LITE) : false;
    }

    void SetBaseType(const AutoPtr<ASTType> &baseType);

    AutoPtr<ASTType> GetBaseType();

    bool AddMember(const AutoPtr<ASTEnumValue> &member);

    inline std::vector<AutoPtr<ASTEnumValue>> GetMembers()
    {
        return members_;
    }

    inline size_t GetMemberNumber()
    {
        return members_.size();
    }

    inline AutoPtr<ASTEnumValue> GetMember(size_t index)
    {
        if (index >= members_.size()) {
            return nullptr;
        }
        return members_[index];
    }

    inline bool HasMember(std::string memberName)
    {
        for (size_t i = 0; i < members_.size(); i++) {
            if (members_[i]->GetName() == memberName) {
                return true;
            }
        }
        return false;
    }
    std::string GetSignature() override;

    bool IsEnumType() override;

    std::string Dump(const std::string &prefix) override;

    TypeKind GetTypeKind() override;
private:
    AutoPtr<ASTAttr> attr_ = new ASTAttr();
    AutoPtr<ASTType> baseType_;
    AutoPtr<ASTType> parentType_;
    std::vector<AutoPtr<ASTEnumValue>> members_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTENUMTYPE_H