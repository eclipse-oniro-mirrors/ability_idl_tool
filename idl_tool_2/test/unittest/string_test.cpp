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
    EXPECT_NE(StringHelper::GetHashCode("name"), 0);
    EXPECT_EQ(StringHelper::GetHashCode("name"), StringHelper::GetHashCode("name"));
}

/*
 * @tc.name: StringHelper_Split_001
 * @tc.desc: Verify the Split function with various delimiters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Split_001, Level1)
{
    // Test split with space delimiter
    std::vector<std::string> result = StringHelper::Split("hello world test", " ");
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "hello");
    EXPECT_EQ(result[1], "world");
    EXPECT_EQ(result[2], "test");

    // Test split with comma delimiter
    result = StringHelper::Split("a,b,c", ",");
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");

    // Test split with multi-char delimiter
    result = StringHelper::Split("a::b::c", "::");
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}

/*
 * @tc.name: StringHelper_Split_002
 * @tc.desc: Verify the Split function with edge cases.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Split_002, Level1)
{
    // Test empty string
    std::vector<std::string> result = StringHelper::Split("", ",");
    EXPECT_TRUE(result.empty());

    // Test string without delimiter
    result = StringHelper::Split("hello", ",");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "hello");

    // Test string with consecutive delimiters
    result = StringHelper::Split("a,,b", ",");
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");

    // Test string starting with delimiter
    result = StringHelper::Split(",hello", ",");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "hello");

    // Test string ending with delimiter
    result = StringHelper::Split("hello,", ",");
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "hello");
}

/*
 * @tc.name: StringHelper_StartWith_001
 * @tc.desc: Verify the StartWith function with char prefix.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_StartWith_001, Level1)
{
    EXPECT_TRUE(StringHelper::StartWith("hello", 'h'));
    EXPECT_FALSE(StringHelper::StartWith("hello", 'e'));
    EXPECT_FALSE(StringHelper::StartWith("", 'h'));
}

/*
 * @tc.name: StringHelper_StartWith_002
 * @tc.desc: Verify the StartWith function with string prefix.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_StartWith_002, Level1)
{
    EXPECT_TRUE(StringHelper::StartWith("hello world", "hello"));
    EXPECT_TRUE(StringHelper::StartWith("hello world", "h"));
    EXPECT_TRUE(StringHelper::StartWith("hello", "hello"));
    EXPECT_FALSE(StringHelper::StartWith("hello world", "world"));
    EXPECT_FALSE(StringHelper::StartWith("hello", "hello world"));
    EXPECT_TRUE(StringHelper::StartWith("", ""));
}

/*
 * @tc.name: StringHelper_EndWith_001
 * @tc.desc: Verify the EndWith function with char suffix.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_EndWith_001, Level1)
{
    EXPECT_TRUE(StringHelper::EndWith("hello", 'o'));
    EXPECT_FALSE(StringHelper::EndWith("hello", 'h'));
    EXPECT_FALSE(StringHelper::EndWith("", 'o'));
}

/*
 * @tc.name: StringHelper_EndWith_002
 * @tc.desc: Verify the EndWith function with string suffix.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_EndWith_002, Level1)
{
    EXPECT_TRUE(StringHelper::EndWith("hello world", "world"));
    EXPECT_TRUE(StringHelper::EndWith("hello", "hello"));
    EXPECT_TRUE(StringHelper::EndWith("hello", "o"));
    EXPECT_FALSE(StringHelper::EndWith("hello world", "hello"));
    EXPECT_FALSE(StringHelper::EndWith("hello", "hello world"));
    EXPECT_TRUE(StringHelper::EndWith("", ""));
}

/*
 * @tc.name: StringHelper_Replace_Char_001
 * @tc.desc: Verify the Replace function with char parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Replace_Char_001, Level1)
{
    // Test basic replace
    EXPECT_EQ(StringHelper::Replace("hello", 'e', 'a'), "hallo");

    // Test replace multiple occurrences
    EXPECT_EQ(StringHelper::Replace("hello", 'l', 'x'), "hexxo");

    // Test replace with same char
    std::string value = "hello";
    EXPECT_EQ(StringHelper::Replace(value, 'e', 'e'), value);

    // Test empty string
    EXPECT_EQ(StringHelper::Replace("", 'a', 'b'), "");

    // Test char not in string
    EXPECT_EQ(StringHelper::Replace("hello", 'x', 'y'), "hello");
}

/*
 * @tc.name: StringHelper_Replace_String_001
 * @tc.desc: Verify the Replace function with string parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Replace_String_001, Level1)
{
    // Test basic string replace
    EXPECT_EQ(StringHelper::Replace("hello world", "world", "there"), "hello there");

    // Test replace multiple occurrences
    EXPECT_EQ(StringHelper::Replace("aa bb aa", "aa", "cc"), "cc bb cc");

    // Test replace with overlapping
    EXPECT_EQ(StringHelper::Replace("aaa", "aa", "b"), "ba");

    // Test replace with empty string
    EXPECT_EQ(StringHelper::Replace("hello", "", "x"), "hello");

    // Test empty source
    EXPECT_EQ(StringHelper::Replace("", "a", "b"), "");
}

/*
 * @tc.name: StringHelper_Replace_Position_001
 * @tc.desc: Verify the Replace function with position parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Replace_Position_001, Level1)
{
    // Test replace from position 0
    EXPECT_EQ(StringHelper::Replace("hello world", 0, "hello", "hi"), "hi world");

    // Test replace from middle position
    EXPECT_EQ(StringHelper::Replace("hello world", 6, "world", "there"), "hello there");

    // Test replace with position out of bounds
    std::string value = "hello";
    EXPECT_EQ(StringHelper::Replace(value, value.size(), "a", "b"), value);

    // Test replace from position with no match
    EXPECT_EQ(StringHelper::Replace("hello world", 0, "xyz", "abc"), "hello world");
}

/*
 * @tc.name: StringHelper_Replace_Length_001
 * @tc.desc: Verify the Replace function with position and length parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Replace_Length_001, Level1)
{
    // Test replace at beginning
    EXPECT_EQ(StringHelper::Replace("hello", 0, 2, "Hi"), "Hillo");

    // Test replace at end
    EXPECT_EQ(StringHelper::Replace("hello", 3, 2, "xyz"), "helxyz");

    // Test replace at middle
    EXPECT_EQ(StringHelper::Replace("hello world", 6, 5, "there"), "hello there");

    // Test with position out of bounds
    std::string value = "hello";
    EXPECT_EQ(StringHelper::Replace(value, value.size(), 1, "x"), value);

    // Test with zero length
    EXPECT_EQ(StringHelper::Replace("hello", 0, 0, "x"), "hello");
}

/*
 * @tc.name: StringHelper_SubStr_001
 * @tc.desc: Verify the SubStr function with various parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_SubStr_001, Level1)
{
    // Test substring from start to end
    EXPECT_EQ(StringHelper::SubStr("hello world", 0, 5), "hello");

    // Test substring to end
    EXPECT_EQ(StringHelper::SubStr("hello world", 6), "world");

    // Test substring from middle
    EXPECT_EQ(StringHelper::SubStr("hello world", 2, 9), "llo wor");

    // Test with start >= end
    EXPECT_EQ(StringHelper::SubStr("hello", 3, 2), "");

    // Test with empty string
    EXPECT_EQ(StringHelper::SubStr("", 0, 1), "");

    // Test with start at npos
    EXPECT_EQ(StringHelper::SubStr("hello", std::string::npos, 1), "");
}

/*
 * @tc.name: StringHelper_StrToLower_001
 * @tc.desc: Verify the StrToLower function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_StrToLower_001, Level1)
{
    EXPECT_EQ(StringHelper::StrToLower("HELLO"), "hello");
    EXPECT_EQ(StringHelper::StrToLower("HeLLo WoRLd"), "hello world");
    EXPECT_EQ(StringHelper::StrToLower("hello"), "hello");
    EXPECT_EQ(StringHelper::StrToLower(""), "");
    EXPECT_EQ(StringHelper::StrToLower("123ABC"), "123abc");
    EXPECT_EQ(StringHelper::StrToLower("Hello123"), "hello123");
}

/*
 * @tc.name: StringHelper_StrToUpper_001
 * @tc.desc: Verify the StrToUpper function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_StrToUpper_001, Level1)
{
    EXPECT_EQ(StringHelper::StrToUpper("hello"), "HELLO");
    EXPECT_EQ(StringHelper::StrToUpper("HeLLo WoRLd"), "HELLO WORLD");
    EXPECT_EQ(StringHelper::StrToUpper("HELLO"), "HELLO");
    EXPECT_EQ(StringHelper::StrToUpper(""), "");
    EXPECT_EQ(StringHelper::StrToUpper("123abc"), "123ABC");
    EXPECT_EQ(StringHelper::StrToUpper("Hello123"), "HELLO123");
}

/*
 * @tc.name: StringHelper_FirstToUpper_001
 * @tc.desc: Verify the FirstToUpper function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_FirstToUpper_001, Level1)
{
    EXPECT_EQ(StringHelper::FirstToUpper("hello"), "Hello");
    EXPECT_EQ(StringHelper::FirstToUpper("HELLO"), "HELLO");
    EXPECT_EQ(StringHelper::FirstToUpper("hELLO"), "HELLO");
    EXPECT_EQ(StringHelper::FirstToUpper(""), "");
    EXPECT_EQ(StringHelper::FirstToUpper("123hello"), "123hello");
}

/*
 * @tc.name: StringHelper_Format_001
 * @tc.desc: Verify the Format function with various format strings.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_Format_001, Level1)
{
    // Test string formatting
    EXPECT_EQ(StringHelper::Format("%s %s", "hello", "world"), "hello world");

    // Test integer formatting
    EXPECT_EQ(StringHelper::Format("%d", 42), "42");

    // Test mixed formatting
    EXPECT_EQ(StringHelper::Format("Value: %d, String: %s", 100, "test"), "Value: 100, String: test");

    // Test empty format
    EXPECT_EQ(StringHelper::Format(""), "");

    // Test format with special characters
    EXPECT_EQ(StringHelper::Format("Hello\nWorld"), "Hello\nWorld");
}

/*
 * @tc.name: StringHelper_GetHashCode_001
 * @tc.desc: Verify the GetHashCode function consistency.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringHelper_GetHashCode_001, Level1)
{
    // Test empty string
    EXPECT_EQ(StringHelper::GetHashCode(""), 0);

    // Test consistency - same input should give same hash
    int hash1 = StringHelper::GetHashCode("hello");
    int hash2 = StringHelper::GetHashCode("hello");
    EXPECT_EQ(hash1, hash2);

    // Test different strings give different hashes
    int hash3 = StringHelper::GetHashCode("world");
    EXPECT_NE(hash1, hash3);

    // Test case sensitivity
    int hash4 = StringHelper::GetHashCode("HELLO");
    EXPECT_NE(hash1, hash4);

    // Test hash is positive (masked to 31 bits)
    int hash5 = StringHelper::GetHashCode("some very long string with many characters");
    EXPECT_GE(hash5, 0);
}

/*
 * @tc.name: StringBuilder_Append_001
 * @tc.desc: Verify the StringBuilder Append function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringBuilder_Append_001, Level1)
{
    StringBuilder sb;

    // Test append char
    sb.Append('H');
    sb.Append('i');
    EXPECT_EQ(sb.ToString(), "Hi");

    // Test append string
    sb.Append(" ");
    sb.Append("World");
    EXPECT_EQ(sb.ToString(), "Hi World");

    // Test append c-string pointer
    const char* cstr = "!";
    sb.Append(cstr);
    EXPECT_EQ(sb.ToString(), "Hi World!");
}

/*
 * @tc.name: StringBuilder_AppendFormat_001
 * @tc.desc: Verify the StringBuilder AppendFormat function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringBuilder_AppendFormat_001, Level1)
{
    StringBuilder sb;

    // Test format with string
    sb.AppendFormat("%s", "Hello");
    EXPECT_EQ(sb.ToString(), "Hello");

    // Test format with integer
    sb.AppendFormat(" %d", 42);
    EXPECT_EQ(sb.ToString(), "Hello 42");

    // Test format with multiple arguments
    sb.AppendFormat(" %s %d", "world", 123);
    EXPECT_EQ(sb.ToString(), "Hello 42 world 123");
}

/*
 * @tc.name: StringBuilder_ToString_001
 * @tc.desc: Verify the StringBuilder ToString function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilStringTest, StringBuilder_ToString_001, Level1)
{
    StringBuilder sb;
    EXPECT_EQ(sb.ToString(), "");

    sb.Append("Hello");
    EXPECT_EQ(sb.ToString(), "Hello");

    // Verify ToString doesn't clear the builder
    std::string result = sb.ToString();
    EXPECT_EQ(sb.ToString(), "Hello");
}

} // namespace idl
} // namespace OHOS