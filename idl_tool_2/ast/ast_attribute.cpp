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

#include "ast/ast_attribute.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
std::string ASTAttr::ToString() const
{
    std::vector<std::string> attrs;
    if (value_ & ASTAttr::MINI) {
        attrs.push_back("mini");
    }

    if (value_ & ASTAttr::LITE) {
        attrs.push_back("lite");
    }

    if (value_ & ASTAttr::FULL) {
        attrs.push_back("full");
    }

    if (value_ & ASTAttr::ONEWAY) {
        attrs.push_back("oneway");
    }

    if (value_ & ASTAttr::CALLBACK) {
        attrs.push_back("callback");
    }

    StringBuilder sb;
    sb.Append("[");
    for (size_t i = 0; i < attrs.size(); i++) {
        sb.Append(attrs[i]);
        if (i + 1 < attrs.size()) {
            sb.Append(", ");
        }
    }
    sb.Append("] ");
    return sb.ToString();
}

std::string ASTAttr::Dump(const std::string &prefix)
{
    return prefix + ToString();
}

bool ASTAttr::Match(SystemLevel level) const
{
    switch (level) {
        case SystemLevel::MINI:
            return HasValue(ASTAttr::MINI);
        case SystemLevel::LITE:
            return HasValue(ASTAttr::LITE);
        case SystemLevel::FULL:
            return HasValue(ASTAttr::FULL);
        default:
            return false;
    }
}

std::string ASTParamAttr::ToString() const
{
    std::vector<std::string> attrs;
    if (value_ & ASTParamAttr::PARAM_IN) {
        attrs.push_back("in");
    }

    if (value_ & ASTParamAttr::PARAM_OUT) {
        attrs.push_back("out");
    }

    StringBuilder sb;
    sb.Append("[");
    for (size_t i = 0; i < attrs.size(); i++) {
        sb.Append(attrs[i]);
        if (i + 1 < attrs.size()) {
            sb.Append(", ");
        }
    }
    sb.Append("]");
    return sb.ToString();
}

std::string ASTParamAttr::Dump(const std::string &prefix)
{
    return prefix + ToString();
}

bool ASTAttr::CacheableStrToInt()
{
    if (!HasValue(ASTAttr::CACHEABLE)) {
        return false;
    }
    try {
        cacheableTime_ = static_cast<int32_t>(std::stoi(cacheableTimeString_));
    } catch(...) {
        return false;
    }
    return true;
}
} // namespace Idl
} // namespace OHOS