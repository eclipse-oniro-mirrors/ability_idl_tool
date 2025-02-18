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

#ifndef OHOS_IDL_FILE_H
#define OHOS_IDL_FILE_H

#include <cstdio>
#include <set>
#include <string>
#include <vector>

namespace OHOS {
namespace Idl {
class File {
public:
    File(const std::string &path, unsigned int mode);

    ~File();

    inline bool IsValid() const
    {
        return fd_ != nullptr;
    }

    inline std::string GetPath() const
    {
        return path_;
    }

    char GetChar();

    char PeekChar();

    char NextChar();

    bool IsEof() const;

    inline size_t GetCharLineNumber() const
    {
        return lineNo_;
    }

    inline size_t GetCharColumnNumber() const
    {
        return columnNo_;
    }

    size_t ReadData(void *data, size_t size) const;

    bool WriteData(const void *data, size_t size) const;

    void Flush() const;

    bool Reset() const;

    bool Skip(long size) const;

    void Close();

    static bool CreateParentDir(const std::string &path);

    static bool CreatePartDir(const std::string &partPath);

    static std::string AdapterPath(const std::string &path);

    static std::string AdapterRealPath(const std::string &path);

    static std::string RealPath(const std::string &path);

    static std::string CanonicalPath(const std::string &path);

    static std::string AbsolutePath(const std::string &path);

    static std::vector<std::string> SplitPath(const std::string &path);

    static std::string RelativePath(const std::string &pathA, const std::string &pathB);

    static bool CheckValid(const std::string &path);

    static std::set<std::string> FindFiles(const std::string &rootDir);

    size_t GetHashKey();

    static constexpr unsigned int READ = 0x1;
    static constexpr unsigned int WRITE = 0x2;
    static constexpr unsigned int APPEND = 0x4;

private:
    size_t Read();

    void OpenByRead(const std::string &path);

    static constexpr int BUFFER_SIZE = 1024;

    char buffer_[BUFFER_SIZE] = {0};
    size_t size_ = 0;
    size_t position_ = 0;
    size_t columnNo_ = 1;
    size_t lineNo_ = 1;
    bool isEof_ = false;
    bool isError_ = false;

    FILE *fd_ = nullptr;
    std::string path_;
    unsigned int mode_ = 0;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_FILE_H