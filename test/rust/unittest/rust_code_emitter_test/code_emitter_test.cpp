/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include "idl_common.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl::TestCommon;

namespace OHOS {
namespace Idl {
namespace UnitTest {
class RustCodeEmitterTest : public testing::Test, public IdlCommon {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RustCodeEmitterTest::SetUpTestCase()
{}

void RustCodeEmitterTest::TearDownTestCase()
{}

void RustCodeEmitterTest::SetUp()
{}

void RustCodeEmitterTest::TearDown()
{}

/*
 * Feature: idl
 * Function: RustCodeEmitter
 * SubFunction: NA
 * FunctionPoints: Check whether the emitter is existed.
 * EnvConditions: NA
 * CaseDescription: RustCodeEmitter is existed.
 */
HWTEST_F(RustCodeEmitterTest, IDLRust_Generator_0100, TestSize.Level1)
{
    EXPECT_EQ(PrepareIdlFile(UNKNOW_TYPE_IDL_NAME.c_str(), UNKNOW_TYPE_CASE1_IDL_CONTENT.c_str()), ERR_OK);
    int argc = 6;
    const char* argvArray[] = {"./idl", "-c", UNKNOW_TYPE_IDL_NAME.c_str(), "-gen-rust", "-d", "."};
    ParameterArgv parameters(argvArray, argc);
    EXPECT_EQ(Ready(argc, parameters.GetArgv()), ERR_OK);
    EXPECT_NE(rustCodeGen_, nullptr);
    StringBuilder sb;
    rustCodeGen_->EmitInterface(sb);
    EXPECT_FALSE(sb.ToString().IsEmpty());
    EXPECT_NE(sb.ToString().IndexOf("Copyright"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("use ipc_rust::"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("FIRST_CALL_TRANSACTION"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("define_remote_object"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IRemoteBroker"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("on_remote_request"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IdlTestStub"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IdlTestProxy"), ERR_FAIL);
}
}  // namespace UnitTest
}  // namespace Idl
}  // namespace OHOS
