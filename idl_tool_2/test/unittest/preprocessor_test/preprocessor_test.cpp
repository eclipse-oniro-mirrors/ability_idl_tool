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
#define private public
#include "util/options.h"
#include "util/file.h"
#include "preprocessor/preprocessor.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace Idl {
class PreprocessorTest : public testing::Test {
public:
    PreprocessorTest() {}
    virtual ~PreprocessorTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PreprocessorTest::SetUpTestCase() {}
void PreprocessorTest::TearDownTestCase() {}
void PreprocessorTest::SetUp() {}
void PreprocessorTest::TearDown() {}

/*
 * @tc.name: Dump_test_001
 * @tc.desc: test Dump function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(PreprocessorTest, Dump_test_001, Level1)
{
    DTEST_LOG << "Dump_test_001 begin" << std::endl;
    FileDetail file;
    file.filePath_ = "/foundation/ability/idl_tool/test/";
    file.fileName_ = "IFoo.idl";
    file.packageName_ = "OHOS.custom.idlFile";

    std::string expectedCode =
        "filePath:/foundation/ability/idl_tool/test/\n"
        "fileName:IFoo.idl\n"
        "packageName:OHOS.custom.idlFile\n"
        "import:[]\n";

    auto ret = file.Dump();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "Dump_test_001 end" << std::endl;
}

/*
 * @tc.name: Dump_test_002
 * @tc.desc: test Dump function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(PreprocessorTest, Dump_test_002, Level1)
{
    DTEST_LOG << "Dump_test_002 begin" << std::endl;
    FileDetail file;
    file.filePath_ = "/foundation/ability/idl_tool/test/";
    file.fileName_ = "IFoo.idl";
    file.packageName_ = "OHOS.custom.idlFile";
    file.imports_.insert("ohos.hdi.foo.v1_0.IFooCallback");
    file.imports_.insert("ohos.hdi.foo.v1_0.Types");

    std::string expectedCode =
        "filePath:/foundation/ability/idl_tool/test/\n"
        "fileName:IFoo.idl\n"
        "packageName:OHOS.custom.idlFile\n"
        "import:[\n"
        "ohos.hdi.foo.v1_0.Types,\n"
        "ohos.hdi.foo.v1_0.IFooCallback,\n"
        "]\n";

    auto ret = file.Dump();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "Dump_test_002 end" << std::endl;
}

/*
 * @tc.name: UnitPreprocess_test_001
 * @tc.desc: test UnitPreprocess function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(PreprocessorTest, UnitPreprocess_test_001, Level1)
{
    DTEST_LOG << "UnitPreprocess_test_001 begin" << std::endl;
    Preprocessor processor;
    FileDetailMap fileDetails;
    Options options;
    options.AddSourcesByDir("/data/test/resource/idl/Test");
    bool ret = processor.UnitPreprocess(fileDetails);
    EXPECT_EQ(ret, false);
    std::vector<FileDetail> vecFileDetails;
    ret = processor.Preprocess(vecFileDetails);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "UnitPreprocess_test_001 end" << std::endl;
}

/*
 * @tc.name: UnitPreprocess_test_002
 * @tc.desc: test Preprocess function.
 * @tc.type: FUNC
  * @tc.require: issueICL05Z
 */
HWTEST_F(PreprocessorTest, UnitPreprocess_test_002, Level1)
{
    DTEST_LOG << "UnitPreprocess_test_002 begin" << std::endl;
    Preprocessor processor;
    FileDetailMap fileDetails;
    Options options;
    options.AddSourcesByDir("/data/test/resource/idl/idl_src");
    bool ret = processor.UnitPreprocess(fileDetails);
    EXPECT_EQ(ret, false);
    std::vector<FileDetail> vecFileDetails;
    ret = processor.Preprocess(vecFileDetails);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "UnitPreprocess_test_002 end" << std::endl;
}

/**
 * @tc.name: PrintCyclefInfo_001
 * @tc.desc: test PrintCyclefInfo_001 function.
 * @tc.type: FUNC
* @tc.require: issueICL05Z
 */
HWTEST_F(PreprocessorTest, PrintCyclefInfo_001, Level1)
{
    DTEST_LOG << "PrintCyclefInfo_001 begin" << std::endl;
    Options option;
    option.SetInterfaceType("sa");
    option.SetLanguage("cpp");
    FileDetail file;
    file.filePath_ = "foundation/ability/idl_too/test";
    file.fileName_ = "ICallback.idl";
    file.packageName_ = "OHOS.custom.idlToolFile";
    file.imports_.insert("ohos.hdi.foo.v1_0.Types");

    FileDetail fileData;
    fileData.filePath_ = "/data/test/resource/idl/idl_src/";
    fileData.fileName_ = "IFoo.idl";
    fileData.packageName_ = "OHOS.custom.idlFile";
    fileData.imports_.insert("ohos.hdi.foo.v1_0.IFooCallback");
    fileData.imports_.insert("ohos.hdi.foo.v1_0.Types");
    std::vector<FileDetail> compileSourceFiles;
    Preprocessor processor;
    FileDetailMap allFileDetails {{"myCallback", file}, {"myIfoo", fileData}};
    FileDetail fileTmp;
    FileDetailMap fileDetails {{"myTest", fileTmp}};
    processor.PrintCyclefInfo(fileDetails);
    auto ret = processor.CheckCircularReference(allFileDetails, compileSourceFiles);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "PrintCyclefInfo_001 end" << std::endl;
}

/**
 * @tc.name: FindCycle_001
 * @tc.desc: test FindCycle_001 function.
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(PreprocessorTest, FindCycle_001, Level1)
{
    DTEST_LOG << "FindCycle_001 begin" << std::endl;
    Options option;
    option.SetInterfaceType("sa");
    option.SetLanguage("cpp");
    FileDetail file;
    file.filePath_ = "foundation/ability/idl_too/test";
    file.fileName_ = "ICallback.idl";
    file.packageName_ = "OHOS.custom.idlToolFile";
    file.imports_.insert("ohos.hdi.foo.v1_0.Types");

    FileDetailMap allFileDetails {{"myIdlTest", file}};
    std::vector<std::string> vecTrace;
    vecTrace.push_back("foundation/ability/idl_too/test");
    vecTrace.push_back("/data/test/resource/idl/idl_src/");

    Preprocessor processor;
    processor.FindCycle("/foundation/ability/idl_too/test", allFileDetails, vecTrace);
    EXPECT_FALSE(allFileDetails.empty());
    DTEST_LOG << "FindCycle_001 end" << std::endl;
}

/**
 * @tc.name: CheckPackageName_001
 * @tc.desc: test CheckPackageName_001 function.
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
*/
HWTEST_F(PreprocessorTest, CheckPackageName_001, Level1)
{
    DTEST_LOG << "CheckPackageName_001 begin" << std::endl;
    string filePath = "IFoo.idl";
    std::string packageName = "myPackage";
    Preprocessor processor;
    auto ret = processor.CheckPackageName(filePath, packageName);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "CheckPackageName_001 end" << std::endl;
}

/**
 * @tc.name: CheckPackageName_002
 * @tc.desc: test CheckPackageName_002 function.
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
*/
HWTEST_F(PreprocessorTest, CheckPackageName_002, Level1)
{
    DTEST_LOG << "CheckPackageName_002 begin" << std::endl;
#ifndef __MINGW32__
    std::string filePath = "/data/test/resource/idl/idl_src/IFoo.idl";
#else
    std::string filePath = "\\data\\test\\resource\\idl\\idl_src\\IFoo.idl";
#endif
    std::string packageName = "myPackage";
    Preprocessor processor;
    auto ret = processor.CheckPackageName(filePath, packageName);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "CheckPackageName_002 end" << std::endl;
}

} // namespace Idl
} // namespace OHOS
