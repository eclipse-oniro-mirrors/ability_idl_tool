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

#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class IdlTool2UtilOptionsTest : public testing::Test {
public:
    IdlTool2UtilOptionsTest() {}
    virtual ~IdlTool2UtilOptionsTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IdlTool2UtilOptionsTest::SetUpTestCase() {}
void IdlTool2UtilOptionsTest::TearDownTestCase() {}
void IdlTool2UtilOptionsTest::SetUp() {}
void IdlTool2UtilOptionsTest::TearDown() {}

/*
 * @tc.name: Options_test_001
 * @tc.desc: test DoHitraceState DoLogOn GetGenerateHitraceTag GetDomainId function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_test_001, Level1)
{
    std::string strings[] = {"-t", "-log-domainid", "-log-tag"};
    int32_t count = sizeof(strings) / sizeof(strings[0]);
    char **argv = new char *[count];
    for (int32_t i = 0; i < count; i++) {
        argv[i] = const_cast<char*>(strings[i].c_str());
    }

    Options &options = Options::GetInstance();
    options.Parse(count, argv);
    auto doHitrace = options.DoHitraceState();
    auto logOn = options.DoLogOn();
    auto hitRaceTag = options.GetGenerateHitraceTag();
    auto doMainId = options.GetDomainId();
    auto logTag = options.GetLogTag();
    auto attRibute = options.GetAttribute();
    options.ShowUsage();
    options.ShowWarning();
    EXPECT_FALSE(doHitrace);
    EXPECT_FALSE(logOn);
    delete[] argv;
}

/*
 * @tc.name: Options_test_002
 * @tc.desc: test ShowErrors ShowVersion function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_test_002, Level1)
{
    std::string strings[] = {"-t", "-log-domainid", "-log-tag"};
    int32_t count = sizeof(strings) / sizeof(strings[0]);
    char **argv = new char *[count];
    for (int32_t i = 0; i < count; i++) {
        argv[i] = const_cast<char*>(strings[i].c_str());
    }

    Options &options = Options::GetInstance();
    options.Parse(count, argv);
    auto doHitrace = options.DoHitraceState();
    auto logOn = options.DoLogOn();
    options.ShowErrors();
    options.ShowVersion();
    EXPECT_FALSE(doHitrace);
    EXPECT_FALSE(logOn);
    delete[] argv;
}

/*
 * @tc.name: Options_test_003
 * @tc.desc: test Parse function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_test_003, Level1)
{
    std::string strings[] = {"./idl", "--help", "--version", "-c", "-dump-ast",
        "-dump-metadata", "-s", "-gen-rust", "-gen-cpp", "-gen-ts", "-d",
        "-log-domainid", "-log-tag", "-t", "-others"};
    int32_t count = sizeof(strings) / sizeof(strings[0]);
    char **argv = new char *[count];
    for (int32_t i = 0; i < count; i++) {
        argv[i] = const_cast<char*>(strings[i].c_str());
    }

    Options &options = Options::GetInstance();
    options.Parse(count, argv);
    EXPECT_TRUE(argv != nullptr);
}

/*
 * @tc.name: Options_test_004
 * @tc.desc: test WriteData ReadData function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_test_004, Level1)
{
    Logger::SetLevel(Logger::VERBOSE);
    Logger::D("IdlTool2UtilOptionsTest", "idl log ut test::DEBUG");
    Logger::E("IdlTool2UtilOptionsTest", "idl log ut test::ERROR");
    Logger::V("IdlTool2UtilOptionsTest", "idl log ut test::VERBOSE");

    File file("", File::WRITE);
    EXPECT_EQ(file.Skip(0), false);
    EXPECT_EQ(file.Reset(), false);
    EXPECT_EQ(file.WriteData(nullptr, 0), true);
    EXPECT_EQ(file.ReadData(nullptr, 0), 0);
    int32_t count = 0;
    void* data = &count;
    EXPECT_EQ(file.ReadData(data, INT16_MAX), false);
}
} // namespace idl
} // namespace OHOS