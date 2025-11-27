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
#include "ast/ast_map_type.h"
#include "ast/base/ast_integer_type.h"
#include "ast/base/ast_short_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTMapTypeTest : public testing::Test {
public:
    ASTMapTypeTest() {}
    virtual ~ASTMapTypeTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTMapTypeTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTMapTypeTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTMapTypeTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTMapTypeTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetSignature_001
 * @tc.desc: test ASTMapType GetSignature
 * @tc.type: FUNC
 */
HWTEST_F(ASTMapTypeTest, GetSignature_001, Level1)
{
    DTEST_LOG << "GetSignature_001 begin" << std::endl;
    ASTMapType mapType;
    AutoPtr<ASTType> arrayType1 = new ASTArrayType();
    AutoPtr<ASTType> arrayType2 = new ASTArrayType();
    auto ret = mapType.GetSignature();
    EXPECT_EQ(ret, "LMap[];");

    mapType.SetKeyType(arrayType1);
    ret = mapType.GetSignature();
    EXPECT_EQ(ret, "LMap[];");

    mapType.SetKeyType(nullptr);
    mapType.SetValueType(arrayType2);
    ret = mapType.GetSignature();
    EXPECT_EQ(ret, "LMap[];");

    mapType.SetKeyType(arrayType1);
    ret = mapType.GetSignature();
    EXPECT_EQ(ret, "LMap[[[];");
    DTEST_LOG << "GetSignature_001 end" << std::endl;
}

/*
 * @tc.name: HasInnerType_001
 * @tc.desc: test ASTMapType HasInnerType
 * @tc.type: FUNC
 */
HWTEST_F(ASTMapTypeTest, HasInnerType_001, Level1)
{
    DTEST_LOG << "HasInnerType_001 begin" << std::endl;
    ASTMapType mapType;
    AutoPtr<ASTType> arrayType1 = new ASTArrayType();
    AutoPtr<ASTType> arrayType2 = new ASTArrayType();
    auto ret = mapType.HasInnerType(TypeKind::TYPE_INT);
    EXPECT_FALSE(ret);

    mapType.SetValueType(arrayType2);
    ret = mapType.HasInnerType(TypeKind::TYPE_INT);
    EXPECT_FALSE(ret);

    AutoPtr<ASTArrayType> parentType2 = static_cast<ASTArrayType *>(arrayType2.Get());
    AutoPtr<ASTType>  integerType = new ASTIntegerType();
    parentType2->SetElementType(integerType);
    ret = mapType.HasInnerType(TypeKind::TYPE_INT);
    EXPECT_TRUE(ret);

    ret = mapType.HasInnerType(TypeKind::TYPE_ARRAY);
    EXPECT_TRUE(ret);

    mapType.SetKeyType(arrayType1);
    ret = mapType.HasInnerType(TypeKind::TYPE_SHORT);
    EXPECT_FALSE(ret);

    AutoPtr<ASTArrayType> parentType1 = static_cast<ASTArrayType *>(arrayType1.Get());
    AutoPtr<ASTType>  shortType = new ASTShortType();
    parentType1->SetElementType(shortType);
    ret = mapType.HasInnerType(TypeKind::TYPE_SHORT);
    EXPECT_TRUE(ret);

    ret = mapType.HasInnerType(TypeKind::TYPE_ARRAY);
    EXPECT_TRUE(ret);
    DTEST_LOG << "HasInnerType_001 end" << std::endl;
}
} // namespace OHOS::Idl