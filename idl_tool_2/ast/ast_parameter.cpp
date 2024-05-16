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

#include "ast/ast_parameter.h"
#include "ast/ast_array_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
std::string ASTParameter::Dump(const std::string &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);
    sb.Append('[');

    if (attr_->value_ == ASTParamAttr::PARAM_IN) {
        sb.Append("in");
    } else if (attr_->value_ == ASTParamAttr::PARAM_OUT) {
        sb.Append("out");
    } else if (attr_->value_ == ASTParamAttr::PARAM_INOUT) {
        sb.Append("in, out");
    }

    sb.Append("] ");
    sb.Append(type_->ToString()).Append(' ');
    sb.Append(name_);

    return sb.ToString();
}
} // namespace Idl
} // namespace OHOS