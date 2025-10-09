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

#include "codegen/SA/type/sa_enum_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaEnumTypeEmitterTest : public testing::Test {
public:
    SaEnumTypeEmitterTest() {}
    virtual ~SaEnumTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaEnumTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaEnumTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaEnumTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaEnumTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaEnumTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaEnumTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_ENUM);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaEnumTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "FooEnum");
    ret = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(ret, "const FooEnum&");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "FooEnum&");
    ret = emitter.EmitCppType(static_cast<TypeMode>(-1));
    EXPECT_EQ(ret, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaEnumTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!data.WriteInt32(static_cast<int32_t>(FooEnum))) {\n"
        "    HiLog::Error(LABEL, \"Write [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaEnumTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!data.WriteInt32(static_cast<int32_t>(FooEnum))) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaEnumTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.logOn_ = true;
    std::string expectedCode =
        "FooEnum value;\n"
        "{\n"
        "int32_t enumTmp = 0;\n"
        "if (!data.ReadInt32(enumTmp)) {\n"
        "    HiLog::Error(LABEL, \"Read [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "value = static_cast<FooEnum>(enumTmp);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaEnumTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.logOn_ = false;
    std::string expectedCode =
        "FooEnum value;\n"
        "{\n"
        "int32_t enumTmp = 0;\n"
        "if (!data.ReadInt32(enumTmp)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "value = static_cast<FooEnum>(enumTmp);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppTypeDecl_001
 * @tc.desc: test SaEnumTypeEmitter EmitCppTypeDecl
 * @tc.type: FUNC
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppTypeDecl_001, Level1)
{
    DTEST_LOG << "EmitCppTypeDecl_001 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.AddMember(new SaEnumValueEmitter("ENUM_ONE", std::string("")));
    emitter.AddMember(new SaEnumValueEmitter("ENUM_TWO", std::string("ONE")));
    std::string expectedCode =
        "enum class FooEnum{\n"
        "    ENUM_ONE,\n"
        "    ENUM_TWO = ONE,\n"
        "};\n";
    auto ret = emitter.EmitCppTypeDecl();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitCppTypeDecl_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppTypeDecl_002
 * @tc.desc: test SaEnumTypeEmitter EmitCppTypeDecl
 * @tc.type: FUNC
 */
HWTEST_F(SaEnumTypeEmitterTest, EmitCppTypeDecl_002, Level1)
{
    DTEST_LOG << "EmitCppTypeDecl_002 begin" << std::endl;
    SaEnumTypeEmitter emitter;
    emitter.SetTypeName("FooEnum");
    emitter.SetBaseTypeName("SaTypeEmitter");
    emitter.AddMember(new SaEnumValueEmitter("ENUM_ONE", std::string("")));
    emitter.AddMember(new SaEnumValueEmitter("ENUM_TWO", std::string("ONE")));
    std::string expectedCode =
        "enum class FooEnum : SaTypeEmitter{\n"
        "    ENUM_ONE,\n"
        "    ENUM_TWO = ONE,\n"
        "};\n";
    auto ret = emitter.EmitCppTypeDecl();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitCppTypeDecl_002 end" << std::endl;
}
} // namespace OHOS::idl