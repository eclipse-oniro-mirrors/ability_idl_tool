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
#include "ast/ast_parameter.h"
#include "ast/ast_void_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

class AstParameterUnitTest : public testing::Test {
public:
    AstParameterUnitTest() {}

    virtual ~AstParameterUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstParameterUnitTest::SetUpTestCase() {}

void AstParameterUnitTest::TearDownTestCase() {}

void AstParameterUnitTest::SetUp() {}

void AstParameterUnitTest::TearDown() {}

/*
 * @tc.name: DumpTest_0100
 * @tc.desc: test if isInParameter in AstParameterUnitTest's Dump function is true.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstParameterUnitTest, DumpTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstParameterUnitTest, DumpTest_0100, TestSize.Level1";
    std::shared_ptr<ASTParameter> imageASTParameter = std::make_shared<ASTParameter>();
    bool inParameter = true;
    bool outParameter = true;
    ASTType *imageASTType = new ASTVoidType();
    String name = "test";
    imageASTParameter->SetInParameter(inParameter);
    imageASTParameter->SetOutParameter(outParameter);
    imageASTParameter->SetType(imageASTType);
    imageASTParameter->SetName(name);
    String prefix = "abctest";
    String result = imageASTParameter->Dump(prefix);
    String typeString = imageASTParameter->GetType()->ToString();
    String expectString = prefix + "[in, out] " + typeString + " " + name;

    ASSERT_STREQ(result, expectString);
}

/*
 * @tc.name: DumpTest_0200
 * @tc.desc: test if isInParameter in AstParameterUnitTest's Dump function is false, isOutParameter is true.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstParameterUnitTest, DumpTest_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstParameterUnitTest, DumpTest_0200, TestSize.Level1";
    std::shared_ptr<ASTParameter> imageASTParameter = std::make_shared<ASTParameter>();
    bool inParameter = false;
    bool outParameter = true;
    ASTType *imageASTType = new ASTVoidType();
    String name = "test";
    imageASTParameter->SetInParameter(inParameter);
    imageASTParameter->SetOutParameter(outParameter);
    imageASTParameter->SetType(imageASTType);
    imageASTParameter->SetName(name);
    String prefix = "abctest";
    String result = imageASTParameter->Dump(prefix);
    String typeString = imageASTParameter->GetType()->ToString();
    String expectString = prefix + "[out] " + typeString + " " + name;
    ASSERT_STREQ(result, expectString);
}
