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

#include "util/file.h"

#include <climits>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <functional>
#include <string>
#include <algorithm>
#include <queue>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util/common.h"
#include "util/logger.h"
#include "util/string_helper.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
File::File(const std::string &path, unsigned int mode) : mode_(mode)
{
    if (path.empty()) {
        return;
    }

    if ((mode_ & read_) != 0) {
        OpenByRead(path);
        return;
    }

    if ((mode_ & write_) != 0) {
        fd_ = fopen(path.c_str(), "w+");
    } else if ((mode_ & append_) != 0) {
        fd_ = fopen(path.c_str(), "a+");
    }

    if (fd_ == nullptr) {
        Logger::E(TAG, "can't open '%s'", path.c_str());
        return;
    }

    path_ = RealPath(path);
}

File::~File()
{
    Close();
}

void File::OpenByRead(const std::string &path)
{
    if (!CheckValid(path)) {
        Logger::E(TAG, "failed to check path '%s'", path.c_str());
        return;
    }

    std::string realPath = RealPath(path);
    if (realPath.empty()) {
        Logger::E(TAG, "invalid path '%s'", path.c_str());
        return;
    }

    fd_ = fopen(realPath.c_str(), "r");
    if (fd_ == nullptr) {
        Logger::E(TAG, "can't open '%s'", realPath.c_str());
        return;
    }

    path_ = realPath;
    PeekChar();
}

char File::GetChar()
{
    char c = PeekChar();

    if (position_ + 1 <= size_) {
        position_++;

        if (c != '\n') {
            columnNo_++;
        } else {
            columnNo_ = 1;
            lineNo_++;
        }
    }
    return c;
}

char File::PeekChar()
{
    if (position_ + 1 > size_) {
        size_t size = Read();
        if (size == 0) {
            isEof_ = true;
        }
    }

    return buffer_[position_];
}

bool File::IsEof() const
{
    return isEof_ || buffer_[position_] == -1;
}

size_t File::Read()
{
    if (isEof_ || isError_) {
        return -1;
    }

    std::fill(buffer_, buffer_ + bufferSize_, 0);
    size_t count = fread(buffer_, 1, bufferSize_ - 1, fd_);
    if (count < bufferSize_ - 1) {
        isError_ = ferror(fd_) != 0;
        buffer_[count] = -1;
    }
    size_ = count;
    position_ = 0;
    return count;
}

size_t File::ReadData(void *data, size_t size) const
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    if (fd_ == nullptr) {
        return 0;
    }

    return fread(data, 1, size, fd_);
}

bool File::WriteData(const void *data, size_t size) const
{
    if (data == nullptr || size == 0) {
        return true;
    }

    if (fd_ == nullptr || !(mode_ & (write_ | append_))) {
        return false;
    }

    size_t count = fwrite(data, size, 1, fd_);
    return count == 1;
}

void File::Flush() const
{
    if ((mode_ & (write_ | append_)) && fd_ != nullptr) {
        fflush(fd_);
    }
}

bool File::Reset() const
{
    if (fd_ == nullptr) {
        return false;
    }

    return fseek(fd_, 0, SEEK_SET) == 0;
}

bool File::Skip(long size) const
{
    if (fd_ == nullptr) {
        return false;
    }

    return fseek(fd_, size, SEEK_CUR) == 0;
}

void File::Close()
{
    if (fd_ != nullptr) {
        fclose(fd_);
        fd_ = nullptr;
    }
}

bool File::CreateParentDir(const std::string &path)
{
    if (access(path.c_str(), F_OK | R_OK | W_OK) == 0) {
        return true;
    }

    size_t pos = 1;
    while ((pos = path.find(SEPARATOR, pos)) != std::string::npos) {
        std::string partPath = StringHelper::SubStr(path, 0, pos);
        partPath += SEPARATOR;
        if (File::CreatePartDir(partPath) == false) {
            return false;
        }
        pos += 1;
    }
    return true;
}

