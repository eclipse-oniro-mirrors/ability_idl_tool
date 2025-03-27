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

REGISTER_SYSTEM_ABILITY_BY_ID(ListenAbility, TEST_SAID, true);

ListenAbility::ListenAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiLog::Info(LABEL, "ListenAbility()");
}

ListenAbility::~ListenAbility()
{
    HiLog::Info(LABEL, "~ListenAbility()");
}

ErrCode ListenAbility::sharedptr_seq_func(
    const std::shared_ptr<MySeq>& inParam,
    std::shared_ptr<MySeq>& inoutParam,
    std::shared_ptr<MySeq>& outParam,
    std::shared_ptr<MySeq>& funcResult)
{
    HiLog::Info(LABEL, "[idlTest] sharedptr_seq_func service, shared_ptr inParam=%{public}d, inoutParam=%{public}d ",
        inParam->size, inoutParam->size);
    inoutParam->size = TEST_IN_SIZE_ELEVEN;
    MySeq* outParamTmp = new MySeq();
    outParamTmp->size = TEST_IN_SIZE_TWENTY;
    outParam = std::shared_ptr<MySeq>(outParamTmp);
    MySeq* resultParamTmp = new MySeq();
    resultParamTmp->size = TEST_IN_SIZE_FIFTY;
    funcResult = std::shared_ptr<MySeq>(resultParamTmp);
    HiLog::Info(LABEL, "[idlTest] sharedptr_seq_func service, shared_ptr outParam=%{public}d, funcResult=%{public}d ",
        outParam->size, funcResult->size);
    return ERR_OK;
}

ErrCode ListenAbility::uniqueptr_seq_func(
    const std::unique_ptr<MySeq>& inParam,
    std::unique_ptr<MySeq>& inoutParam,
    std::unique_ptr<MySeq>& outParam,
    std::unique_ptr<MySeq>& funcResult)
{
    HiLog::Info(LABEL, "[idlTest] uniqueptr_seq_func service, unique_ptr inParam=%{public}d, inoutParam=%{public}d ",
        inParam->size, inoutParam->size);
    inoutParam->size = TEST_IN_SIZE_ELEVEN;
    MySeq* outParamTmp = new MySeq();
    outParamTmp->size = TEST_IN_SIZE_TWENTY;
    outParam = std::unique_ptr<MySeq>(outParamTmp);
    MySeq* resultParamTmp = new MySeq();
    resultParamTmp->size = TEST_IN_SIZE_FIFTY;
    funcResult = std::unique_ptr<MySeq>(resultParamTmp);
    HiLog::Info(LABEL, "[idlTest] uniqueptr_seq_func service, unique_ptr outParam=%{public}d, funcResult=%{public}d ",
        outParam->size, funcResult->size);
    return ERR_OK;
}

ErrCode ListenAbility::sptr_seq_func(
    const sptr<MySeq>& inParam,
    sptr<MySeq>& inoutParam,
    sptr<MySeq>& outParam,
    sptr<MySeq>& funcResult)
{
    HiLog::Info(LABEL, "[idlTest] sptr_seq_func service, sptr inParam=%{public}d, inoutParam=%{public}d ",
        inParam->size, inoutParam->size);
    inoutParam->size = TEST_IN_SIZE_ELEVEN;
    MySeq* outParamTmp = new MySeq();
    outParamTmp->size = TEST_IN_SIZE_TWENTY;
    outParam = sptr<MySeq>(outParamTmp);
    MySeq* resultParamTmp = new MySeq();
    resultParamTmp->size = TEST_IN_SIZE_FIFTY;
    funcResult = sptr<MySeq>(resultParamTmp);
    HiLog::Info(LABEL, "[idlTest] sptr_seq_func service, sptr outParam=%{public}d, funcResult=%{public}d ",
        outParam->size, funcResult->size);
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