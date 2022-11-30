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
#include "ast/ast_namespace.h"

#include "ast/ast_interface_type.h"
#include "ast/ast_sequenceable_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class AstNamespaceUnitTest : public testing::Test {
public:
    AstNamespaceUnitTest() {}

    virtual ~AstNamespaceUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstNamespaceUnitTest::SetUpTestCase() {}

void AstNamespaceUnitTest::TearDownTestCase() {}

void AstNamespaceUnitTest::SetUp() {}

void AstNamespaceUnitTest::TearDown() {}

/*
 * @tc.name: NamespaceTest_0100
 * @tc.desc: test function in ASTNamespace.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstNamespaceUnitTest, NamespaceTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstNamespaceUnitTest, NamespaceTest_0100, TestSize.Level1";
    String nspaceStr = "";
    size_t index = 3;
    AutoPtr<ASTNamespace> nspace = new ASTNamespace(nspaceStr);
    AutoPtr<ASTNamespace> findResult = nspace->FindNamespace(nspaceStr);
    AutoPtr<ASTNamespace> getResult = nspace->GetNamespace(index);
    AutoPtr<ASTInterfaceType> getInterfaceResult = nspace->GetInterface(index);
    AutoPtr<ASTSequenceableType> getSequenceableResult = nspace->GetSequenceable(index);
    EXPECT_EQ(findResult, nullptr);
    EXPECT_EQ(getResult, nullptr);
    EXPECT_EQ(getInterfaceResult, nullptr);
    EXPECT_EQ(getSequenceableResult, nullptr);
}
} // namespace idl
} // namespace OHOS