bool File::CreatePartDir(const std::string &partPath)
{
    struct stat st;
    if (stat(partPath.c_str(), &st) < 0) {
        if (errno != ENOENT) {
            return false;
        }

#ifndef __MINGW32__
        if (mkdir(partPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
#else
        if (mkdir(partPath.c_str()) < 0) {
#endif
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        return false;
    }
    return true;
}

std::string File::AdapterPath(const std::string &path)
{
#ifndef __MINGW32__
    std::string newPath = StringHelper::Replace(path, '\\', '/');
#else
    std::string newPath = StringHelper::Replace(path, '/', '\\');
#endif

    // "foo/v1_0//ifoo.h" -> "foo/v1_0/ifoo.h"
    StringBuilder adapterPath;
    bool hasSep = false;
    for (size_t i = 0; i < newPath.size(); i++) {
        char c = newPath[i];
        if (c == SEPARATOR) {
            if (hasSep) {
                continue;
            }
            adapterPath.Append(c);
            hasSep = true;
        } else {
            adapterPath.Append(c);
            hasSep = false;
        }
    }
    return adapterPath.ToString();
}

std::string File::AdapterRealPath(const std::string &path)
{
    if (path.empty()) {
        return "";
    }
    return RealPath(File::AdapterPath(path));
}

std::string File::RealPath(const std::string &path)
{
    if (path.empty()) {
        return "";
    }

    char realPath[PATH_MAX + 1];
#ifdef __MINGW32__
    char *absPath = _fullpath(realPath, path.c_str(), PATH_MAX);
#else
    char *absPath = realpath(path.c_str(), realPath);
#endif
    return absPath == nullptr ? "" : absPath;
}

bool File::CheckValid(const std::string &path)
{
    if (access(path.c_str(), F_OK | R_OK | W_OK) != 0) {
        return false;
    }

    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        return false;
    }

    if (S_ISDIR(st.st_mode)) {
        return false;
    }

    return true;
}

std::set<std::string> File::FindFiles(const std::string &rootDir)
{
    if (rootDir.empty()) {
        return std::set<std::string>();
    }

    std::set<std::string> files;
    std::queue<std::string> dirs;
    dirs.push(rootDir);
    while (!dirs.empty()) {
        std::string dirPath = dirs.front().back() == SEPARATOR ? dirs.front() : dirs.front() + SEPARATOR;
        dirs.pop();
        DIR *dir = opendir(dirPath.c_str());
        if (dir == nullptr) {
            Logger::E(TAG, "failed to open '%s', errno:%d", dirPath.c_str(), errno);
            continue;
        }

        struct dirent *dirInfo = readdir(dir);
        for (; dirInfo != nullptr; dirInfo = readdir(dir)) {
            if (strcmp(dirInfo->d_name, ".") == 0 || strcmp(dirInfo->d_name, "..") == 0) {
                continue;
            }
#ifndef __MINGW32__
            if (dirInfo->d_type == DT_REG && StringHelper::EndWith(dirInfo->d_name, ".idl")) {
                std::string filePath = dirPath + dirInfo->d_name;
                files.insert(filePath);
                continue;
            }

            if (dirInfo->d_type == DT_DIR) {
                dirs.emplace(dirPath + dirInfo->d_name);
                continue;
            }
#else
            std::string filePath = dirPath + dirInfo->d_name;
            struct stat fileInfo;
            if ((stat(filePath.c_str(), &fileInfo) == 0) && S_ISREG(fileInfo.st_mode)) {
                files.insert(filePath);
            }
#endif
        }
        closedir(dir);
    }

    return files;
}

size_t File::GetHashKey()
{
    StringBuilder fileStr;
    while (!IsEof()) {
        fileStr.Append(GetChar());
    }

    return std::hash<std::string>()(fileStr.ToString());
}
} // namespace Idl
} // namespace OHOS