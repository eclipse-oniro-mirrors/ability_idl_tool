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

#ifndef MY_SEQ_H
#define MY_SEQ_H

#include "parcel.h"
#include "hilog/log.h"

namespace OHOS {

static constexpr int TEST_SAID = 1494;
static constexpr int TEST_IN_SIZE_FIVE = 5;
static constexpr int TEST_IN_SIZE_SEVEN = 7;
static constexpr int TEST_IN_SIZE_ELEVEN = 11;
static constexpr int TEST_IN_SIZE_TWENTY = 20;
static constexpr int TEST_IN_SIZE_FIFTY = 50;

using namespace OHOS::HiviewDFX;
class MySeq : public Parcelable {
public:
    int32_t size;

    virtual bool Marshalling(Parcel &parcel) const override;
    static MySeq *Unmarshalling(Parcel &parcel);
protected:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD001718, "idlTest"};
};
} // namespace OHOS

#endif // MY_SEQ_H