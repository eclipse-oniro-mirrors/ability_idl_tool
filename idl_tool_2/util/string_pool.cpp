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

#include "util/string_pool.h"
#include "securec.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
const char* StringPool::TAG = "StringPool";

StringPool::StringPool()
{
    data_ = reinterpret_cast<uint8_t *>(calloc(1, dataCapacity_));
    if (data_ == nullptr) {
        Logger::E(TAG, "Out of memory.");
    }
}

StringPool::~StringPool()
{
    if (data_ != nullptr) {
        free(data_);
    }
}

void StringPool::Add(const std::string& string)
{
    if (string.empty() || stringOffsets_.find(string) != stringOffsets_.end()) {
        return;
    }

    ptrdiff_t offset = AddInternal(string);
    if (offset != -1) {
        stringOffsets_[string] = offset;
    }
}

ptrdiff_t StringPool::GetOffset(const std::string& string)
{
    return stringOffsets_[string];
}

ptrdiff_t StringPool::AddInternal(const std::string& string)
{
    if (!Grow(string.length() + 1)) {
        return -1;
    }

    uint8_t* addr = data_ + dataOffset_;
    if (strcpy_s(reinterpret_cast<char *>(addr), dataCapacity_ - dataOffset_, string.c_str())) {
        Logger::E(TAG, "Error to copy str");
        return -1;
    }
    dataOffset_ += static_cast<ptrdiff_t>(string.length()) + 1;
    return addr - data_;
}

bool StringPool::Grow(size_t expand)
{
    size_t newSize = dataOffset_ + expand;
    if (newSize < dataCapacity_) {
        return true;
    }
    // 3->3x capacity expansion
    size_t step = dataCapacity_ * 3;
    newSize = step > newSize ? step : step + newSize;
    uint8_t* newData = reinterpret_cast<uint8_t *>(calloc(1, newSize));
    if (newData == nullptr) {
        Logger::E(TAG, "Out of memory.");
        return false;
    }
    if (memcpy_s(newData, newSize, data_, dataOffset_) != EOK) {
        free(newData);
        return false;
    }
    free(data_);
    data_ = newData;
    dataCapacity_ = newSize;
    return true;
}
}
}
