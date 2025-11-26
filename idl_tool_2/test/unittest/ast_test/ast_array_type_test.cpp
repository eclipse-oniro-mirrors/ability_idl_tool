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

#include "ast/ast_array_type.h"
#include "ast/ast_interface_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTArrayTypeTest : public testing::Test {
public:
    ASTArrayTypeTest() {}
    virtual ~ASTArrayTypeTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTArrayTypeTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTArrayTypeTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTArrayTypeTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTArrayTypeTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetSignature_001
 * @tc.desc: test ASTArrayType GetSignature
 * @tc.type: FUNC
 */
HWTEST_F(ASTArrayTypeTest, GetSignature_001, Level1)
{
    DTEST_LOG << "GetSignature_001 begin" << std::endl;
    ASTArrayType arrayType;
    EXPECT_EQ(arrayType.GetSignature(), "[");
    AutoPtr<ASTType> type = new ASTInterfaceType();
    arrayType.SetElementType(type);
    EXPECT_EQ(arrayType.GetSignature(), "[L;");
    DTEST_LOG << "GetSignature_001 end" << std::endl;
}

/*
 * @tc.name: HasInnerType_001
 * @tc.desc: test ASTArrayType HasInnerType
 * @tc.type: FUNC
 */
HWTEST_F(ASTArrayTypeTest, HasInnerType_001, Level1)
{
    DTEST_LOG << "HasInnerType_001 begin" << std::endl;
    ASTArrayType arrayType;
    EXPECT_FALSE(arrayType.HasInnerType(TypeKind::TYPE_INTERFACE));
    AutoPtr<ASTType> type = new ASTInterfaceType();
    arrayType.SetElementType(type);
    EXPECT_TRUE(arrayType.HasInnerType(TypeKind::TYPE_INTERFACE));
    EXPECT_FALSE(arrayType.HasInnerType(TypeKind::TYPE_LIST));
    DTEST_LOG << "HasInnerType_001 end" << std::endl;
}

/*
 * @tc.name: ToString_001
 * @tc.desc: test ASTArrayType ToString
 * @tc.type: FUNC
 */
HWTEST_F(ASTArrayTypeTest, ToString_001, Level1)
{
    DTEST_LOG << "ToString_001 begin" << std::endl;
    ASTArrayType arrayType;
    ASTListType listType;
    EXPECT_EQ(arrayType.ToString(), "[]");
    EXPECT_EQ(listType.ToString(), "List<>");
    AutoPtr<ASTType> type = new ASTInterfaceType();
    arrayType.SetElementType(type);
    listType.SetElementType(type);
    EXPECT_EQ(arrayType.ToString(), "[]");
    EXPECT_EQ(listType.ToString(), "List<>");
    DTEST_LOG << "ToString_001 end" << std::endl;
}
} // namespace OHOS::idl