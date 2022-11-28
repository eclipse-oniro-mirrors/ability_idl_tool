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
#include "ast/ast_interface_type.h"
#include "ast/ast_method.h"
#include "ast/ast_type.h"
#include "ast/ast_namespace.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class AstInterfaceTypeUnitTest : public testing::Test {
public:
    AstInterfaceTypeUnitTest() {}

    virtual ~AstInterfaceTypeUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstInterfaceTypeUnitTest::SetUpTestCase() {}

void AstInterfaceTypeUnitTest::TearDownTestCase() {}

void AstInterfaceTypeUnitTest::SetUp() {}

void AstInterfaceTypeUnitTest::TearDown() {}

/*
 * @tc.name: GetMethodTest_0100
 * @tc.desc: test GetMethod in AstInterfaceTypeUnitTest's GetMethod.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(AstInterfaceTypeUnitTest, GetMethodTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstInterfaceTypeUnitTest, GetMethodTest_0100, TestSize.Level1";
    std::shared_ptr<ASTInterfaceType> imageASTInterfaceType = std::make_shared<ASTInterfaceType>();
    size_t index = 3;
    AutoPtr<ASTMethod> result = imageASTInterfaceType->GetMethod(index);
    
    EXPECT_EQ(result, nullptr);
}
} // namespace idl
} // namespace OHOS
