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
#include "codegen/cpp_code_emitter.h"
#include "metadata/meta_component.h"
#include "metadata/meta_type.h"
#include "util/string_builder.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {
namespace idl {

class CppCodeEmitterUnitTest : public testing::Test {
public:
    CppCodeEmitterUnitTest() {}

    virtual ~CppCodeEmitterUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void CppCodeEmitterUnitTest::SetUpTestCase() {}

void CppCodeEmitterUnitTest::TearDownTestCase() {}

void CppCodeEmitterUnitTest::SetUp() {}

void CppCodeEmitterUnitTest::TearDown() {}

/**
 * @tc.name: EmitInterfaceStdlibInclusions_test_001
 * @tc.desc: Verify the EmitInterfaceStdlibInclusions function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitInterfaceStdlibInclusions_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitInterfaceStdlibInclusions_test_001, TestSize.Level1";

    MetaComponent mc;
    mc.typeNumber_ = 1;

    MetaType *mt = new MetaType;
    mc.types_ = new MetaType *[1];
    mc.types_[0] = mt;

    CppCodeEmitter codeEmitter(&mc);
    {
        mt->kind_ = TypeKind::Byte;
        StringBuilder sb;
        codeEmitter.EmitInterfaceStdlibInclusions(sb);
        EXPECT_STREQ(sb.buffer_, "#include <cstdint>\n");
    }
    {
        mt->kind_ = TypeKind::String;
        StringBuilder sb;
        codeEmitter.EmitInterfaceStdlibInclusions(sb);
        EXPECT_STREQ(sb.buffer_, "#include <string_ex.h>\n");
    }
    {
        mt->kind_ = TypeKind::Array;
        StringBuilder sb;
        codeEmitter.EmitInterfaceStdlibInclusions(sb);
        EXPECT_STREQ(sb.buffer_, "#include <vector>\n");
    }
    {
        mt->kind_ = TypeKind::Map;
        StringBuilder sb;
        codeEmitter.EmitInterfaceStdlibInclusions(sb);
        EXPECT_STREQ(sb.buffer_, "#include <unordered_map>\n");
    }
    {
        mt->kind_ = TypeKind::Unknown;
        StringBuilder sb;
        codeEmitter.EmitInterfaceStdlibInclusions(sb);
        EXPECT_EQ(sb.buffer_, nullptr);
    }
    delete mt;
    delete [] mc.types_;
}

/**
 * @tc.name: FileName_test_001
 * @tc.desc: Verify the FileName function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, FileName_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, FileName_test_002, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String inputStr, expectStr;
    EXPECT_EQ(codeEmitter.FileName(inputStr), expectStr);

    const String inputStr1("abc");
    const String expectStr1("abc");
    EXPECT_STREQ(codeEmitter.FileName(inputStr1), expectStr1);

    const String inputStr2("a.b.c");
    const String expectStr2("a/b/c");
    EXPECT_STREQ(codeEmitter.FileName(inputStr2), expectStr2);

    const String inputStr3("a..B..C..d..");
    const String expectStr3("a//b//c//d//");
    EXPECT_STREQ(codeEmitter.FileName(inputStr3), expectStr3);
}

/**
 * @tc.name: MacroName_test_001
 * @tc.desc: Verify the MacroName function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, MacroName_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, MacroName_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String emptyStr, expectStr;
    EXPECT_EQ(codeEmitter.MacroName(emptyStr), expectStr);

    const String inputStr1("macro.Str");
    const String expectStr1("MACRO_STR_H");
    EXPECT_STREQ(codeEmitter.MacroName(inputStr1), expectStr1);
}

/**
 * @tc.name: EmitWriteVariable_test_001
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_001, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Boolean };
    std::string expectStr(
        "const if (!ParcelNameWriteInt32(Name ? 1 : 0)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_002
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_002, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Char };
    std::string expectStr("const if (!ParcelNameWriteInt32(Name)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_003
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_003, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Long };
    std::string expectStr("const if (!ParcelNameWriteInt64(Name)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_004
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_004, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Float };
    std::string expectStr("const if (!ParcelNameWriteFloat(Name)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_005
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_005, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Double };
    std::string expectStr("const if (!ParcelNameWriteDouble(Name)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_006
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_006, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::String };
    std::string expectStr(
        "const if (!ParcelNameWriteString16(Str8ToStr16(Name))) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_007
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_007, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Sequenceable };
    std::string expectStr(
        "const if (!ParcelNameWriteParcelable(&Name)) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_008
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNeC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitWriteVariable_test_008, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Interface };
    std::string expectStr(
        "const if (Name == nullptr) {\nconst     return ERR_INVALID_DATA;\nconst }\nconst if "
        "(!ParcelNameWriteRemoteObject(Name->AsObject())) {\nconst     return ERR_INVALID_DATA;\nconst }\n");
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_001
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_001, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Boolean };
    std::string expectStr("const bool Name = ParcelNameReadInt32() == 1 ? true : false;\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_002
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_002, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Boolean };
    std::string expectStr("const Name = ParcelNameReadInt32() == 1 ? true : false;\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_003
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_003, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Short };
    std::string expectStr("const short Name = (short)ParcelNameReadInt32();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_004
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_004, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Short };
    std::string expectStr("const Name = (short)ParcelNameReadInt32();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_005
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_005, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Integer };
    std::string expectStr("const int32_t Name = ParcelNameReadInt32();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_006
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_006, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Integer };
    std::string expectStr("const Name = ParcelNameReadInt32();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_007
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_007, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Long };
    std::string expectStr("const long Name = ParcelNameReadInt64();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_008
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_008, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Long };
    std::string expectStr("const Name = ParcelNameReadInt64();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_009
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_009, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Float };
    std::string expectStr("const float Name = ParcelNameReadFloat();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_010
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_010, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_010, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Float };
    std::string expectStr("const Name = ParcelNameReadFloat();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_011
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_011, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_011, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Double };
    std::string expectStr("const double Name = ParcelNameReadDouble();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_012
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_012, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_012, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::Double };
    std::string expectStr("const Name = ParcelNameReadDouble();\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_013
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_013, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_013, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::String };
    std::string expectStr("const std::string Name = Str16ToStr8(ParcelNameReadString16());\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, true);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: EmitReadVariable_test_014
 * @tc.desc: Verify the EmitReadVariable function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReadVariable_test_014, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReadVariable_test_014, TestSize.Level1";

    const String parcelName("ParcelName");
    const String prefix("const ");
    const std::string name("Name");

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    StringBuilder sb;
    MetaType mt { .kind_ = TypeKind::String };
    std::string expectStr("const Name = Str16ToStr8(ParcelNameReadString16());\n");
    codeEmitter.EmitReadVariable(parcelName, name, &mt, sb, prefix, false);
    EXPECT_STREQ(sb.buffer_, expectStr.c_str());
}

/**
 * @tc.name: ConstantName_test_001
 * @tc.desc: Verify the ConstantName function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, ConstantName_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, ConstantName_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String emptyStr, expectStr;
    EXPECT_EQ(codeEmitter.ConstantName(emptyStr), expectStr);

    const String inputStr3("aKxZ");
    const String expectStr3("AKX_Z");
    EXPECT_STREQ(codeEmitter.ConstantName(inputStr3), expectStr3);
}

/**
 * @tc.name: UnderlineAdded_test_001
 * @tc.desc: Verify the UnderlineAdded function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, UnderlineAdded_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, UnderlineAdded_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String inputStr("origStr");
    const std::string expectStr("_origStr");
    EXPECT_STREQ(codeEmitter.UnderlineAdded(inputStr).c_str(), expectStr.c_str());
}

/**
 * @tc.name: EmitReturnParameter_test_001
 * @tc.desc: Verify the EmitReturnParameter function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitReturnParameter_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitReturnParameter_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String name("const_string");
    {
        MetaType mt { .kind_ = TypeKind::Char };
        StringBuilder sb;
        codeEmitter.EmitReturnParameter(name, &mt, sb);
        EXPECT_STREQ(sb.buffer_, name.string());
    }
    {
        MetaType mt { .kind_ = TypeKind::Unknown };
        StringBuilder sb;
        codeEmitter.EmitReturnParameter(name, &mt, sb);
        EXPECT_EQ(sb.buffer_, nullptr);
    }
}

/**
 * @tc.name: EmitType_test_001
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Char };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("zchar"));
    EXPECT_STREQ(retStr2, String("zchar&"));
}

/**
 * @tc.name: EmitType_test_002
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_002, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_002, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Boolean };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("bool"));
    EXPECT_STREQ(retStr2, String("bool&"));
}

/**
 * @tc.name: EmitType_test_003
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_003, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_003, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Byte };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("int8_t"));
    EXPECT_STREQ(retStr2, String("int8_t&"));
}

/**
 * @tc.name: EmitType_test_004
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_004, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_004, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Short };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("short"));
    EXPECT_STREQ(retStr2, String("short&"));
}

/**
 * @tc.name: EmitType_test_005
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_005, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_006, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Integer };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("int32_t"));
    EXPECT_STREQ(retStr2, String("int32_t&"));
}

/**
 * @tc.name: EmitType_test_006
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_006, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_006, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Long };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("long"));
    EXPECT_STREQ(retStr2, String("long&"));
}

/**
 * @tc.name: EmitType_test_007
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_007, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_007, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Float };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("float"));
    EXPECT_STREQ(retStr2, String("float&"));
}

/**
 * @tc.name: EmitType_test_008
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_008, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_008, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Double };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr2 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("double"));
    EXPECT_STREQ(retStr2, String("double&"));
}


/**
 * @tc.name: EmitType_test_009
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_009, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_009, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::String };
    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN, false);
    String retStr2 = codeEmitter.EmitType(&mt, ATTR_IN, true);
    String retStr3 = codeEmitter.EmitType(&mt, 0, true);

    EXPECT_STREQ(retStr1, String("const std::string&"));
    EXPECT_STREQ(retStr2, String("std::string"));
    EXPECT_STREQ(retStr3, String("std::string&"));
}

/**
 * @tc.name: EmitType_test_010
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_010, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_010, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
}

/**
 * @tc.name: EmitType_test_011
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_011, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_011, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    MetaType mt { .kind_ = TypeKind::Void };
    String retStr = codeEmitter.EmitType(&mt, ATTR_IN, false);
    EXPECT_STREQ(retStr, String("void"));
}

/**
 * @tc.name: EmitType_test_012
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_012, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_012, TestSize.Level1";

    MetaType mt { .kind_ = TypeKind::Sequenceable, .index_ = 0 };
    char mseqName[] = "MetaSequenceable";
    MetaSequenceable mp;
    mp.name_ = mseqName;

    MetaComponent mc;
    mc.sequenceables_ = new MetaSequenceable*[1];
    mc.sequenceables_[0] = &mp;
    CppCodeEmitter codeEmitter(&mc);

    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN | ATTR_OUT, false);
    String retStr2 = codeEmitter.EmitType(&mt, ATTR_IN, false);
    String retStr3 = codeEmitter.EmitType(&mt, 0, false);

    EXPECT_STREQ(retStr1, String("MetaSequenceable*"));
    EXPECT_STREQ(retStr2, String("const MetaSequenceable&"));
    EXPECT_STREQ(retStr3, String("MetaSequenceable&"));

    delete []mc.sequenceables_;
}

/**
 * @tc.name: EmitType_test_013
 * @tc.desc: Verify the EmitType function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitType_test_013, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, EmitType_test_013, TestSize.Level1";

    MetaType mt { .kind_ = TypeKind::Interface, .index_ = 0 };
    char miName[] = "MetaInterface";
    MetaInterface mi;
    mi.name_ = miName;

    MetaComponent mc;
    mc.interfaces_ = new MetaInterface*[1];
    mc.interfaces_[0] = &mi;
    CppCodeEmitter codeEmitter(&mc);

    String retStr1 = codeEmitter.EmitType(&mt, ATTR_IN | ATTR_OUT, false);
    String retStr2 = codeEmitter.EmitType(&mt, ATTR_IN, false);
    String retStr3 = codeEmitter.EmitType(&mt, 0, false);

    EXPECT_STREQ(retStr1.string(), "const sptr<MetaInterface>&");
    EXPECT_STREQ(retStr2.string(), "const sptr<MetaInterface>&");
    EXPECT_STREQ(retStr3.string(), "sptr<MetaInterface>&");

    delete []mc.interfaces_;
}

/**
 * @tc.name: CppFullName_test_001
 * @tc.desc: Verify the CppFullName function.
 * @tc.type: FUNC
 * @tc.require: #I72EZC
 */
HWTEST_F(CppCodeEmitterUnitTest, CppFullName_test_001, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "CppCodeEmitterUnitTest, CppFullName_test_001, TestSize.Level1";

    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);

