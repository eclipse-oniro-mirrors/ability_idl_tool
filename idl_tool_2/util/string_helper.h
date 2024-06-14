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

#ifndef OHOS_IDL_STRING_HELPER_H
#define OHOS_IDL_STRING_HELPER_H

#include <cstring>
#include <string>
#include <vector>

namespace OHOS {
namespace Idl {
class StringHelper {
public:
    static std::vector<std::string> Split(std::string sources, const std::string &limit);

    static bool StartWith(const std::string &value, char prefix);

    static bool StartWith(const std::string &value, const std::string &prefix);

    static bool EndWith(const std::string &value, char suffix);

    static bool EndWith(const std::string &value, const std::string &suffix);

    static std::string Replace(const std::string &value, char oldChar, char newChar);

    static std::string Replace(const std::string &value, const std::string &oldstr, const std::string &newstr);

    static std::string Replace(
        const std::string &value, size_t position, const std::string &substr, const std::string &newstr);

    static std::string Replace(const std::string &value, size_t position, size_t len, const std::string &newStr);

    static std::string SubStr(const std::string &value, size_t start, size_t end = std::string::npos);

    static std::string StrToLower(const std::string &value);

    static std::string StrToUpper(const std::string &value);

    static std::string FirstToUpper(const std::string &value);

    static std::string Format(const char *format, ...);

    static int GetHashCode(const std::string &key);

    static constexpr size_t lineMaxSize = 2048;  // 2KB
    static constexpr size_t maxSize = 262144;    // 256KB
};

struct StringHashFunc {
    int operator()(const std::string& key) const
    {
        return StringHelper::GetHashCode(key);
    }
};

struct StringEqualFunc {
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return lhs == rhs;
    }
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_STRING_HELPER_H