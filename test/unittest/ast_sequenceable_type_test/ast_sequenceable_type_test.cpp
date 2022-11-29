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
#include "ast/ast_sequenceable_type.h"
#include "ast/ast_namespace.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class ASTSequenceableTypeUnitTest : public testing::Test {
public:
    ASTSequenceableTypeUnitTest() {}

    virtual ~ASTSequenceableTypeUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void ASTSequenceableTypeUnitTest::SetUpTestCase() {}

void ASTSequenceableTypeUnitTest::TearDownTestCase() {}

void ASTSequenceableTypeUnitTest::SetUp() {}

void ASTSequenceableTypeUnitTest::TearDown() {}

/*
 * @tc.name: DumpTest_0100
 * @tc.desc: test Dump in ASTSequenceableTypeUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ASTSequenceableTypeUnitTest, DumpTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "ASTSequenceableTypeUnitTest, DumpTest_0100, TestSize.Level1";
    AutoPtr<ASTSequenceableType> sequenceable = new ASTSequenceableType();
    String prefix = "prefix";
    String name = "name";
    sequenceable->SetName(name);
    String result = sequenceable->Dump(prefix);

    String expectResult = prefix + "sequenceable " + name + ";\n";

    ASSERT_STREQ(result, expectResult);
}

/*
 * @tc.name: DumpTest_0200
 * @tc.desc: test Dump in ASTSequenceableTypeUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ASTSequenceableTypeUnitTest, DumpTest_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "ASTSequenceableTypeUnitTest, DumpTest_0200, TestSize.Level1";
    AutoPtr<ASTSequenceableType> sequenceable = new ASTSequenceableType();

    String prefix = "prefix";
    String name = "name";
    String nspaceStr = "nspaceStr";

    ASTNamespace* nspace  = new ASTNamespace(nspaceStr);
    sequenceable->SetName(name);
    sequenceable->SetNamespace(nspace);
    String result = sequenceable->Dump(prefix);
    String expectResult = prefix + "sequenceable " + nspace->ToString() + name + ";\n";

    ASSERT_STREQ(result, expectResult);
}
} // namespace idl
} // namespace OHOS
