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
#include <vector>

#include <gtest/gtest.h>
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "listen_ability_client.h"
#include "my_rawdata.h"

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
    ASSERT_NE(client_, nullptr);

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
    ASSERT_NE(client_, nullptr);

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
    ASSERT_NE(client_, nullptr);

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
    ASSERT_NE(client_, nullptr);

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
    ASSERT_NE(client_, nullptr);

    auto cb = [](bool status) {
        std::cout << "LoadSystemAbility cb status: " << status << std::endl;
    };

    int32_t ret = client_->LoadSystemAbility(cb);
    EXPECT_EQ(ret, ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlOverLoadTest001
 * @tc.desc: async load sa
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlOverLoadTest001, TestSize.Level1)
{
    std::cout << "IdlOverLoadTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    ASSERT_NE(client_, nullptr);

    std::unordered_map<int32_t, int32_t> outApp;
    int32_t outParam = 9;
    outApp[4] = 11;
    int32_t ret = client_->overloadfun(outParam);
#ifdef DEVICE
    ret = client_->overloadfun(outApp);
#endif
    std::cout << "TestOverLoad" << std::endl;
    EXPECT_EQ(outParam, 10);
    EXPECT_EQ(outApp[4], 7);
    EXPECT_EQ(ret, ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlSaCustomTest001
 * @tc.desc: async load sa
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlSaCustomTest001, TestSize.Level1)
{
    std::cout << "IdlSaCustomTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    ASSERT_NE(client_, nullptr);

    FooEnum in1 = FooEnum::ENUM_ONE, out1 = FooEnum::ENUM_ONE, inout1 = FooEnum::ENUM_ONE, result1 = FooEnum::ENUM_ONE;
    int32_t ret = client_->enum_test_func(in1, out1, inout1, result1);
    std::cout << "TestSaCallSa enum_test_func" << std::endl;
    EXPECT_EQ(static_cast<int>(out1), 2);
    EXPECT_EQ(ret, ERR_OK);

    FooStruct in2 = { 1, "ExampleName", FooEnum::ENUM_ONE };
    FooStruct inout2 = { 2, "AnotherName", FooEnum::ENUM_TWO };
    FooStruct result2 = { 3, "ResultName", FooEnum::ENUM_NESTING};
    RequestInfo out2 = {
        {1, 2},
        {{"key1", "value1"}, {"key2", "value2"}}  // optionalData
    };

    ret = client_->struct_test_func(in2, out2, inout2, result2);
    std::cout << "TestSaCallSa struct_test_func" << std::endl;
    EXPECT_EQ(out2.initData[2], 3);
    EXPECT_EQ(ret, ERR_OK);

    FooUnion in3, out3, inout3, result3;
    in3.enumType = FooEnum::ENUM_ONE;
    ret = client_->union_test_func(in3, out3, inout3, result3);
    std::cout << "TestSaCallSa union_test_func" << std::endl;
    EXPECT_EQ(static_cast<int>(out3.enumType), 2);
    EXPECT_EQ(ret, ERR_OK);

    std::vector<std::string> quick;
    bool isTrue = true;
    ret = client_->ApplyQuickFix(quick, isTrue);
    std::cout << "TestSaCallSa ApplyQuickFix" << std::endl;
    EXPECT_EQ(ret, ERR_OK);

    std::unordered_map<int32_t, FooStruct> inApp, outApp;
    inApp[0].id = 11;
    inApp[0].name = "shiyi";
    ret = client_->GetAllAppSuspendState(inApp, outApp);
    std::cout << "TestSaCallSa GetAllAppSuspendState" << std::endl;
    EXPECT_EQ(outApp[1].id, 999);
    EXPECT_EQ(outApp[1].name, "MapTest");
    EXPECT_EQ(ret, ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlSaCustomTest001
 * @tc.desc: async load sa
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlRawDataTest001, TestSize.Level1)
{
    std::cout << "IdlRawDataTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    ASSERT_NE(client_, nullptr);

    MyRawdata in, out, inout, result;
    const char sampleData[] = "Hello!";

    in.size = sizeof(sampleData);
    in.RawDataCpy(sampleData);
    out.size = sizeof(sampleData);
    out.RawDataCpy(sampleData);
    inout.size = sizeof(sampleData);
    inout.RawDataCpy(sampleData);
    int32_t ret = client_->rawdata_test_func(in, out, inout, result);
    std::cout << "TestRawData" << std::endl;

    EXPECT_STREQ(static_cast<const char*>(out.data), "Hello, world!");
    EXPECT_EQ(out.size, 14);
    EXPECT_STREQ(static_cast<const char*>(inout.data), "world!");
    EXPECT_EQ(inout.size, 7);
    EXPECT_EQ(ret, ERR_OK);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

/*
 * @tc.name: IdlfdTest001
 * @tc.desc: test fd
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTest, IdlfdTest001, TestSize.Level1)
{
    std::cout << "IdlfdTest001 start" << std::endl;

    ListenAbilityClient* client_ = new ListenAbilityClient(1494);
    ASSERT_NE(client_, nullptr);

    // sync func
    int fd;
    int32_t ret = client_->fd_test_func(fd);
    std::cout << "fd_test_func client" << std::endl;
    EXPECT_EQ(ret, ERR_OK);

    if (fd >= 0) {
        close(fd);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete client_;
}

} // namespace idl
} // namespace OHOS