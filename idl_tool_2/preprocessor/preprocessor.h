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

#ifndef OHOS_IDL_PREPROCESSOR_H
#define OHOS_IDL_PREPROCESSOR_H

#include <map>
#include <set>
#include <unordered_set>
#include <vector>

#include "lexer/lexer.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
class FileDetail {
public:
    inline std::string GetFullName() const
    {
        return StringHelper::Format("%s.%s", packageName_.c_str(), fileName_.c_str());
    }

    std::string Dump() const;

public:
    std::string filePath_;
    std::string fileName_;
    std::string packageName_;
    std::unordered_set<std::string> imports_;
};

using FileDetailMap = std::map<std::string, FileDetail>;

class Preprocessor {
public:
    // analyze idl files and return sorted ids files
    static bool Preprocess(std::vector<FileDetail> &fileDetails);

    static bool UnitPreprocess(FileDetailMap &fileDetails);

private:
    static bool CheckAllFilesPath(const std::set<std::string> &sourceFiles);

    static bool AnalyseImportInfo(std::set<std::string> sourceFiles, FileDetailMap &allFileDetails);

    static bool ParseFileDetail(const std::string &sourceFile, FileDetail &info);

    static bool ParsePackage(Lexer &lexer, FileDetail &info);

    static bool ParseImports(Lexer &lexer, FileDetail &info);

    static bool LoadOtherIdlFiles(
        const FileDetail &ownerFileDetail, FileDetailMap &allFileDetails, std::set<std::string> &sourceFiles);

    static bool CheckCircularReference(const FileDetailMap &allFileDetails,
        std::vector<FileDetail> &compileSourceFiles);

    static void PrintCyclefInfo(FileDetailMap &allFileDetails);

    static void FindCycle(const std::string &curNode, FileDetailMap &allFiles, std::vector<std::string> &trace);

    // check if the file path matches the package name
    static bool CheckPackageName(const std::string &filePath, const std::string &packageName);
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_PREPROCESSOR_H