    const String inputStr, expectStr;
    EXPECT_STREQ(codeEmitter.CppFullName(inputStr), expectStr);

    const String inputStr1("a.b.c.d");
    const String expectStr1("a::b::c::d");
    EXPECT_STREQ(codeEmitter.CppFullName(inputStr1).string(), expectStr1.string());
}

/**
 * @tc.name: EmitInterfaceProxyCppFile_test_001
 * @tc.desc: Verify the EmitInterfaceProxyCppFile function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitInterfaceProxyCppFile_test_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    char name = 'n';
    const String directory = "Idl";
    String HitraceTag = "HITRACE_TAG_DLSM";
    String logTag = "DISP_SEED";
    String domainId = "0xb0d2d4";

    /**
     * @tc.steps: step2. Initialize pointers in functions.
     */
    codeEmitter.metaInterface_ = new MetaInterface[13];
    ASSERT_NE(codeEmitter.metaInterface_, nullptr);
    codeEmitter.metaInterface_->license_ = &name;
    ASSERT_NE(codeEmitter.metaInterface_->license_, nullptr);
    codeEmitter.metaInterface_->namespace_ = &name;
    codeEmitter.metaInterface_->methods_ = new MetaMethod *[6];
    for (int j = 0; j < 6; ++j) {
        codeEmitter.metaInterface_->methods_[j] = new MetaMethod();
        ASSERT_NE(codeEmitter.metaInterface_->methods_[j], nullptr);
    }
    codeEmitter.metaInterface_->methodNumber_ = 2;
    codeEmitter.metaComponent_->types_ = new MetaType *[3];
    for (int i = 0; i < 3; ++i) {
        codeEmitter.metaComponent_->types_[i] = new MetaType();
        ASSERT_NE(codeEmitter.metaComponent_->types_[i], nullptr);
    }

