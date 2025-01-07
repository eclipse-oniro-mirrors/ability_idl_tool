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

#include "util/string_helper.h"

#include <cstdarg>

#include "securec.h"

namespace OHOS {
namespace Idl {
std::vector<std::string> StringHelper::Split(const std::string& sources, const std::string &limit)
{
    std::vector<std::string> result;
    if (sources.empty()) {
        return result;
    }

    if (limit.empty()) {
        result.push_back(sources);
        return result;
    }

    size_t begin = 0;
    size_t pos = sources.find(limit, begin);
    while (pos != std::string::npos) {
        std::string element = sources.substr(begin, pos - begin);
        if (!element.empty()) {
            result.push_back(element);
        }
        begin = pos + limit.size();
        pos = sources.find(limit, begin);
    }

    if (begin < sources.size()) {
        std::string element = sources.substr(begin);
        result.push_back(element);
    }
    return result;
}

bool StringHelper::StartWith(const std::string &value, char prefix)
{
    return value.find(prefix) == 0;
}

bool StringHelper::StartWith(const std::string &value, const std::string &prefix)
{
    return value.find(prefix) == 0;
}

bool StringHelper::EndWith(const std::string &value, char suffix)
{
    if (value.empty()) {
        return false;
    }
    return value.back() == suffix;
}

bool StringHelper::EndWith(const std::string &value, const std::string &suffix)
{
    size_t index = value.rfind(suffix);
    if (index == std::string::npos) {
        return false;
    }

    return index + suffix.size() == value.size();
}

std::string StringHelper::Replace(const std::string &value, char oldChar, char newChar)
{
    if (value.empty() || oldChar == newChar) {
        return value;
    }

    std::string result = value;
    for (char &c : result) {
        if (c != oldChar) {
            continue;
        }
        c = newChar;
    }
    return result;
}

std::string StringHelper::Replace(const std::string &value, const std::string &oldStr, const std::string &newStr)
{
    std::string result = value;
    size_t pos = 0;
    while ((pos = result.find(oldStr, pos)) != std::string::npos) {
        result.replace(pos, oldStr.size(), newStr);
        pos += newStr.size();
    }
    return result;
}

std::string StringHelper::Replace(const std::string &value, size_t position, const std::string &substr,
    const std::string &newStr)
{
    if (position >= value.size()) {
        return value;
    }

    std::string prefix = value.substr(0, position);
    std::string suffix = value.substr(position);
    return prefix + Replace(suffix, substr, newStr);
}

std::string StringHelper::Replace(const std::string &value, size_t position, size_t len, const std::string &newStr)
{
    if (position >= value.size() || len == 0) {
        return value;
    }

    std::string prefix = value.substr(0, position);
    std::string suffix = value.substr(position);
    return prefix + newStr + suffix;
}

std::string StringHelper::SubStr(const std::string &value, size_t start, size_t end)
{
    if (value.empty() || start == std::string::npos || start >= end) {
        return "";
    }
    return end == std::string::npos ? value.substr(start) : value.substr(start, end - start);
}

std::string StringHelper::StrToLower(const std::string &value)
{
    std::string result = value;
    for (char &c : result) {
        if (std::isupper(c)) {
            c = static_cast<char>(std::tolower(c));
        }
    }
    return result;
}

std::string StringHelper::StrToUpper(const std::string &value)
{
    std::string result = value;
    for (char &c : result) {
        if (std::islower(c)) {
            c = static_cast<char>(std::toupper(c));
        }
    }
    return result;
}

std::string StringHelper::FirstToUpper(const std::string &value)
{
    std::string result = value;
    if (!result.empty()) {
        result[0] = static_cast<char>(std::toupper(result[0]));
    }
    return result;
}

std::string StringHelper::Format(const char *format, ...)
{
    va_list args;
    va_list argsCopy;

    va_start(args, format);
    va_copy(argsCopy, args);

    char buf[lineMaxSize] = {0};
    int len = vsnprintf_s(buf, lineMaxSize, lineMaxSize - 1, format, args);
    if (len <= 0) {
        va_end(args);
        va_end(argsCopy);
        return "";
    }

    va_end(args);
    va_end(argsCopy);
    return {buf, static_cast<size_t>(len)};
}

int StringHelper::GetHashCode(const std::string &key)
{
    // BKDR Hash Function
    constexpr unsigned int seed = 31; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    const char* string = key.c_str();
    if (string != nullptr) {
        for (; *string; ++string) {
            hash = hash * seed + (*string);
        }
    }
    return static_cast<int>(hash & 0x7FFFFFFF);
}

} // namespace Idl
} // namespace OHOS