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

#include "codegen/SA/type/sa_string_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaStringTypeEmitterTest : public testing::Test {
public:
    SaStringTypeEmitterTest() {}
    virtual ~SaStringTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaStringTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaStringTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaStringTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaStringTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaStringTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaStringTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_STRING);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaStringTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaStringTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "std::string");
    ret = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(ret, "const std::string&");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "std::string&");
    ret = emitter.EmitCppType(static_cast<TypeMode>(-1));
    EXPECT_EQ(ret, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaStringTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaStringTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!parcel.WriteString16(Str8ToStr16(value))) {\n"
        "    HiLog::Error(LABEL, \"Write [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaStringTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaStringTypeEmitter emitter;
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.WriteString16(Str8ToStr16(value))) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaStringTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    std::string expectedCode = "std::string value = Str16ToStr8(data.ReadString16());\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaStringTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaStringTypeEmitter emitter;
    std::string expectedCode = "value = Str16ToStr8(reply.ReadString16());\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("reply.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaStringTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    auto ret = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(ret, "str");
    ret = emitter.EmitRustType(false, TypeMode::NO_MODE);
    EXPECT_EQ(ret, "String");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaStringTypeEmitter EmitTsType without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    auto ret = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(ret, "string");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaStringTypeEmitter EmitTsWriteVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    std::string expectedCode = "parcel.writeString(value);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaStringTypeEmitter EmitTsReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStringTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaStringTypeEmitter emitter;
    std::string expectedCode = "let value = parcel.readString();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}
} // namespace OHOS::Idl