    /**
     * @tc.steps: step3. Assigning parameters to a function.
     */
    codeEmitter.SetDirectory(directory);
    codeEmitter.proxyName_ = "ability_proxy";
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetHitraceTag(HitraceTag);
    codeEmitter.SetHitraceOn(true);
    codeEmitter.SetLogTag(logTag);

    /**
     * @tc.steps: step4. Execute functions that require validation.
     * @tc.expected: Can obtain the correct hitraceOn_.
     */
    codeEmitter.EmitInterfaceProxyCppFile();
    EXPECT_TRUE(codeEmitter.hitraceOn_);

    for (int i = 0; i < 6; i++) {
        delete codeEmitter.metaInterface_->methods_[i];
    }
    delete[] codeEmitter.metaInterface_->methods_;
    for (int j = 0; j < 3; j++) {
        delete codeEmitter.metaComponent_->types_[j];
    }
    delete[] codeEmitter.metaComponent_->types_;
    delete[] codeEmitter.metaInterface_;
}

/**
 * @tc.name: EmitInterfaceMemberVariables_test_001
 * @tc.desc: Verify the EmitInterfaceMemberVariables function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitInterfaceMemberVariables_test_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    String logTag = "HITRACE_TAG_ABILITY_MANAGER";
    String domainId = "0xb0d2d4";
    StringBuilder sb;
    String prefix = "const";
    codeEmitter.proxyName_ = "ability_proxy";

    /**
     * @tc.steps: step2. Set domainId and logTag.
     */
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetLogTag(logTag);

    /**
     * @tc.steps: step3. Execute functions that require validation.
     * @tc.expected: Can obtain the correct logTag_.
     */
    codeEmitter.EmitInterfaceMemberVariables(sb, prefix);
    std::string expectStr("HITRACE_TAG_ABILITY_MANAGER");
    EXPECT_STREQ(codeEmitter.logTag_, expectStr.c_str());
}

