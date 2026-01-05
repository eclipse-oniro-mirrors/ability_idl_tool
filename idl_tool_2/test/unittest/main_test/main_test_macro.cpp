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
#include "codegen/code_generator.h"
#include "parser/parser.h"
#include "preprocessor/preprocessor.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class IdlTool2MainTestMacro : public testing::Test {
public:
    IdlTool2MainTestMacro() {}
    virtual ~IdlTool2MainTestMacro() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IdlTool2MainTestMacro::SetUpTestCase() {}
void IdlTool2MainTestMacro::TearDownTestCase() {}
void IdlTool2MainTestMacro::SetUp() {}
void IdlTool2MainTestMacro::TearDown() {}

static int FileParse(Parser &parser)
{
    std::vector<FileDetail> fileDetails;
    if (!Preprocessor::Preprocess(fileDetails)) {
        Logger::E(TAG, "failed to preprocess");
        return -1;
    }

    if (!parser.Parse(fileDetails)) {
        Logger::E(TAG, "failed to parse file");
        return -1;
    }

    return 0;
}

/*
 * @tc.name: Main_Test_Macro_001
 * @tc.desc: test IdlTool2MainMacro.
 * @tc.type: FUNC
 */
HWTEST_F(IdlTool2MainTestMacro, Main_Test_Macro_001, Level1)
{
    char* argv[] = {
        const_cast<char*>("./IdlTool2MainTestMacro"),
        const_cast<char*>("-gen-cpp"),
        const_cast<char*>("-c"),
        const_cast<char*>("/data/test/resource/idl/IFooHilog.idl"),
        const_cast<char*>("-client-enable"),
        const_cast<char*>("-log-tag"),
        const_cast<char*>("IDL_TOOL"),
        const_cast<char*>("-log-domainid"),
        const_cast<char*>("0xD001800"),
        const_cast<char*>("-d"),
        const_cast<char*>("/data/test/resource/target_cpp"),
        const_cast<char*>("--intf-type"),
        const_cast<char*>("sa"),
        const_cast<char*>("-t"),
        const_cast<char*>("HITRACE_TAG_SAMGR"),
        const_cast<char*>("--dump-ast")
    };
    int argc = 16;
    Options &options = Options::GetInstance();
    options.Parse(argc, argv);
    StrAstMap astList;
    Parser parser;
    FileParse(parser);
    astList = parser.GetAllAst();
    auto ret = CodegenBuilder::GetInstance().Generate(astList);
    EXPECT_FALSE(options.GetSourceFiles().empty());
    EXPECT_TRUE(ret);
}
} // namespace OHOS::Idl