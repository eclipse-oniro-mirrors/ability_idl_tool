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

#include "util/string_pool.h"
#include "util/string_helper.h"
#include "util/string_builder.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {

static const int TEST_THIRTY = 13;

class IdlTool2UtilStringTest : public testing::Test {
public:
    IdlTool2UtilStringTest() {}

    virtual ~IdlTool2UtilStringTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void IdlTool2UtilStringTest::SetUpTestCase() {}

void IdlTool2UtilStringTest::TearDownTestCase() {}

void IdlTool2UtilStringTest::SetUp() {}

void IdlTool2UtilStringTest::TearDown() {}

/*
 * @tc.name: StringPool_test_001
 * @tc.desc: Verify the StringPool function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringPool_test_001, Level1)
{
    StringPool stringPool_;
    StringPool *stringPoolPtr_ = new StringPool();
    stringPool_.Add("");
    stringPool_.Add("name");
    stringPool_.Add("name");
    stringPool_.Add("bananan");
    ptrdiff_t offset = stringPool_.GetOffset("name");
    size_t size = stringPool_.GetSize();
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(size, TEST_THIRTY);
    delete stringPoolPtr_;
}

/*
 * @tc.name: StringHelper_test_001
 * @tc.desc: Verify the Split StartWith EndWith function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_001, Level1)
{
    std::vector<std::string> result;
    EXPECT_EQ(StringHelper::Split("", ""), result);
    result = {"name"};
    EXPECT_EQ(StringHelper::Split("name", ""), result);
    result = {"n", "e"};
    EXPECT_EQ(StringHelper::Split("name", "am"), result);

    EXPECT_EQ(StringHelper::StartWith("name", 'n'), true);

    EXPECT_EQ(StringHelper::StartWith("name", "m"), false);
    EXPECT_EQ(StringHelper::StartWith("name", "n"), true);

    EXPECT_EQ(StringHelper::EndWith("", 'e'), false);
    EXPECT_EQ(StringHelper::EndWith("name", 'e'), true);

    EXPECT_EQ(StringHelper::EndWith("name", "m"), false);
    EXPECT_EQ(StringHelper::EndWith("name", "me"), true);
}

/*
 * @tc.name: StringHelper_test_002
 * @tc.desc: Verify the Replace function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_002, Level1)
{
    std::string value = "name";
    EXPECT_EQ(StringHelper::Replace("", 'a', 'm'), "");
    EXPECT_EQ(StringHelper::Replace(value, 'a', 'a'), value);
    EXPECT_EQ(StringHelper::Replace(value, 'a', 'm'), "nmme");

    EXPECT_EQ(StringHelper::Replace(value, "a", "m"), "nmme");

    EXPECT_EQ(StringHelper::Replace(value, value.size(), "a", "m"), value);
    EXPECT_EQ(StringHelper::Replace(value, 0, "a", "m"), "nmme");

    EXPECT_EQ(StringHelper::Replace(value, value.size(), 1, "m"), value);
    EXPECT_EQ(StringHelper::Replace(value, 0, 0, "m"), value);
}

/*
 * @tc.name: StringHelper_test_003
 * @tc.desc: Verify the SubStr function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_003, Level1)
{
    std::string value = "name";
    EXPECT_EQ(StringHelper::SubStr("", 0, 0), "");
    EXPECT_EQ(StringHelper::SubStr(value, std::string::npos, 0), "");
    EXPECT_EQ(StringHelper::SubStr(value, 1, 0), "");
    EXPECT_EQ(StringHelper::SubStr(value, 0, std::string::npos), value);
    EXPECT_EQ(StringHelper::SubStr(value, 0, 1), "n");
}

/*
 * @tc.name: StringHelper_test_004
 * @tc.desc: Verify the StrToLower StrToUpper FirstToUpper function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_004, Level1)
{
    std::string valueLow = "name";
    std::string valueUpper = "NAME";
    EXPECT_EQ(StringHelper::StrToLower(valueUpper), valueLow);
    EXPECT_EQ(StringHelper::StrToUpper(valueLow), valueUpper);
    EXPECT_EQ(StringHelper::FirstToUpper(valueLow), "Name");
}

/*
 * @tc.name: StringHelper_test_005
 * @tc.desc: Verify the Format function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_005, Level1)
{
    std::string value = "name";
    EXPECT_EQ(StringHelper::Format(""), "");
    EXPECT_EQ(StringHelper::Format("%s", "name"), value);
}

/*
 * @tc.name: StringHelper_test_006
 * @tc.desc: Verify the GetHashCode function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_test_006, Level1)
{
    EXPECT_EQ(StringHelper::GetHashCode(""), 0);
}

/*
 * @tc.name: StringBuilder_test_001
 * @tc.desc: Verify the Append ToString function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringBuilder_test_001, Level1)
{
    StringBuilder sb;
    sb.Append('a');
    EXPECT_EQ(sb.ToString(), "a");
    const char *value = "b";
    sb.Append(value);
    EXPECT_EQ(sb.ToString(), "ab");
    sb.Append("c");
    EXPECT_EQ(sb.ToString(), "abc");
    sb.AppendFormat("%s", "d");
    EXPECT_EQ(sb.ToString(), "abcd");
}
} // namespace idl
} // namespace OHOS