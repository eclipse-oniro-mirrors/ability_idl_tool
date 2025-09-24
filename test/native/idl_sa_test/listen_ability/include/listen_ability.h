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

#include "listen_ability_stub.h"
#include "system_ability.h"
#include "ifoo_custom.h"

namespace OHOS {
class ListenAbility : public SystemAbility, public ListenAbilityStub {
    DECLARE_SYSTEM_ABILITY(ListenAbility);

public:
    ListenAbility(int32_t systemAbilityId, bool runOnCreate);
    ~ListenAbility();
    ErrCode AddVolume(int32_t volume) override;
    ErrCode TestSaCallSa(int32_t input, double &output) override;
    ErrCode TestGetIpcSendRequestTimes(int32_t &times) override;
    ErrCode overloadfun(
        int32_t& outParam) override;

#ifdef DEVICE
    ErrCode overloadfun(
        std::unordered_map<int32_t, int32_t>& outParam) override;
#endif

    ErrCode enum_test_func(
        FooEnum inParam,
        FooEnum& outParam,
        FooEnum& inoutParam,
        FooEnum& funcResult) override;
    ErrCode struct_test_func(
        const FooStruct& inParam,
        RequestInfo& outParam,
        FooStruct& inoutParam,
        FooStruct& funcResult) override;
    ErrCode union_test_func(
        const FooUnion& inParam,
        FooUnion& outParam,
        FooUnion& inoutParam,
        FooUnion& funcResult) override;

    ErrCode ApplyQuickFix(const std::vector<std::string>& quickFixFiles, bool isDebug) override;
    ErrCode GetAllAppSuspendState(
        const std::unordered_map<int32_t,
        FooStruct>& inApp,
        std::unordered_map<int32_t, FooStruct>& outApp) override;

    ErrCode rawdata_test_func(
        const MyRawdata& inParam,
        MyRawdata& outParam,
        MyRawdata& inoutParam,
        MyRawdata& funcResult) override;
    
    ErrCode fd_test_func(int32_t& fd) override;

protected:
    void OnStart() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
};
} // namespace OHOS

#endif // IDL_SA_TEST_LISTEN_ABILITY_H