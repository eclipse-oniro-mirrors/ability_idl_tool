/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "cacheable_idl.h"
#include "cacheable_common.h"
#include "util/options.h"
#include "parser/parser.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Idl {
using namespace TestCommon;

class CacheableTest : public testing::Test {
public:
    CacheableTest() {}

    virtual ~CacheableTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void CacheableTest::SetUpTestCase() {}

void CacheableTest::TearDownTestCase() {}

void CacheableTest::SetUp() {}

void CacheableTest::TearDown() {}

/*
 * @tc.name: CacheTimeTest001
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest_0100, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_001_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST001_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_001_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: CacheTimeTest002
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest002, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest002, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_002_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST002_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_002_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: CacheTimeTest003
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest003, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest003, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_003_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST003_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_003_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: CacheTimeTest004
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest004, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest004, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_004_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST004_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_004_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: CacheTimeTest005
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest005, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest005, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_005_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST005_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_005_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: CacheTimeTest006
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest006, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest006, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_006_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST006_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_006_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: CacheTimeTest007
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest007, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest007, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_007_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST007_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_007_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: CacheTimeTest008
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest008, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest008, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_008_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST008_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_008_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: CacheTimeTest009
 * @tc.desc: test CacheTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheableTest, CacheTimeTest009, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "CacheableTest, CacheTimeTest009, TestSize.Level1";

    PrepareIdlFile(CACHE_TIME_009_IDL_NAME.c_str(),
       CACHEABLE_TIME_TEST009_IDL_CONTENT.c_str());

    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_009_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);

    Options options(parameters.argc_, parameters.argv_);

    Parser p(options);
    auto ret = p.Parse(options.GetSourceFile());
    EXPECT_EQ(ret, false);
}
} // namespace idl
} // namespace OHOS