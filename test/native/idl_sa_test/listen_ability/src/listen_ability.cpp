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

#include "listen_ability.h"

#include "hilog/log.h"
#include "iremote_object.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {

static const int TEST_THREE = 3;
static const int TEST_FOUR = 4;
static const int TEST_SEVEN = 7;
static const int TEST_TEN = 10;
static const int TEST_NNN = 999;

REGISTER_SYSTEM_ABILITY_BY_ID(ListenAbility, 1494, true);

ListenAbility::ListenAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiLog::Info(LABEL, "ListenAbility()");
}

ListenAbility::~ListenAbility()
{
    HiLog::Info(LABEL, "~ListenAbility()");
}

ErrCode ListenAbility::AddVolume(int32_t volume)
{
    HiLog::Info(LABEL, "[idlTest] AddVolume service, volume: %{public}d", volume);
    return volume + 1;
}

ErrCode ListenAbility::TestSaCallSa(int32_t input, double &output)
{
    HiLog::Info(LABEL, "[idlTest] TestSaCallSa service, input: %{public}d, output: %{public}lf", input, output);
    return ERR_OK;
}

ErrCode ListenAbility::TestGetIpcSendRequestTimes(int32_t &times)
{
    times = 1;
    HiLog::Info(LABEL, "[idlTest] TestGetIpcSendRequestTimes service");
    return ERR_OK;
}

ErrCode ListenAbility::overloadfun(int32_t& outParam)
{
    outParam = TEST_TEN;
    HiLog::Info(LABEL, "[idlTest] overloadfunone service");
    return ERR_OK;
}

#ifdef DEVICE
ErrCode ListenAbility::overloadfun(std::unordered_map<int32_t, int32_t>& outParam)
{
    outParam[TEST_FOUR] = TEST_SEVEN;
    HiLog::Info(LABEL, "[idlTest] overloadfuntwo service");
    return ERR_OK;
}
#endif

ErrCode ListenAbility::enum_test_func(FooEnum inParam, FooEnum& outParam, FooEnum& inoutParam, FooEnum& funcResult)
{
    outParam = FooEnum::ENUM_TWO;
    HiLog::Info(IFOOCUSTOM_LABEL, "[idlTest] enum_test_func service said: %{public}d", static_cast<int>(inParam));
    return ERR_OK;
}

ErrCode ListenAbility::struct_test_func(
    const FooStruct& inParam,
    RequestInfo& outParam,
    FooStruct& inoutParam,
    FooStruct& funcResult)
{
    outParam.initData.push_back(TEST_THREE);
    HiLog::Info(IFOOCUSTOM_LABEL, "[idlTest] struct_test_func service, said: %{public}s", inParam.name.c_str());
    return ERR_OK;
}

ErrCode ListenAbility::union_test_func(
    const FooUnion& inParam,
    FooUnion& outParam,
    FooUnion& inoutParam,
    FooUnion& funcResult)
{
    outParam.enumType = FooEnum::ENUM_TWO;
    HiLog::Info(IFOOCUSTOM_LABEL, "[idlTest] union_test_func service, said: %{public}d",
        static_cast<int>(inParam.enumType));
    return ERR_OK;
}

ErrCode ListenAbility::ApplyQuickFix(const std::vector<std::string>& quickFixFiles, bool isDebug)
{
    std::string debugTrue;
    if (isDebug) {
        debugTrue = "true";
    }
    HiLog::Info(LABEL, "[idlTest] ApplyQuickFix service, said: %{public}s", debugTrue.c_str());
    return ERR_OK;
}

ErrCode ListenAbility::GetAllAppSuspendState(
    const std::unordered_map<int32_t, FooStruct>& inApp,
    std::unordered_map<int32_t, FooStruct>& outApp)
{
    outApp[1].id = TEST_NNN;
    outApp[1].name = "MapTest";
    HiLog::Info(LABEL, "[idlTest] GetAllAppSuspendState service,said: %{public}d, %{public}s",
        inApp.at(0).id, inApp.at(0).name.c_str());
    return ERR_OK;
}

ErrCode ListenAbility::rawdata_test_func(
    const MyRawdata& inParam,
    MyRawdata& outParam,
    MyRawdata& inoutParam,
    MyRawdata& funcResult)
{
    const char sampleData[] = "Hello, world!";
    const char sampleData2[] = "world!";
    outParam.size = sizeof(sampleData);
    outParam.RawDataCpy(sampleData);
    HiLog::Info(LABEL, "[idlTest] inout rawdata service, said: %{public}u, %{public}s",
        inoutParam.size, static_cast<const char*>(inoutParam.data));
    inoutParam.size = sizeof(sampleData2);
    inoutParam.RawDataCpy(sampleData2);
    funcResult.size = sizeof(sampleData2);
    funcResult.RawDataCpy(sampleData2);
    HiLog::Info(LABEL, "[idlTest] in rawdata service, said: %{public}u, %{public}s",
        inParam.size, static_cast<const char*>(inParam.data));
    return ERR_OK;
}

ErrCode fd_test_func(int& fd) {
    fd = 0;
    HiLog::Info(LABEL, "[idlTest] fd_test_func service");
    return ERR_OK;

void ListenAbility::OnStart()
{
    HiLog::Info(LABEL, "[idlTest] OnStart() service");
    bool res = Publish(this);
    if (!res) {
        HiLog::Info(LABEL, "[idlTest] OnStart service Publish failed");
        return;
    }
    HiLog::Info(LABEL, "[idlTest] ListenAbility service begin");
}

void ListenAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HiLog::Info(LABEL, "[idlTest] OnAddSystemAbility service begin, said: %{public}d", systemAbilityId);
}

void ListenAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    HiLog::Info(LABEL, "[idlTest] OnRemoveSystemAbility service begin, said: %{public}d", systemAbilityId);
}

}