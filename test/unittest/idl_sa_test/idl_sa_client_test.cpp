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

#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "listen_ability_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Idl {

class IdlSaUnitTest : public testing::Test {
public:
    IdlSaUnitTest() {}

    virtual ~IdlSaUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void IdlSaUnitTest::SetUpTestCase() {}

void IdlSaUnitTest::TearDownTestCase() {}

void IdlSaUnitTest::SetUp() {}

void IdlSaUnitTest::TearDown() {}

/*
 * @tc.name: IdlSaProxyTest001
 * @tc.desc: sync func run
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaProxyTest001, TestSize.Level1)
{
    std::cout << "IdlSaProxyTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    if (client_ == nullptr) {
        std::cout << "client is nullptr" << std::endl;
        return;
    }

    // sync func
    double retDouble;
    int32_t ret = client_->TestSaCallSa(100, retDouble);
    std::cout << "TestSaCallSa client" << std::endl;
    EXPECT_EQ(ret, ERR_OK);

    int32_t times;
    ret = client_->TestGetIpcSendRequestTimes(times);
    std::cout << "TestGetIpcSendRequestTimes client" << std::endl;
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(times, 1);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlSaProxyTest002
 * @tc.desc: async func run
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaProxyTest002, TestSize.Level1)
{
    std::cout << "IdlSaProxyTest002 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    if (client_ == nullptr) {
        std::cout << "client is nullptr" << std::endl;
        return;
    }

    // async func
    int32_t ret = client_->LoadSystemAbility(3);
    client_->AddVolume(100);
    EXPECT_EQ(ret, ERR_OK);

    std::cout << "AddVolume client" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlSaRemoveDeathTest001
 * @tc.desc: RemoveDeathRecipient test
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaRemoveDeathTest001, TestSize.Level1)
{
    std::cout << "IdlSaRemoveDeathTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    if (client_ == nullptr) {
        std::cout << "client is nullptr" << std::endl;
        return;
    }

    auto cb = [](bool status) {
        std::cout << "LoadSystemAbility cb status: " << status << std::endl;
    };
    
    auto cbDied = []() {
        std::cout << "LoadSystemAbility cbDied." << std::endl;
    };
    client_->RegisterOnRemoteDiedCallback(cbDied);
    int32_t ret = client_->LoadSystemAbility(cb);
    EXPECT_EQ(ret, ERR_OK);
    
    std::this_thread::sleep_for(std::chrono::minutes(2));
    delete client_;
}

/*
 * @tc.name: IdlSaLoadTest001
 * @tc.desc: sync load sa
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaLoadTest001, TestSize.Level1)
{
    std::cout << "IdlSaLoadTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    if (client_ == nullptr) {
        std::cout << "client is nullptr" << std::endl;
        return;
    }

    int32_t ret = client_->LoadSystemAbility(3);
    EXPECT_EQ(ret, ERR_OK);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlSaLoadTest002
 * @tc.desc: async load sa
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaLoadTest002, TestSize.Level1)
{
    std::cout << "IdlSaLoadTest002 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    if (client_ == nullptr) {
        std::cout << "client is nullptr" << std::endl;
        return;
    }

    auto cb = [](bool status) {
        std::cout << "LoadSystemAbility cb status: " << status << std::endl;
    };
    
    int32_t ret = client_->LoadSystemAbility(cb);
    EXPECT_EQ(ret, ERR_OK);
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}



} // namespace idl
} // namespace OHOS