/**
 * @tc.name: EmitInterfaceDBinderInclusions_test_001
 * @tc.desc: Verify the EmitInterfaceDBinderInclusions function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitInterfaceDBinderInclusions_test_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    String logTag = "HITRACE_TAG_ABILITY_MANAGER";
    String domainId = "0xb0d2d4";
    StringBuilder sb;

    /**
     * @tc.steps: step2. Set domainId and logTag.
     */
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetLogTag(logTag);

    /**
     * @tc.steps: step3. Execute functions that require validation.
     * @tc.expected: Can obtain the correct logTag_.
     */
    codeEmitter.EmitInterfaceDBinderInclusions(sb);
    std::string expectStr("HITRACE_TAG_ABILITY_MANAGER");
    EXPECT_STREQ(codeEmitter.logTag_, expectStr.c_str());
}

/**
 * @tc.name: EmitInterfaceStubCppFile_test_001
 * @tc.desc: Verify the EmitInterfaceStubCppFile function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitInterfaceStubCppFile_test_001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    String logTag = "HITRACE_TAG_ABILITY_MANAGER";
    String domainId = "0xb0d2d4";
    StringBuilder sb;
    char name = 'n';

    /**
     * @tc.steps: step2. Initialize pointers in functions.
     */
    codeEmitter.metaInterface_ = new MetaInterface[13];
    ASSERT_NE(codeEmitter.metaInterface_, nullptr);
    codeEmitter.metaInterface_->license_ = &name;
    ASSERT_NE(codeEmitter.metaInterface_->license_, nullptr);
    codeEmitter.metaInterface_->namespace_ = &name;
    ASSERT_NE(codeEmitter.metaInterface_->namespace_, nullptr);
    codeEmitter.metaInterface_->methods_ = new MetaMethod *[6];
    for (int j = 0; j < 6; ++j) {
        codeEmitter.metaInterface_->methods_[j] = new MetaMethod();
        ASSERT_NE(codeEmitter.metaInterface_->methods_[j], nullptr);
    }
    codeEmitter.metaInterface_->methodNumber_ = 2;
    codeEmitter.metaComponent_->types_ = new MetaType *[4];
    for (int i = 0; i < 4; ++i) {
        codeEmitter.metaComponent_->types_[i] = new MetaType();
        ASSERT_NE(codeEmitter.metaComponent_->types_[i], nullptr);
    }

    /**
     * @tc.steps: step3. Set domainId and logTag.
     */
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetLogTag(logTag);

    /**
     * @tc.steps: step4. Execute functions that require validation.
     * @tc.expected: Can obtain the correct hitraceOn_.
     */
    codeEmitter.EmitInterfaceStubCppFile();
    EXPECT_FALSE(codeEmitter.hitraceOn_);

    for (int i = 0; i < 6; i++) {
        delete codeEmitter.metaInterface_->methods_[i];
    }
    delete[] codeEmitter.metaInterface_->methods_;
    for (int j = 0; j < 4; j++) {
        delete codeEmitter.metaComponent_->types_[j];
    }
    delete[] codeEmitter.metaComponent_->types_;
    delete [] codeEmitter.metaInterface_;
}

