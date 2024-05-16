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

#include "lexer/token.h"

#include <unordered_map>

#include "util/common.h"
#include "util/file.h"
#include "util/string_builder.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
std::string Token::Dump()
{
    StringBuilder sb;
    sb.AppendFormat("{kind:%u, row:%u, col:%u, value:%s}",
        static_cast<size_t>(kind), location.row, location.col, value.c_str());
    return sb.ToString();
}

std::string LocInfo(const Token &token)
{
    size_t index = token.location.filePath.rfind(SEPARATOR);
    std::string fileName =
        (index == std::string::npos) ? token.location.filePath : token.location.filePath.substr(index + 1);
    return StringHelper::Format("%s:%zu:%zu", fileName.c_str(), token.location.row, token.location.col);
}
} // namespace Idl
} // namespace OHOS