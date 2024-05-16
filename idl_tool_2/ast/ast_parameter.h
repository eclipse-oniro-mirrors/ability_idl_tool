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

#ifndef OHOS_IDL_ASTPARAMETER_H
#define OHOS_IDL_ASTPARAMETER_H

#include "ast/ast_attribute.h"
#include "ast/ast_node.h"
#include "ast/ast_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTParameter : public ASTNode {
public:
    ASTParameter(const std::string &name, ASTParamAttr::ParamAttr attribute, const AutoPtr<ASTType> &type)
        : ASTNode(), name_(name), attr_(new ASTParamAttr(attribute)), type_(type)
    {
    }

    ASTParameter(const std::string &name, const AutoPtr<ASTParamAttr> &attribute, const AutoPtr<ASTType> &type)
        : ASTNode(), name_(name), attr_(attribute), type_(type)
    {
    }

    inline std::string GetName()
    {
        return name_;
    }

    inline AutoPtr<ASTType> GetType()
    {
        return type_;
    }

    inline ASTParamAttr::ParamAttr GetAttribute()
    {
        return attr_->value_;
    }

    std::string Dump(const std::string &prefix) override;

private:
    std::string name_;
    AutoPtr<ASTParamAttr> attr_;
    AutoPtr<ASTType> type_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTPARAMETER_H