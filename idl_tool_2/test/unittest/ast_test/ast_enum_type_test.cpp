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
#include "ast/ast_enum_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTEnumTypeTest : public testing::Test {
public:
    ASTEnumTypeTest() {}
    virtual ~ASTEnumTypeTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTEnumTypeTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTEnumTypeTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTEnumTypeTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTEnumTypeTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: ASTEnumTypeDump_001
 * @tc.desc: test ASTEnumType Dump
 * @tc.type: FUNC
 */
HWTEST_F(ASTEnumTypeTest, ASTEnumTypeDump_001, Level1)
{
    DTEST_LOG << "ASTEnumTypeDump_001 begin" << std::endl;
    ASTEnumType enumType;
    AutoPtr<ASTType> arrayType = new ASTArrayType();
    AutoPtr<ASTType> enumType2 = new ASTEnumType();
    enumType.SetBaseType(nullptr);
    auto ret = enumType.Dump("");
    EXPECT_EQ(ret, "[] enum  {\n};\n");
    enumType.SetBaseType(arrayType);
    ret = enumType.Dump("");
    EXPECT_EQ(ret, "[] enum  : [] {\n};\n");
    enumType.SetBaseType(enumType2);
    ret = enumType.Dump("");
    EXPECT_EQ(ret, "[] enum  {\n};\n");
    AutoPtr<ASTEnumValue> enumValue = new ASTEnumValue("test");
    AutoPtr<ASTEnumType> parentEnumType = static_cast<ASTEnumType *>(enumType2.Get());
    parentEnumType->AddMember(enumValue);
    parentEnumType->AddMember(enumValue);
    enumType.SetBaseType(enumType2);
    ret = enumType.Dump("");
    EXPECT_EQ(ret, "[] enum  {\n  test,\n};\n");
    enumValue->SetExprValue(new ASTNumExpr());
    ret = enumType.Dump("");
    EXPECT_EQ(ret, "[] enum  {\n  test = ,\n};\n");
    DTEST_LOG << "ASTEnumTypeDump_001 end" << std::endl;
}
} // namespace OHOS::idl