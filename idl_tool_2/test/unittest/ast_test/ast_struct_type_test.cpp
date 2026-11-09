/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ast/ast_struct_type.h"
#include "ast/base/ast_boolean_type.h"
#include "ast/base/ast_integer_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTStructTypeTest : public testing::Test {
public:
    ASTStructTypeTest() {}
    virtual ~ASTStructTypeTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTStructTypeTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTStructTypeTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTStructTypeTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTStructTypeTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: NeedNormalize_001
 * @tc.desc: test ASTStructType NeedNormalize default value
 * @tc.type: FUNC
 */
HWTEST_F(ASTStructTypeTest, NeedNormalize_001, Level1)
{
    DTEST_LOG << "NeedNormalize_001 begin" << std::endl;
    ASTStructType structType;
    EXPECT_FALSE(structType.NeedNormalize());

    AutoPtr<ASTType> integerType = new ASTIntegerType();
    structType.AddMember(integerType, "intMember");
    EXPECT_FALSE(structType.NeedNormalize());
    DTEST_LOG << "NeedNormalize_001 end" << std::endl;
}

/*
 * @tc.name: AddMember_001
 * @tc.desc: test ASTStructType AddMember with boolean member
 * @tc.type: FUNC
 */
HWTEST_F(ASTStructTypeTest, AddMember_001, Level1)
{
    DTEST_LOG << "AddMember_001 begin" << std::endl;
    ASTStructType structType;
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType.AddMember(booleanType, "boolMember");
    EXPECT_EQ(structType.GetMemberNumber(), 1);
    EXPECT_EQ(structType.GetMemberName(0), "boolMember");
    EXPECT_TRUE(structType.NeedNormalize());
    DTEST_LOG << "AddMember_001 end" << std::endl;
}

/*
 * @tc.name: AddMember_002
 * @tc.desc: test ASTStructType AddMember with nested struct member
 * @tc.type: FUNC
 */
HWTEST_F(ASTStructTypeTest, AddMember_002, Level1)
{
    DTEST_LOG << "AddMember_002 begin" << std::endl;
    AutoPtr<ASTStructType> plainStruct = new ASTStructType();
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    plainStruct->AddMember(integerType, "intMember");

    ASTStructType outerStruct1;
    AutoPtr<ASTType> plainType = static_cast<ASTType *>(plainStruct.Get());
    outerStruct1.AddMember(plainType, "plainMember");
    EXPECT_FALSE(outerStruct1.NeedNormalize());

    AutoPtr<ASTStructType> boolStruct = new ASTStructType();
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    boolStruct->AddMember(booleanType, "boolMember");

    ASTStructType outerStruct2;
    AutoPtr<ASTType> boolType = static_cast<ASTType *>(boolStruct.Get());
    outerStruct2.AddMember(boolType, "boolStructMember");
    EXPECT_TRUE(outerStruct2.NeedNormalize());
    DTEST_LOG << "AddMember_002 end" << std::endl;
}

/*
 * @tc.name: AddMember_003
 * @tc.desc: test ASTStructType AddMember with duplicate member name
 * @tc.type: FUNC
 */
HWTEST_F(ASTStructTypeTest, AddMember_003, Level1)
{
    DTEST_LOG << "AddMember_003 begin" << std::endl;
    ASTStructType structType;
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    structType.AddMember(integerType, "member");
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType.AddMember(booleanType, "member");
    EXPECT_EQ(structType.GetMemberNumber(), 1);
    EXPECT_FALSE(structType.NeedNormalize());

    structType.AddMember(booleanType, "otherMember");
    EXPECT_EQ(structType.GetMemberNumber(), 2);
    EXPECT_TRUE(structType.NeedNormalize());
    DTEST_LOG << "AddMember_003 end" << std::endl;
}

/*
 * @tc.name: SetParentType_001
 * @tc.desc: test ASTStructType SetParentType propagate NeedNormalize flag
 * @tc.type: FUNC
 */
HWTEST_F(ASTStructTypeTest, SetParentType_001, Level1)
{
    DTEST_LOG << "SetParentType_001 begin" << std::endl;
    ASTStructType childStruct;
    AutoPtr<ASTStructType> nullParent = nullptr;
    childStruct.SetParentType(nullParent);
    EXPECT_EQ(childStruct.GetMemberNumber(), 0);

    AutoPtr<ASTStructType> plainParent = new ASTStructType();
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    plainParent->AddMember(integerType, "intMember");
    childStruct.SetParentType(plainParent);
    EXPECT_EQ(childStruct.GetMemberNumber(), 1);
    EXPECT_FALSE(childStruct.NeedNormalize());

    AutoPtr<ASTStructType> boolParent = new ASTStructType();
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    boolParent->AddMember(booleanType, "boolMember");
    ASTStructType childStruct2;
    childStruct2.SetParentType(boolParent);
    EXPECT_EQ(childStruct2.GetMemberNumber(), 1);
    EXPECT_TRUE(childStruct2.NeedNormalize());
    DTEST_LOG << "SetParentType_001 end" << std::endl;
}
} // namespace OHOS::Idl
