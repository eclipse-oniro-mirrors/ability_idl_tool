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
#include "ast/ast_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

class AstNodeUnitTest : public testing::Test {
public:
    AstNodeUnitTest() {}

    virtual ~AstNodeUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstNodeUnitTest::SetUpTestCase() {}

void AstNodeUnitTest::TearDownTestCase() {}

void AstNodeUnitTest::SetUp() {}

void AstNodeUnitTest::TearDown() {}

/*
 * @tc.name: ToStringTest_0100
 * @tc.desc: test ToString in AstNodeUnitTest's ToString.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstNodeUnitTest, ToStringTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstNodeUnitTest, ToStringTest_0100, TestSize.Level1";
    std::shared_ptr<ASTNode> imageASTNode = std::make_shared<ASTNode>();
    String result = imageASTNode->ToString();
    String expectString = "ASTNode";

    ASSERT_STREQ(result, expectString);
}

/*
 * @tc.name: DumpTest_0100
 * @tc.desc: test ToString in AstNodeUnitTest's Dump.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstNodeUnitTest, DumpTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstNodeUnitTest, DumpTest_0100, TestSize.Level1";
    std::shared_ptr<ASTNode> imageASTNode = std::make_shared<ASTNode>();
    String prefix = "test";
    String result = imageASTNode->Dump(prefix);
    String expectString = prefix + "ASTNode";

    ASSERT_STREQ(result, expectString);
}
