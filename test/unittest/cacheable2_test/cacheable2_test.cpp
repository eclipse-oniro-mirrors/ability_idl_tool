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

#include <gtest/gtest.h>
#include "cacheable_idl.h"
#include "cacheable_common.h"

#ifdef IDLTOOL_GTEST
#define private   public
#define protected public
#endif
#include "util/options.h"
#include "parser/parser.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Idl {
using namespace TestCommon;

class Cacheable2Test : public testing::Test {
public:
    Cacheable2Test() {}

    virtual ~Cacheable2Test() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void Cacheable2Test::SetUpTestCase() {}

void Cacheable2Test::TearDownTestCase() {}

void Cacheable2Test::SetUp() {}

void Cacheable2Test::TearDown() {}

/*
 * @tc.name: Cacheable2Test001
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test001, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test001, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_001_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST001_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_001_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: Cacheable2Test002
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test002, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test002, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_002_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST002_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_002_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: Cacheable2Test003
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test003, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test003, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_003_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST003_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_003_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: Cacheable2Test004
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test004, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test004, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_004_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST004_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_004_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: Cacheable2Test005
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test005, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test005, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_005_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST005_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_005_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: Cacheable2Test006
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test006, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test006, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_006_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST006_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_006_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    Parser parser;
    auto mode = parser.lexer_.mode_;
    parser.lexer_.mode_ = Lexer::ParseMode::EXPR_MODE;
    std::string path = "Cache.h";
    parser.lexer_.Reset(path);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, false);
    parser.lexer_.mode_ = mode;
}

/*
 * @tc.name: Cacheable2Test007
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test007, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test007, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_007_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST007_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_007_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: Cacheable2Test008
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test008, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test008, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_008_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST008_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_008_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    ret = parser.Parse(fileDetails);
    EXPECT_EQ(ret, true);
}

/*
 * @tc.name: Cacheable2Test009
 * @tc.desc: test Idltool2 Lexer
 * @tc.type: FUNC
 * @tc.require: issueICIIF8
 */
HWTEST_F(Cacheable2Test, Cacheable2Test009, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "Cacheable2Test, Cacheable2Test009, TestSize.Level1";
    PrepareIdlFile(CACHE_TIME_009_IDL_NAME.c_str(),
        CACHEABLE_TIME_TEST009_IDL_CONTENT.c_str());
    const char* argvArray[] = {"./idl", "-gen-cpp", "-c", CACHE_TIME_009_IDL_NAME.c_str()};
    int argc = 4;
    ParameterArgv parameters(argvArray, argc);
    Options &options = Options::GetInstance();
    options.sourceFiles.clear();
    bool ret = options.Parse(parameters.argc_, parameters.argv_);
    EXPECT_EQ(ret, true);
    std::vector<FileDetail> fileDetails;
    ret = Preprocessor::Preprocess(fileDetails);
    EXPECT_EQ(ret, true);
    Parser parser;
    parser.lexer_.Reset(fileDetails[0].filePath_);
    parser.lexer_.SkipCurrentLine();
    ret = parser.lexer_.SkipCurrentLine('h');
    EXPECT_EQ(ret, true);
    ret = parser.lexer_.SkipCurrentLine('f');
    EXPECT_EQ(ret, false);
    parser.lexer_.Skip('b');
    parser.lexer_.SkipToken(TokenType::LONG);
    parser.lexer_.SkipUntilToken(TokenType::END_OF_FILE);
    parser.lexer_.SkipEof();
}
} // namespace idl
} // namespace OHOS