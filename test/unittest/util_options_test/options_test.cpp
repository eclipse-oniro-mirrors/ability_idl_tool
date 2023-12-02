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
#define protected public
#include "util/options.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class OptionsUnitTest : public testing::Test {
public:
    OptionsUnitTest() {}
    virtual ~OptionsUnitTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void OptionsUnitTest::SetUpTestCase() {}
void OptionsUnitTest::TearDownTestCase() {}
void OptionsUnitTest::SetUp() {}
void OptionsUnitTest::TearDown() {}

/*
 * @tc.name: OptionsUnitTest_0100
 * @tc.desc: test GetDoHitraceState and GetLogState in OptionsUnitTest.
 * @tc.type: FUNC
 */
HWTEST_F(OptionsUnitTest, OptionsUnitTest_0100, Function | MediumTest | Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    std::string strings[] = {"-t", "-log-domainid", "-log-tag"};
    int32_t count = sizeof(strings) / sizeof(strings[0]);
    char **argv = new char *[count];
    for (int32_t i = 0; i < count; i++) {
        argv[i] = const_cast<char*>(strings[i].c_str());
    }

    /**
     * @tc.steps: step2. Set options to call functions.
     * @tc.expected: Can obtain the correct doHitrace, reasonAble and logState value.
     */
    Options options(count, argv);
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
}
}