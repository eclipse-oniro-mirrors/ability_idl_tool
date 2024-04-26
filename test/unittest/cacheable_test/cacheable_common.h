/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_CACHEABLE_COMMON_H
#define OHOS_CACHEABLE_COMMON_H
#include "securec.h"
#include "util/string.h"
#include "util/string_builder.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {
namespace TestCommon {
const int ERR_FAIL = -1;
const int ERR_OK = 0;

class ParameterArgv {
public:
    ParameterArgv(const char** args, const int argc) : argc_(argc)
    {
        argv_ = new char* [argc_] {nullptr};
        for (int i = 0; i < argc_; ++i) {
            const int itemSize = strlen(args[i]);
            argv_[i] = new char[itemSize + 1] {0};
            if (strcpy_s(argv_[i], itemSize + 1, args[i]) != ERR_OK) {
                GTEST_LOG_(ERROR) << "strcpy_s error [!EOK]";
                return;
            }
        }
    };
    ~ParameterArgv()
    {
        if (argv_ == nullptr) {
            return;
        }

        for (int i = 0; i < argc_; ++i) {
            if (argv_[i] == nullptr) {
                continue;
            }
            delete[] argv_[i];
            argv_[i] = nullptr;
        }
        delete[] argv_;
        argv_ = nullptr;
    };

    char** GetArgv()
    {
        return argv_;
    };

    char** argv_;
    const int argc_;
};

int PrepareIdlFile(const std::string &fileName, const std::string &fileContent)
{
    String filePath = String::Format("%s/%s", "./", fileName.c_str());
    File file(filePath, File::WRITE);
    if (!file.IsValid()) {
        GTEST_LOG_(INFO) << "OPEN FILE FAIL";
        return ERR_FAIL;
    }

    StringBuilder stringBuilder;
    stringBuilder.Append(fileContent.c_str());

    String data = stringBuilder.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
    return ERR_OK;
}

}
}
}
#endif