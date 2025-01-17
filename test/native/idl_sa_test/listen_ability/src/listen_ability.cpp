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