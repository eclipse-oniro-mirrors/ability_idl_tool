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

#ifndef IDL_SA_TEST_LISTEN_ABILITY_H
#define IDL_SA_TEST_LISTEN_ABILITY_H

#include <string>

#include "foo_ptr_stub.h"
#include "system_ability.h"

namespace OHOS {
class ListenAbility : public SystemAbility, public FooPtrStub {
    DECLARE_SYSTEM_ABILITY(ListenAbility);

public:
    ListenAbility(int32_t systemAbilityId, bool runOnCreate);
    ~ListenAbility();

    ErrCode sharedptr_seq_func(
        const std::shared_ptr<MySeq>& inParam,
        std::shared_ptr<MySeq>& inoutParam,
        std::shared_ptr<MySeq>& outParam,
        std::shared_ptr<MySeq>& funcResult) override;

    ErrCode uniqueptr_seq_func(
        const std::unique_ptr<MySeq>& inParam,
        std::unique_ptr<MySeq>& inoutParam,
        std::unique_ptr<MySeq>& outParam,
        std::unique_ptr<MySeq>& funcResult) override;

    ErrCode sptr_seq_func(
        const sptr<MySeq>& inParam,
        sptr<MySeq>& inoutParam,
        sptr<MySeq>& outParam,
        sptr<MySeq>& funcResult) override;

protected:
    void OnStart() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
};
} // namespace OHOS

#endif // IDL_SA_TEST_LISTEN_ABILITY_H