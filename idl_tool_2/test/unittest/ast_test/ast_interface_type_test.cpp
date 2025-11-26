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

#include "ast/ast_interface_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTInterfaceTypeTest : public testing::Test {
public:
    ASTInterfaceTypeTest() {}
    virtual ~ASTInterfaceTypeTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTInterfaceTypeTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTInterfaceTypeTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTInterfaceTypeTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTInterfaceTypeTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: SetNamespace_001
 * @tc.desc: test ASTInterfaceType SetNamespace
 * @tc.type: FUNC
 */
HWTEST_F(ASTInterfaceTypeTest, SetNamespace_001, Level1)
{
    DTEST_LOG << "SetNamespace_001 begin" << std::endl;
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();
    AutoPtr<ASTNamespace> nspace = new ASTNamespace("com.example.package");
    interfaceType->SetNamespace(nullptr);
    EXPECT_EQ(nspace->GetInterfaceNumber(), 0);
    interfaceType->SetNamespace(nspace);
    EXPECT_EQ(nspace->GetInterfaceNumber(), 1);
    DTEST_LOG << "SetNamespace_001 end" << std::endl;
}

/*
 * @tc.name: AddMethod_001
 * @tc.desc: test ASTInterfaceType AddMethod
 * @tc.type: FUNC
 */
HWTEST_F(ASTInterfaceTypeTest, AddMethod_001, Level1)
{
    DTEST_LOG << "AddMethod_001 begin" << std::endl;
    ASTInterfaceType interfaceType;
    AutoPtr<ASTMethod> method = new ASTMethod();
    interfaceType.AddMethod(nullptr);
    EXPECT_EQ(interfaceType.GetMethod(0), nullptr);
    interfaceType.AddMethod(method);
    EXPECT_NE(interfaceType.GetMethod(0), nullptr);
    DTEST_LOG << "AddMethod_001 end" << std::endl;
}

/*
 * @tc.name: AddExtendsInterface_001
 * @tc.desc: test ASTInterfaceType AddExtendsInterface
 * @tc.type: FUNC
 */
HWTEST_F(ASTInterfaceTypeTest, AddExtendsInterface_001, Level1)
{
    DTEST_LOG << "AddExtendsInterface_001 begin" << std::endl;
    ASTInterfaceType interfaceType;
    AutoPtr<ASTInterfaceType> extendsInterface = new ASTInterfaceType();
    auto ret = interfaceType.AddExtendsInterface(extendsInterface);
    EXPECT_TRUE(ret);
    ret = interfaceType.AddExtendsInterface(nullptr);
    EXPECT_FALSE(ret);
    DTEST_LOG << "AddExtendsInterface_001 end" << std::endl;
}

/*
 * @tc.name: GetMethodsBySystem_001
 * @tc.desc: test ASTInterfaceType GetMethodsBySystem
 * @tc.type: FUNC
 */
HWTEST_F(ASTInterfaceTypeTest, GetMethodsBySystem_001, Level1)
{
    DTEST_LOG << "GetMethodsBySystem_001 begin" << std::endl;
    ASTInterfaceType interfaceType;
    AutoPtr<ASTMethod> method1 = new ASTMethod();
    AutoPtr<ASTMethod> method2 = new ASTMethod();
    AutoPtr<ASTAttr> attr = new ASTAttr(ASTAttr::MINI);
    method1->SetAttribute(attr);
    interfaceType.AddMethod(method1);
    interfaceType.AddMethod(method2);
    auto ret = interfaceType.GetMethodsBySystem(SystemLevel::MINI);
    EXPECT_FALSE(ret.empty());
    DTEST_LOG << "GetMethodsBySystem_001 end" << std::endl;
}

/*
 * @tc.name: Dump_001
 * @tc.desc: test ASTInterfaceType Dump
 * @tc.type: FUNC
 */
HWTEST_F(ASTInterfaceTypeTest, Dump_001, Level1)
{
    DTEST_LOG << "Dump_001 begin" << std::endl;
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();
    AutoPtr<ASTNamespace> nspace = new ASTNamespace("com.example.package");
    interfaceType->SetNamespace(nspace);
    AutoPtr<ASTMethod> method1 = new ASTMethod();
    AutoPtr<ASTMethod> method2 = new ASTMethod();
    interfaceType->AddMethod(method1);
    interfaceType->AddMethod(method2);
    auto ret = interfaceType->Dump("prefix");
    EXPECT_EQ(ret, "prefixinterface com.example.package. {\n();\n\n();\nprefix}\n");
    DTEST_LOG << "Dump_001 end" << std::endl;
}
} // namespace OHOS::idl