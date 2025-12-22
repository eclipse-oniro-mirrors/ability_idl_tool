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

#include "ast/ast_attribute.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTAttrTest : public testing::Test {
public:
    ASTAttrTest() {}
    virtual ~ASTAttrTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTAttrTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTAttrTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTAttrTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTAttrTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: ToString_001
 * @tc.desc: test ASTAttr ToString
 * @tc.type: FUNC
 */
HWTEST_F(ASTAttrTest, ToString_001, Level1)
{
    DTEST_LOG << "ToString_001 begin" << std::endl;
    ASTAttr attr(ASTAttr::NONE);
    ASTParamAttr paramAttr(ASTParamAttr::PARAM_NONE);
    EXPECT_EQ(attr.ToString(), "[] ");
    EXPECT_EQ(paramAttr.ToString(), "[]");
    attr.SetValue(ASTAttr::MINI);
    attr.SetValue(ASTAttr::LITE);
    attr.SetValue(ASTAttr::FULL);
    attr.SetValue(ASTAttr::ONEWAY);
    attr.SetValue(ASTAttr::CALLBACK);
    ASTParamAttr paramAttr2(ASTParamAttr::PARAM_INOUT);
    EXPECT_EQ(attr.ToString(), "[mini, lite, full, oneway, callback] ");
    EXPECT_EQ(paramAttr2.ToString(), "[in, out]");
    DTEST_LOG << "ToString_001 end" << std::endl;
}

/*
 * @tc.name: Match_001
 * @tc.desc: test ASTAttr Match
 * @tc.type: FUNC
 */
HWTEST_F(ASTAttrTest, Match_001, Level1)
{
    DTEST_LOG << "Match_001 begin" << std::endl;
    ASTAttr attr(ASTAttr::NONE);
    EXPECT_FALSE(attr.Match(SystemLevel::MINI));
    EXPECT_FALSE(attr.Match(SystemLevel::LITE));
    EXPECT_FALSE(attr.Match(SystemLevel::FULL));
    EXPECT_FALSE(attr.Match(static_cast<SystemLevel>(-1)));
    DTEST_LOG << "Match_001 end" << std::endl;
}

/*
 * @tc.name: CacheableStrToInt_001
 * @tc.desc: test ASTAttr CacheableStrToInt
 * @tc.type: FUNC
 */
HWTEST_F(ASTAttrTest, CacheableStrToInt_001, Level1)
{
    DTEST_LOG << "CacheableStrToInt_001 begin" << std::endl;
    ASTAttr attr(ASTAttr::NONE);
    EXPECT_FALSE(attr.CacheableStrToInt());
    attr.SetValue(ASTAttr::CACHEABLE);
    EXPECT_FALSE(attr.CacheableStrToInt());
    DTEST_LOG << "CacheableStrToInt_001 end" << std::endl;
}
} // namespace OHOS::Idl