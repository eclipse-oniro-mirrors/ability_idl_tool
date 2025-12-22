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

#include "gtest/gtest.h"
#include "test_log.h"

#define private public
#include "ast/ast_interface_type.h"
#include "ast/base/ast_integer_type.h"
#include "ast/ast_method.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTMethodTest : public testing::Test {
public:
    ASTMethodTest() {}
    virtual ~ASTMethodTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTMethodTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTMethodTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTMethodTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTMethodTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: CheckOverload_001
 * @tc.desc: test ASTMethod CheckOverload
 * @tc.type: FUNC
 */
HWTEST_F(ASTMethodTest, CheckOverload_001, Level1)
{
    DTEST_LOG << "CheckOverload_001 begin" << std::endl;
    ASTMethod method;
    method.SetName("test1");
    method.CheckOverload(nullptr);
    EXPECT_FALSE(method.isOverload_);

    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();
    method.CheckOverload(interfaceType);
    EXPECT_FALSE(method.isOverload_);

    AutoPtr<ASTInterfaceType> extendsInterface1 = new ASTInterfaceType();
    auto ret = interfaceType->AddExtendsInterface(extendsInterface1);
    EXPECT_TRUE(ret);
    method.CheckOverload(interfaceType);
    EXPECT_FALSE(method.isOverload_);

    interfaceType->extendsInterface_ = nullptr;
    AutoPtr<ASTInterfaceType> extendsInterface2 = new ASTInterfaceType();
    AutoPtr<ASTMethod> toIntfaceMethod = new ASTMethod();
    toIntfaceMethod->SetName("test1");
    toIntfaceMethod->attr_->SetValue(ASTAttr::MINI);
    extendsInterface2->AddMethod(toIntfaceMethod);
    ret = interfaceType->AddExtendsInterface(extendsInterface2);
    EXPECT_TRUE(ret);
    ret = Options::GetInstance().SetSystemLevel("mini");
    EXPECT_TRUE(ret);
    method.CheckOverload(interfaceType);
    EXPECT_TRUE(method.isOverload_);
    DTEST_LOG << "CheckOverload_001 end" << std::endl;
}

/*
 * @tc.name: GetSignature_001
 * @tc.desc: test ASTMethod GetSignature
 * @tc.type: FUNC
 */
HWTEST_F(ASTMethodTest, GetSignature_001, Level1)
{
    DTEST_LOG << "GetSignature_001 begin" << std::endl;
    ASTMethod method;
    AutoPtr<ASTType> type = new ASTIntegerType();
    method.SetReturnType(type);
    auto ret = method.GetSignature();
    EXPECT_EQ(ret, "(I)");
    method.AddParameter(nullptr);
    EXPECT_EQ(method.GetParameter(0), nullptr);
    AutoPtr<ASTParameter> parameter = new ASTParameter("minorVer", ASTParamAttr::PARAM_OUT, type);
    method.AddParameter(parameter);
    EXPECT_EQ(method.GetParameter(0), parameter);
    ret = method.GetSignature();
    EXPECT_EQ(ret, "(I)");
    DTEST_LOG << "GetSignature_001 end" << std::endl;
}

/*
 * @tc.name: Dump_001
 * @tc.desc: test ASTMethod GetParameter
 * @tc.type: FUNC
 */
HWTEST_F(ASTMethodTest, Dump_001, Level1)
{
    DTEST_LOG << "Dump_001 begin" << std::endl;
    ASTMethod method;
    EXPECT_EQ(method.Dump("prefix"), "();\n");
    AutoPtr<ASTType> type = new ASTIntegerType();
    method.SetReturnType(type);
    EXPECT_EQ(method.Dump("prefix"), "prefixint ();\n");
    AutoPtr<ASTParameter> parameter1 = new ASTParameter("minorVer", ASTParamAttr::PARAM_OUT, type);
    AutoPtr<ASTParameter> parameter2 = new ASTParameter("majorVer", ASTParamAttr::PARAM_OUT, type);
    method.AddParameter(parameter1);
    method.AddParameter(parameter2);
    EXPECT_EQ(method.Dump("prefix"), "prefixint (\n"
        "prefix    [out] int minorVer,\n"
        "prefix    [out] int majorVer);\n");
    DTEST_LOG << "Dump_001 end" << std::endl;
}
} // namespace OHOS::Idl
