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

#ifndef OHOS_IDL_ASTUNIONTYPE_H
#define OHOS_IDL_ASTUNIONTYPE_H

#include <tuple>
#include <vector>

#include "ast/ast_attribute.h"
#include "ast/ast_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTUnionType : public ASTType {
public:
    ASTUnionType() : ASTType(true), attr_(new ASTAttr()), members_() {}

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

    void AddMember(const AutoPtr<ASTType> &typeName, std::string name);

    inline size_t GetMemberNumber()
    {
        return members_.size();
    }

    inline std::string GetMemberName(size_t index)
    {
        if (index >= members_.size()) {
            return std::string("");
        }
        return std::get<0>(members_[index]);
    }

    inline AutoPtr<ASTType> GetMemberType(size_t index)
    {
        if (index >= members_.size()) {
            return nullptr;
        }
        return std::get<1>(members_[index]);
    }

    std::string GetSignature() override;

    bool IsUnionType() override;

    std::string Dump(const std::string &prefix) override;

    TypeKind GetTypeKind() override;
private:
    AutoPtr<ASTAttr> attr_;
    std::vector<std::tuple<std::string, AutoPtr<ASTType>>> members_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTUNIONTYPE_H