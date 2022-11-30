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
#include "ast/ast_namespace.h"
#include "ast/ast_parameter.h"
#include "ast/ast_type.h"
#include "ast/ast_void_type.h"

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
 * @tc.desc: test GetMethod in AstInterfaceTypeUnitTest.
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

/*
 * @tc.name: DumpTest_0100
 * @tc.desc: test Dump in AstInterfaceTypeUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstInterfaceTypeUnitTest, DumpTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstInterfaceTypeUnitTest, DumpTest_0100, TestSize.Level1";
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();

    String prefix = "prefix";
    String name = "name";
    String nspaceStr = "nspaceStr";

    ASTNamespace* nspace  = new ASTNamespace(nspaceStr);
    interfaceType->SetName(name);
    interfaceType->SetNamespace(nspace);
    interfaceType->SetExternal(true);

    String result = interfaceType->Dump(prefix);
    String expectResult = prefix + "interface " + nspace->ToString() + name + ";\n";

    ASSERT_STREQ(result, expectResult);
}

/*
 * @tc.name: DumpTest_0200
 * @tc.desc: test Dump in AstInterfaceTypeUnitTest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AstInterfaceTypeUnitTest, DumpTest_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "AstInterfaceTypeUnitTest, DumpTest_0200, TestSize.Level1";
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();

    String prefix = "prefix";
    String name = "name";
    String methodName0 = "methodName0";
    String methodName1 = "methodName1";
    String parameterName0 = "parameterName0";
    String parameterName1 = "parameterName1";
    String typeName = "typeName";
    String nspaceStr = "nspaceStr";

    ASTNamespace* nspace  = new ASTNamespace(nspaceStr);
    ASTMethod* method0 = new ASTMethod();
    ASTMethod* method1 = new ASTMethod();
    ASTParameter* parameter0 = new ASTParameter();
    ASTParameter* parameter1 = new ASTParameter();
    ASTType *methodType0 = new ASTVoidType();
    ASTType *methodType1 = new ASTVoidType();
    ASTType *parameterType0 = new ASTVoidType();
    ASTType *parameterType1 = new ASTVoidType();

    interfaceType->SetExternal(false);
    interfaceType->SetName(name);
    interfaceType->SetNamespace(nspace);

    method0->SetName(methodName0);
    method1->SetName(methodName1);
    method0->SetReturnType(methodType0);
    method1->SetReturnType(methodType1);
    method0->AddParameter(parameter0);
    method0->AddParameter(parameter1);
    method1->AddParameter(parameter0);
    method1->AddParameter(parameter1);

    parameter0->SetInParameter(false);
    parameter1->SetInParameter(false);
    parameter0->SetOutParameter(false);
    parameter1->SetOutParameter(false);
    parameter0->SetType(parameterType0);
    parameter1->SetType(parameterType1);
    parameter0->SetName(parameterName0);
    parameter1->SetName(parameterName1);

    interfaceType->AddMethod(method0);
    interfaceType->AddMethod(method1);

    String result = interfaceType->Dump(prefix);
    String expectResult = prefix + "interface " + nspace->ToString() + name + " {\n"
      + prefix + "  " + methodType0->ToString() + " " + methodName0 + "(" + "\n"
      + prefix + "    " + "[" + "] " + parameterType0->ToString() + " " + parameterName0 + ",\n"
      + prefix + "    " + "[" + "] " + parameterType1->ToString() + " " + parameterName1 + ");\n" + "\n"
      + prefix + "  " + methodType1->ToString() + " " + methodName1 + "(" + "\n"
      + prefix + "    " + "[" + "] " + parameterType0->ToString() + " " + parameterName0 + ",\n"
      + prefix + "    " + "[" + "] " + parameterType1->ToString() + " " + parameterName1 + ");\n"
      + prefix + "}\n";

    ASSERT_STREQ(result, expectResult);
}
} // namespace idl
} // namespace OHOS
