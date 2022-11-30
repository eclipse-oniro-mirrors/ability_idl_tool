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
#include "ast/ast_module.h"
#include "ast/ast_namespace.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class AstModuleUnitTest : public testing::Test {
public:
    AstModuleUnitTest() {}

    virtual ~AstModuleUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void AstModuleUnitTest::SetUpTestCase() {}

void AstModuleUnitTest::TearDownTestCase() {}

void AstModuleUnitTest::SetUp() {}

void AstModuleUnitTest::TearDown() {}

/*
 * @tc.name: GetParameterTest_0100
 * @tc.desc: test GetParameter in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, GetNamespaceTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, GetNamespaceTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    size_t index = 3;
    AutoPtr<ASTNamespace> result = module->GetNamespace(index);

    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: GetInterfaceTest_0100
 * @tc.desc: test GetInterface in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, GetInterfaceTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, GetInterfaceTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    size_t index = 3;
    AutoPtr<ASTInterfaceType> result = module->GetInterface(index);

    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: IndexOfTest_0100
 * @tc.desc: test IndexOf in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, IndexOfTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, IndexOfTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    ASTInterfaceType* interface = new ASTInterfaceType();
    int result = module->IndexOf(interface);

    EXPECT_EQ(result, -1);
}

/*
 * @tc.name: GetSequenceableTest_0100
 * @tc.desc: test GetSequenceable in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, GetSequenceableTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, GetSequenceableTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    size_t index = 3;
    AutoPtr<ASTSequenceableType> result = module->GetSequenceable(index);

    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: IndexOfTest_0200
 * @tc.desc: test IndexOf in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, IndexOfTest_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, IndexOfTest_0200, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    ASTSequenceableType* sequenceable = new ASTSequenceableType();
    int result = module->IndexOf(sequenceable);

    EXPECT_EQ(result, -1);
}

/*
 * @tc.name: FindTypeTest_0100
 * @tc.desc: test IndexOf in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, FindTypeTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, FindTypeTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    String typeName = "";
    AutoPtr<ASTType> result = module->FindType(typeName);

    EXPECT_EQ(result, nullptr);
}

/*
 * @tc.name: IndexOfTest_0300
 * @tc.desc: test IndexOf in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, IndexOfTest_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, IndexOfTest_0300, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();
    int result = module->IndexOf(interfaceType);

    EXPECT_EQ(result, -1);
}

/*
 * @tc.name: IsValidTest_0100
 * @tc.desc: test IsValid in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, IsValidTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, IsValidTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    bool result = module->IsValid();

    EXPECT_EQ(result, false);
}

/*
 * @tc.name: DumpTest_0100
 * @tc.desc: test Dump in AstModuleUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstModuleUnitTest, DumpTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstModuleUnitTest, DumpTest_0100, TestSize.Level1";
    AutoPtr<ASTModule> module = new ASTModule();
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();

    String prefix = "prefix";
    String idlFile = "idlFile";

    ASTSequenceableType* sequenceable = new ASTSequenceableType();
    ASTInterfaceType* interface = new ASTInterfaceType();
    interface->SetExternal(true);

    module->SetIdlFile(idlFile);
    module->AddSequenceable(sequenceable);
    module->AddInterface(interface);

    String result = module->Dump(prefix);
    String expectResult = prefix + "Module[" + "name: " + " " + "file: " + idlFile + "]\n"
      + "  sequenceable " + ";\n" + "\n" + "  interface " + ";\n" + "\n";

    ASSERT_STREQ(result, expectResult);
}
} // namespace idl
} // namespace OHOS