/**
 * @tc.name: EmitWriteVariable_test_009
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    String logTag = "HITRACE_TAG_ABILITY_MANAGER";
    String domainId = "0xb0d2d4";
    StringBuilder sb;
    String parcelName = "parcelName";
    std::string name = "name";
    String prefix = "const";

    /**
     * @tc.steps: step2. When kind_ Validate functions when taking different values.
     * @tc.expected: Can obtain the correct logTag_.
     */
    MetaType mt;
    mt.kind_ = TypeKind::Integer;
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetLogTag(logTag);
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);

    mt.kind_ = TypeKind::Long;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);

    mt.kind_ = TypeKind::Float;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);

    mt.kind_ = TypeKind::Double;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);

    mt.kind_ = TypeKind::String;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);

    mt.kind_ = TypeKind::Sequenceable;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    std::string expectStr("HITRACE_TAG_ABILITY_MANAGER");
    EXPECT_STREQ(codeEmitter.logTag_, expectStr.c_str());
}

/**
 * @tc.name: EmitWriteVariable_test_0010
 * @tc.desc: Verify the EmitWriteVariable function.
 * @tc.type: FUNC
 * @tc.require: #I8JQUO
 */
HWTEST_F(CppCodeEmitterUnitTest, EmitWriteVariable_test_0010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Initialization parameters.
     */
    MetaComponent mc;
    CppCodeEmitter codeEmitter(&mc);
    String logTag = "HITRACE_TAG_ABILITY_MANAGER";
    String domainId = "0xb0d2d4";
    StringBuilder sb;
    String parcelName = "parcelName";
    std::string name = "name";
    String prefix = "const";
    int32_t indexes = 1;

    /**
     * @tc.steps: step2. When kind_ Validate functions when taking different values.
     * @tc.expected: Can obtain the correct logTag_.
     */
    MetaType mt;
    mt.kind_ = TypeKind::Integer;
    codeEmitter.SetDomainId(domainId);
    codeEmitter.SetLogTag(logTag);
    codeEmitter.metaComponent_->types_ = new MetaType *[3];
    for (int i = 0; i < 3; ++i) {
        codeEmitter.metaComponent_->types_[i] = new MetaType();
        ASSERT_NE(codeEmitter.metaComponent_->types_[i], nullptr);
    }
    mt.nestedTypeIndexes_ = &indexes;
    mt.kind_ = TypeKind::List;

    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    mt.kind_ = TypeKind::Array;
    codeEmitter.EmitWriteVariable(parcelName, name, &mt, sb, prefix);
    std::string expectStr("HITRACE_TAG_ABILITY_MANAGER");
    EXPECT_STREQ(codeEmitter.logTag_, expectStr.c_str());

    for (int j = 0; j < 3; j++) {
        delete codeEmitter.metaComponent_->types_[j];
    }
    delete[] codeEmitter.metaComponent_->types_;
}
} // namespace idl
} // namespace OHOS
