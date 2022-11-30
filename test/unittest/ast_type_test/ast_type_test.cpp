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
#include "ast/ast_type.h"
#include "ast/ast_void_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

class AstTypeUnitTest : public testing::Test {
public:
    AstTypeUnitTest() {}

    virtual ~AstTypeUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstTypeUnitTest::SetUpTestCase() {}

void AstTypeUnitTest::TearDownTestCase() {}

void AstTypeUnitTest::SetUp() {}

void AstTypeUnitTest::TearDown() {}

/*
 * @tc.name: ToShortStringTest_0100
 * @tc.desc: test ToShortString in AstTypeUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstTypeUnitTest, ToShortStringTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstTypeUnitTest, ToShortStringTest_0100, TestSize.Level1";
    ASTType *type = new ASTVoidType();
    String name = "name";
    type->SetName(name);
    String result = type->ToShortString();
    String expectString = "name";

    ASSERT_STREQ(result, expectString);
}
