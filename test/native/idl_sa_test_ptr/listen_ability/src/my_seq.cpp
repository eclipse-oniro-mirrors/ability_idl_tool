/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "my_seq.h"

#include "hilog/log.h"

namespace OHOS {

bool MySeq::Marshalling(Parcel &parcel) const
{
    HiLog::Info(LABEL, "Marshalling size=%{public}d ", size);
    parcel.WriteInt32(size);
    return true;
}

MySeq *MySeq::Unmarshalling(Parcel &parcel)
{
    MySeq *info = new (std::nothrow) MySeq();
    if (info == nullptr) {
        HiLog::Info(LABEL, "Unmarshalling Create failed");
        return nullptr;
    }

    info->size = parcel.ReadInt32();
    HiLog::Info(LABEL, "Unmarshalling size=%{public}d ", info->size);

    return info;
}

}