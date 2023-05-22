/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#define private public
#include "util/string.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {

class UtilStringUnitTest : public testing::Test {
public:
    UtilStringUnitTest() {}

    virtual ~UtilStringUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void UtilStringUnitTest::SetUpTestCase() {}

void UtilStringUnitTest::TearDownTestCase() {}

void UtilStringUnitTest::SetUp() {}

void UtilStringUnitTest::TearDown() {}

/**
 * @tc.name: IndexOf_test_001
 * @tc.desc: Verify the IndexOf function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, IndexOf_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, IndexOf_test_001, TestSize.Level1";

    EXPECT_EQ(String().IndexOf(String("name"), 1), -1);
    EXPECT_EQ(String("name").IndexOf(String(), 1), -1);
    EXPECT_EQ(String("bananan").IndexOf("ana", 1), 1);
}

/**
 * @tc.name: LastIndexOf_test_001
 * @tc.desc: Verify the LastIndexOf function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, LastIndexOf_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, LastIndexOf_test_001, TestSize.Level1";

    EXPECT_EQ(String().LastIndexOf(String("name"), 1), -1);
    EXPECT_EQ(String("bananan").LastIndexOf(String("ana"), 1), 1);
    EXPECT_EQ(String("bananan").LastIndexOf(String("ana"), -2), -1);
    EXPECT_EQ(String("bananana").LastIndexOf(String("ana"), 0), 5);
    EXPECT_EQ(String("bananana").LastIndexOf(String("ana"), 1), 1);
}

/**
 * @tc.name: StartsWith_test_001
 * @tc.desc: Verify the StartsWith function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, StartsWith_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, StartsWith_test_001, TestSize.Level1";

    EXPECT_FALSE(String().StartsWith(String("name")));
    EXPECT_FALSE(String("name").StartsWith(String()));
    EXPECT_FALSE(String("name").StartsWith(String("names")));

    EXPECT_TRUE(String("name").StartsWith(String("")));
    EXPECT_TRUE(String("name").StartsWith(String("na")));
    EXPECT_TRUE(String("n.a.m.e").StartsWith(String("n.a")));
}

/**
 * @tc.name: EndsWith_test_001
 * @tc.desc: Verify the EndsWith function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, EndsWith_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, EndsWith_test_001, TestSize.Level1";

    EXPECT_FALSE(String().EndsWith(String("name")));
    EXPECT_FALSE(String("name").EndsWith(String()));
    EXPECT_FALSE(String("name").EndsWith(String("names")));
    EXPECT_FALSE(String("name").EndsWith(String("ma")));

    EXPECT_TRUE(String("name").EndsWith(String("")));
    EXPECT_TRUE(String("name").EndsWith(String("me")));
    EXPECT_TRUE(String("n.a.m.e").EndsWith(String("m.e")));
}

/**
 * @tc.name: ToLowerCase_test_001
 * @tc.desc: Verify the ToLowerCase function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, ToLowerCase_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, ToLowerCase_test_001, TestSize.Level1";

    EXPECT_STREQ(String().ToLowerCase(), String());
    EXPECT_STREQ(String("name").ToLowerCase(), String("name"));
    EXPECT_STREQ(String("NAME").ToLowerCase(), String("name"));
    EXPECT_STREQ(String("n.A.m.E").ToLowerCase(), String("n.a.m.e"));
    EXPECT_STREQ(String("n.a.m.e").ToLowerCase(), String("n.a.m.e"));
}

/**
 * @tc.name: ToUpperCase_test_001
 * @tc.desc: Verify the ToUpperCase function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, ToUpperCase_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, ToUpperCase_test_001, TestSize.Level1";

    EXPECT_STREQ(String().ToUpperCase(), String());
    EXPECT_STREQ(String("name").ToUpperCase(), String("NAME"));
    EXPECT_STREQ(String("NAME").ToUpperCase(), String("NAME"));
    EXPECT_STREQ(String("n.a.m.e").ToUpperCase(), String("N.A.M.E"));
    EXPECT_STREQ(String("n.A.m.E").ToUpperCase(), String("N.A.M.E"));
}

/**
 * @tc.name: Replace_test_001
 * @tc.desc: Verify the Replace function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, Replace_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, Replace_test_001, TestSize.Level1";

    String name("n.a.m.e");
    EXPECT_STREQ(name.Replace('.', '.'), name);
    EXPECT_STREQ(String("n.a.m.e").Replace('.', '|'), String("n|a|m|e"));
    EXPECT_STREQ(String().Replace('.', '|'), String());
}

/**
 * @tc.name: Replace_test_002
 * @tc.desc: Verify the Replace function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, Replace_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, Replace_test_002, TestSize.Level1";

    String name("n..a..m..e");
    EXPECT_STREQ(name.Replace(nullptr, "replacemenet"), name);
    EXPECT_STREQ(name.Replace("", "replacemenet"), name);
    EXPECT_STREQ(name.Replace("target", nullptr), name);
    EXPECT_STREQ(name.Replace("..", "//"), String("n//a//m//e"));
}

/**
 * @tc.name: Replace_test_003
 * @tc.desc: Verify the Replace function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, Replace_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, Replace_test_003, TestSize.Level1";

    String name("n..a..m..e");
    EXPECT_STREQ(name.Replace(String(), String("replacemenet")), name);
    EXPECT_STREQ(name.Replace(String(".."), String("||")), String("n||a||m||e"));
}

/**
 * @tc.name: Operator_test_001
 * @tc.desc: Verify the Operator function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(UtilStringUnitTest, Operator_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UtilStringUnitTest, Operator_test_001, TestSize.Level1";

    EXPECT_STREQ(String("str1") += String("str2"), String("str1str2"));
    EXPECT_STREQ(String("str1") = static_cast<const char *>(nullptr), nullptr);
}

} // namespace idl
} // namespace OHOS
