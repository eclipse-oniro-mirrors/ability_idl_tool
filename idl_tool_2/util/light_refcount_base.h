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

#ifndef OHOS_IDL_LIGHTREFCOUNTBASE_H
#define OHOS_IDL_LIGHTREFCOUNTBASE_H

#include <atomic>

namespace OHOS {
namespace Idl {
class LightRefCountBase {
public:
    inline LightRefCountBase() : refCount_(0) {}

    int AddRef();

    int Release();

    inline int GetRefCount() const
    {
        return refCount_.load(std::memory_order_relaxed);
    }

protected:
    virtual ~LightRefCountBase() = default;

private:
    std::atomic<int> refCount_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_LIGHTREFCOUNTBASE_H