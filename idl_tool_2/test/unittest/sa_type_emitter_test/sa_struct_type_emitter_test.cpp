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

#include "codegen/SA/type/sa_int_type_emitter.h"
#include "codegen/SA/type/sa_struct_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaStructTypeEmitterTest : public testing::Test {
public:
    SaStructTypeEmitterTest() {}
    virtual ~SaStructTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaStructTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaStructTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaStructTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaStructTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaStructTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaStructTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_STRUCT);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaStructTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "FooStruct");
    ret = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(ret, "const FooStruct&");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "FooStruct&");
    ret = emitter.EmitCppType(static_cast<TypeMode>(-1));
    EXPECT_EQ(ret, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaStructTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (FooStructBlockMarshalling(data, value) != ERR_NONE) {\n"
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
 * @tc.desc: test SaStructTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (FooStructBlockMarshalling(reply, value) != ERR_NONE) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("reply.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_003
 * @tc.desc: test SaStructTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppWriteVar_003, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_003 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (FooStructBlockMarshalling(, value) != ERR_NONE) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("code.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaStructTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = true;
    std::string expectedCode =
        "FooStruct value;\n"
        "if (FooStructBlockUnmarshalling(data, value) != ERR_NONE) {\n"
        "    HiLog::Error(LABEL, \"Read [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaStructTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (FooStructBlockUnmarshalling(reply, value) != ERR_NONE) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("reply.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_003
 * @tc.desc: test SaStructTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppReadVar_003, Level1)
{
    DTEST_LOG << "EmitCppReadVar_003 begin" << std::endl;
    SaStructTypeEmitter emitter;
    emitter.SetTypeName("FooStruct");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (FooStructBlockUnmarshalling(, value) != ERR_NONE) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("code.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppTypeDecl_001
 * @tc.desc: test SaStructTypeEmitter EmitCppTypeDecl
 * @tc.type: FUNC
 */
HWTEST_F(SaStructTypeEmitterTest, EmitCppTypeDecl_001, Level1)
{
    DTEST_LOG << "EmitCppTypeDecl_001 begin" << std::endl;
    SaStructTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.AddMember("id", intEmitter);
    emitter.SetTypeName("FooStruct");
    std::string expectedCode =
        "struct FooStruct {\n"
        "    int32_t id;\n"
        "};";
    auto ret = emitter.EmitCppTypeDecl();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitCppTypeDecl_001 end" << std::endl;
}
} // namespace OHOS::idl