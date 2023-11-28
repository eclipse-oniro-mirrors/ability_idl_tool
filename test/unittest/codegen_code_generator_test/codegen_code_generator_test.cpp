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
#include "codegen/code_generator.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {
class CodeGeneratorUnitTest : public testing::Test {
public:
    CodeGeneratorUnitTest() {}
    virtual ~CodeGeneratorUnitTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CodeGeneratorUnitTest::SetUpTestCase() {}
void CodeGeneratorUnitTest::TearDownTestCase() {}
void CodeGeneratorUnitTest::SetUp() {}
void CodeGeneratorUnitTest::TearDown() {}

/*
 * @tc.name: CodeGeneratorUnitTest_0100
 * @tc.desc: Verify the initialization in CodeGeneratorUnitTest function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeGeneratorUnitTest, CodeGeneratorUnitTest_0100, Function | MediumTest | Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    String language = "cpp";
    String dir = "this dir";
    Options::Attribute att;
    att.hitraceTag = "hitraceTag";
    att.doHitrace = true;
    att.logTag = "logTag";
    att.domainId = "domainId";

    /**
     * @tc.steps: step2. Create codeGenerator object.
     * @tc.expected: Can obtain the correct logOn_.
     */
    CodeGenerator codeGenerator(&mc, language, dir, att);
    EXPECT_FALSE(codeGenerator.emitter_->logOn_);
}
}
}