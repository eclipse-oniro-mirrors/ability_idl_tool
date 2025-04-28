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
#include "foo_ptr_client.h"
#include "my_seq.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Idl {

class IdlSaUnitTestPtr : public testing::Test {
public:
    IdlSaUnitTestPtr() {}

    virtual ~IdlSaUnitTestPtr() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void IdlSaUnitTestPtr::SetUpTestCase() {}

void IdlSaUnitTestPtr::TearDownTestCase() {}

void IdlSaUnitTestPtr::SetUp() {}

void IdlSaUnitTestPtr::TearDown() {}

/*
 * @tc.name: IdlSaProxyPtrTest001
 * @tc.desc: shared_ptr run
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTestPtr, IdlSaProxyPtrTest001, TestSize.Level1)
{
    std::cout << "IdlSaProxyPtrTest001 start" << std::endl;

    FooPtrClient* client_ = new FooPtrClient(TEST_SAID);
    ASSERT_NE(client_, nullptr);

    // sync func
    MySeq* inParamTmp = new MySeq();
    inParamTmp->size = TEST_IN_SIZE_FIVE;
    std::shared_ptr<MySeq> inParam = std::shared_ptr<MySeq>(inParamTmp);
    MySeq* inoutParamTmp = new MySeq();
    inoutParamTmp->size = TEST_IN_SIZE_SEVEN;
    std::shared_ptr<MySeq> inoutParam = std::shared_ptr<MySeq>(inoutParamTmp);
    std::shared_ptr<MySeq> outParam;
    std::shared_ptr<MySeq> funcResult;
    int32_t ret = client_->sharedptr_seq_func(inParam, inoutParam, outParam, funcResult);
    std::cout << "sharedptr_seq_func client" << std::endl;
    ASSERT_NE(outParam, nullptr);
    ASSERT_NE(funcResult, nullptr);
    EXPECT_EQ(TEST_IN_SIZE_TWENTY, outParam->size);
    EXPECT_EQ(TEST_IN_SIZE_FIFTY, funcResult->size);
    EXPECT_EQ(ret, ERR_OK);
    delete client_;
}

/*
 * @tc.name: IdlSaProxyPtrTest002
 * @tc.desc: unique_ptr run
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTestPtr, IdlSaProxyPtrTest002, TestSize.Level1)
{
    std::cout << "IdlSaProxyPtrTest002 start" << std::endl;

    FooPtrClient* client_ = new FooPtrClient(TEST_SAID);
    ASSERT_NE(client_, nullptr);

    // sync func
    MySeq* inParamTmp = new MySeq();
    inParamTmp->size = TEST_IN_SIZE_FIVE;
    std::unique_ptr<MySeq> inParam = std::unique_ptr<MySeq>(inParamTmp);
    MySeq* inoutParamTmp = new MySeq();
    inoutParamTmp->size = TEST_IN_SIZE_SEVEN;
    std::unique_ptr<MySeq> inoutParam = std::unique_ptr<MySeq>(inoutParamTmp);
    std::unique_ptr<MySeq> outParam;
    std::unique_ptr<MySeq> funcResult;
    int32_t ret = client_->uniqueptr_seq_func(inParam, inoutParam, outParam, funcResult);
    std::cout << "sharedptr_seq_func client" << std::endl;
    ASSERT_NE(outParam, nullptr);
    ASSERT_NE(funcResult, nullptr);
    EXPECT_EQ(TEST_IN_SIZE_TWENTY, outParam->size);
    EXPECT_EQ(TEST_IN_SIZE_FIFTY, funcResult->size);
    EXPECT_EQ(ret, ERR_OK);
    delete client_;
}

/*
 * @tc.name: IdlSaProxyPtrTest003
 * @tc.desc: sptr run
 * @tc.type: FUNC
 */
HWTEST_F(IdlSaUnitTestPtr, IdlSaProxyPtrTest003, TestSize.Level1)
{
    std::cout << "IdlSaProxyPtrTest003 start" << std::endl;

    FooPtrClient* client_ = new FooPtrClient(TEST_SAID);
    ASSERT_NE(client_, nullptr);

    // sync func
    MySeq* inParamTmp = new MySeq();
    inParamTmp->size = TEST_IN_SIZE_FIVE;
    sptr<MySeq> inParam = sptr<MySeq>(inParamTmp);
    MySeq* inoutParamTmp = new MySeq();
    inoutParamTmp->size = TEST_IN_SIZE_SEVEN;
    sptr<MySeq> inoutParam = sptr<MySeq>(inoutParamTmp);
    sptr<MySeq> outParam;
    sptr<MySeq> funcResult;
    int32_t ret = client_->sptr_seq_func(inParam, inoutParam, outParam, funcResult);
    std::cout << "sharedptr_seq_func client" << std::endl;
    ASSERT_NE(outParam, nullptr);
    ASSERT_NE(funcResult, nullptr);
    EXPECT_EQ(TEST_IN_SIZE_TWENTY, outParam->size);
    EXPECT_EQ(TEST_IN_SIZE_FIFTY, funcResult->size);
    EXPECT_EQ(ret, ERR_OK);
    delete client_;
}

} // namespace idl
} // namespace OHOS