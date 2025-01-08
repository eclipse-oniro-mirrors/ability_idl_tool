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

#include <hash/hash.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "util/options.h"
#include "util/common.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool Hash::GenHashKey()
{
    FileDetailMap fileDetails;
    if (!Preprocessor::UnitPreprocess(fileDetails)) {
        return false;
    }

    std::string filePath = Options::GetInstance().GetOutPutFile();
    return filePath.empty() ? FormatStdout(fileDetails) : FormatFile(fileDetails, filePath);
}

bool Hash::FormatStdout(const FileDetailMap &fileDetails)
{
    std::vector<std::string> hashInfos = Hash::GetHashInfo(fileDetails);
    if (hashInfos.empty()) {
        return false;
    }

    for (const auto &info : hashInfos) {
        std::cout << info << "\n";
    }
    return true;
}

bool Hash::FormatFile(const FileDetailMap &fileDetails, const std::string &filePath)
{
    std::vector<std::string> hashInfos = Hash::GetHashInfo(fileDetails);
    if (hashInfos.empty()) {
        return false;
    }

    std::ofstream hashFile(filePath, std::ios::out | std::ios ::binary);
    if (!hashFile.is_open()) {
        Logger::E(TAG, "failed to open %s", filePath.c_str());
        return false;
    }

    for (const auto &info : hashInfos) {
        hashFile << info << "\n";
    }

    hashFile.close();
    return true;
}

std::vector<std::string> Hash::GetHashInfo(const FileDetailMap &fileDetails)
{
    std::vector<std::string> hashInfos;
    for (const auto &detail : fileDetails) {
        size_t haskKey = 0;
        std::stringstream hashInfo;
        if (!Hash::GenFileHashKey(detail.second.filePath_, haskKey)) {
            return std::vector<std::string>();
        }
        hashInfo << detail.first << ":" << haskKey;
        hashInfos.push_back(hashInfo.str());
    }

    return hashInfos;
}

bool Hash::GenFileHashKey(const std::string &path, size_t &hashKey)
{
    std::ifstream fs(path);
    if (!fs.is_open()) {
        Logger::E(TAG, "invalid file path '%s'", path.c_str());
        return false;
    }

    std::stringstream buffer;
    buffer << fs.rdbuf();

    hashKey = std::hash<std::string>()(buffer.str());
    return true;
}
} // namespace Idl
} // namespace OHOS
