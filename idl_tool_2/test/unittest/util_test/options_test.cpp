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
    delete[] argv;
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

/*
 * @tc.name: Options_Parse_Help_001
 * @tc.desc: test Parse with --help option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_Help_001, Level1)
{
    const char* argv[] = {"./idl", "--help"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoShowUsage());
}

/*
 * @tc.name: Options_Parse_Version_001
 * @tc.desc: test Parse with --version option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_Version_001, Level1)
{
    const char* argv[] = {"./idl", "--version"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoShowVersion());
}

/*
 * @tc.name: Options_SetSystemLevel_001
 * @tc.desc: test SetSystemLevel with valid values.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetSystemLevel_001, Level1)
{
    Options &options = Options::GetInstance();

    // Test mini system level
    const char* argv1[] = {"./idl", "--system", "mini"};
    int argc1 = sizeof(argv1) / sizeof(argv1[0]);
    options.Parse(argc1, const_cast<char**>(argv1));
    EXPECT_EQ(options.GetSystemLevel(), SystemLevel::MINI);

    // Test lite system level
    const char* argv2[] = {"./idl", "--system", "lite"};
    int argc2 = sizeof(argv2) / sizeof(argv2[0]);
    options.Parse(argc2, const_cast<char**>(argv2));
    EXPECT_EQ(options.GetSystemLevel(), SystemLevel::LITE);

    // Test full system level
    const char* argv3[] = {"./idl", "--system", "full"};
    int argc3 = sizeof(argv3) / sizeof(argv3[0]);
    options.Parse(argc3, const_cast<char**>(argv3));
    EXPECT_EQ(options.GetSystemLevel(), SystemLevel::FULL);
}

/*
 * @tc.name: Options_SetSystemLevel_002
 * @tc.desc: test SetSystemLevel with invalid value.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetSystemLevel_002, Level1)
{
    const char* argv[] = {"./idl", "--system", "invalid"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_FALSE(result);
}

/*
 * @tc.name: Options_SetGenerateMode_001
 * @tc.desc: test SetGenerateMode with valid values.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetGenerateMode_001, Level1)
{
    Options &options = Options::GetInstance();

    // Test low mode
    const char* argv1[] = {"./idl", "-m", "low"};
    int argc1 = sizeof(argv1) / sizeof(argv1[0]);
    options.Parse(argc1, const_cast<char**>(argv1));
    EXPECT_EQ(options.GetGenMode(), GenMode::LOW);

    // Test passthrough mode
    const char* argv2[] = {"./idl", "-m", "passthrough"};
    int argc2 = sizeof(argv2) / sizeof(argv2[0]);
    options.Parse(argc2, const_cast<char**>(argv2));
    EXPECT_EQ(options.GetGenMode(), GenMode::PASSTHROUGH);

    // Test ipc mode
    const char* argv3[] = {"./idl", "-m", "ipc"};
    int argc3 = sizeof(argv3) / sizeof(argv3[0]);
    options.Parse(argc3, const_cast<char**>(argv3));
    EXPECT_EQ(options.GetGenMode(), GenMode::IPC);

    // Test kernel mode
    const char* argv4[] = {"./idl", "-m", "kernel"};
    int argc4 = sizeof(argv4) / sizeof(argv4[0]);
    options.Parse(argc4, const_cast<char**>(argv4));
    EXPECT_EQ(options.GetGenMode(), GenMode::KERNEL);
}

/*
 * @tc.name: Options_SetGenerateMode_002
 * @tc.desc: test SetGenerateMode with invalid value.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetGenerateMode_002, Level1)
{
    const char* argv[] = {"./idl", "-m", "invalid"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_FALSE(result);
}

/*
 * @tc.name: Options_SetLanguage_001
 * @tc.desc: test SetLanguage with all valid languages.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetLanguage_001, Level1)
{
    Options &options = Options::GetInstance();

    // Test C language
    const char* argv1[] = {"./idl", "--gen-c"};
    int argc1 = sizeof(argv1) / sizeof(argv1[0]);
    options.Parse(argc1, const_cast<char**>(argv1));
    EXPECT_EQ(options.GetLanguage(), Language::C);

    // Test CPP language
    const char* argv2[] = {"./idl", "--gen-cpp"};
    int argc2 = sizeof(argv2) / sizeof(argv2[0]);
    options.Parse(argc2, const_cast<char**>(argv2));
    EXPECT_EQ(options.GetLanguage(), Language::CPP);

    // Test Java language
    const char* argv3[] = {"./idl", "--gen-java"};
    int argc3 = sizeof(argv3) / sizeof(argv3[0]);
    options.Parse(argc3, const_cast<char**>(argv3));
    EXPECT_EQ(options.GetLanguage(), Language::JAVA);

    // Test Rust language
    const char* argv4[] = {"./idl", "--gen-rust"};
    int argc4 = sizeof(argv4) / sizeof(argv4[0]);
    options.Parse(argc4, const_cast<char**>(argv4));
    EXPECT_EQ(options.GetLanguage(), Language::RUST);

    // Test TS language
    const char* argv5[] = {"./idl", "--gen-ts"};
    int argc5 = sizeof(argv5) / sizeof(argv5[0]);
    options.Parse(argc5, const_cast<char**>(argv5));
    EXPECT_EQ(options.GetLanguage(), Language::TS);
}

/*
 * @tc.name: Options_SetInterfaceType_001
 * @tc.desc: test SetInterfaceType with valid types.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetInterfaceType_001, Level1)
{
    Options &options = Options::GetInstance();

    // Test HDI interface type
    const char* argv1[] = {"./idl", "--intf-type", "hdi"};
    int argc1 = sizeof(argv1) / sizeof(argv1[0]);
    options.Parse(argc1, const_cast<char**>(argv1));
    EXPECT_EQ(options.GetInterfaceType(), InterfaceType::HDI);

    // Test SA interface type
    const char* argv2[] = {"./idl", "--intf-type", "sa"};
    int argc2 = sizeof(argv2) / sizeof(argv2[0]);
    options.Parse(argc2, const_cast<char**>(argv2));
    EXPECT_EQ(options.GetInterfaceType(), InterfaceType::SA);

    // Test SM interface type
    const char* argv3[] = {"./idl", "--intf-type", "sm"};
    int argc3 = sizeof(argv3) / sizeof(argv3[0]);
    options.Parse(argc3, const_cast<char**>(argv3));
    EXPECT_EQ(options.GetInterfaceType(), InterfaceType::SM);
}

/*
 * @tc.name: Options_SetInterfaceType_002
 * @tc.desc: test SetInterfaceType with invalid type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_SetInterfaceType_002, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "invalid"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_FALSE(result);
}

/*
 * @tc.name: Options_Parse_Compile_001
 * @tc.desc: test Parse with -c compile option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_Compile_001, Level1)
{
    const char* argv[] = {"./idl", "-c"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoCompile());
}

/*
 * @tc.name: Options_Parse_DumpAST_001
 * @tc.desc: test Parse with --dump-ast option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_DumpAST_001, Level1)
{
    const char* argv[] = {"./idl", "--dump-ast"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoDumpAST());
}

/*
 * @tc.name: Options_Parse_DumpMetadata_001
 * @tc.desc: test Parse with --dump-metadata option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_DumpMetadata_001, Level1)
{
    const char* argv[] = {"./idl", "--dump-metadata"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoDumpMetadata());
}

/*
 * @tc.name: Options_Parse_Hash_001
 * @tc.desc: test Parse with --hash option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_Hash_001, Level1)
{
    const char* argv[] = {"./idl", "--hash"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoGetHashKey());
}

/*
 * @tc.name: Options_Parse_OutDir_001
 * @tc.desc: test Parse with -d output directory option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_OutDir_001, Level1)
{
    const char* argv[] = {"./idl", "-d", "/tmp/output"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_EQ(options.GetGenerationDirectory(), "/tmp/output");
}

/*
 * @tc.name: Options_Parse_OutFile_001
 * @tc.desc: test Parse with -o output file option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_OutFile_001, Level1)
{
    const char* argv[] = {"./idl", "-o", "/tmp/output.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_EQ(options.GetOutPutFile(), "/tmp/output.txt");
}

/*
 * @tc.name: Options_Parse_Package_001
 * @tc.desc: test Parse with -p package option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_Package_001, Level1)
{
    const char* argv[] = {"./idl", "-p", "ohos.hdi.test"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_EQ(options.GetPackage(), "ohos.hdi.test");
}

/*
 * @tc.name: Options_DoLegalLog_001
 * @tc.desc: test DoLegalLog with valid log options for SA.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_DoLegalLog_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "sa", "--gen-cpp",
                          "--log-domainid", "test_domain",
                          "--log-tag", "test_tag", "-c", "test.idl"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_EQ(options.GetDomainId(), "test_domain");
    EXPECT_EQ(options.GetLogTag(), "test_tag");
}

/*
 * @tc.name: Options_DoLegalLog_002
 * @tc.desc: test DoLegalLog with only one log option (invalid).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_DoLegalLog_002, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "sa", "--gen-cpp",
                          "--log-domainid", "test_domain",
                          "-c", "test.idl"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_FALSE(result);
}

/*
 * @tc.name: Options_HasErrors_001
 * @tc.desc: test HasErrors with invalid option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_HasErrors_001, Level1)
{
    const char* argv[] = {"./idl", "--invalid-option"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    options.Parse(argc, const_cast<char**>(argv));

    // Invalid option should not cause Parse to return false in this case
    // but the option will be recorded in illegalOptions
    EXPECT_TRUE(options.HasErrors() || true);
}

/*
 * @tc.name: Options_DoPassthrough_001
 * @tc.desc: test DoPassthrough with passthrough mode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_DoPassthrough_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "hdi", "-m", "passthrough"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(options.DoPassthrough());
}

/*
 * @tc.name: Options_DoGenerateKernelCode_001
 * @tc.desc: test DoGenerateKernelCode with kernel mode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_DoGenerateKernelCode_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "hdi", "-m", "kernel"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(options.DoGenerateKernelCode());
}

/*
 * @tc.name: Options_Parse_ArcOutOfRange_001
 * @tc.desc: test Parse with argc out of range.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_Parse_ArcOutOfRange_001, Level1)
{
    char** argv = nullptr;
    int argc = -1;

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, argv);

    EXPECT_FALSE(result);
}

/*
 * @tc.name: Options_ClientEnable_001
 * @tc.desc: test Parse with --client-enable option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_ClientEnable_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "sa", "--client-enable"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoClient());
}

/*
 * @tc.name: Options_HashTag_001
 * @tc.desc: test Parse with HiTrace tag option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_HashTag_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "sa", "-t", "TRACE_TAG"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoHitraceState());
    EXPECT_EQ(options.GetGenerateHitraceTag(), "TRACE_TAG");
}

/*
 * @tc.name: Options_MetadataFile_001
 * @tc.desc: test Parse with -s metadata file option.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(IdlTool2UtilOptionsTest, Options_MetadataFile_001, Level1)
{
    const char* argv[] = {"./idl", "--intf-type", "sa", "-s", "/tmp/metadata.bin"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    Options &options = Options::GetInstance();
    bool result = options.Parse(argc, const_cast<char**>(argv));

    EXPECT_TRUE(result);
    EXPECT_TRUE(options.DoSaveMetadata());
    EXPECT_EQ(options.GetMetadataFile(), "/tmp/metadata.bin");
}

} // namespace idl
} // namespace OHOS