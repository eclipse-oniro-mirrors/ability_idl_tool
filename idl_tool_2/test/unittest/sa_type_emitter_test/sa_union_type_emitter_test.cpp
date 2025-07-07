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
#include "codegen/SA/type/sa_union_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaUnionTypeEmitterTest : public testing::Test {
public:
    SaUnionTypeEmitterTest() {}
    virtual ~SaUnionTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaUnionTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaUnionTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaUnionTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaUnionTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaUnionTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaUnionTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_UNION);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaUnionTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    emitter.SetTypeName("FooUnion");
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "FooUnion");
    ret = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(ret, "const FooUnion&");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "FooUnion&");
    ret = emitter.EmitCppType(static_cast<TypeMode>(-1));
    EXPECT_EQ(ret, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaUnionTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    emitter.SetTypeName("FooUnion");
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!data.WriteUnpadBuffer(&value, sizeof(FooUnion))) {\n"
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
 * @tc.desc: test SaUnionTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    emitter.SetTypeName("FooUnion");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!data.WriteUnpadBuffer(&value, sizeof(FooUnion))) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaUnionTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    emitter.SetTypeName("FooUnion");
    emitter.logOn_ = true;
    std::string expectedCode =
        "FooUnion value;\n"
        "const FooUnion *valueCp = reinterpret_cast<const FooUnion *>(data.ReadUnpadBuffer(sizeof(FooUnion)));\n"
        "if (valueCp == nullptr) {\n"
        "    HiLog::Error(LABEL, \"Read [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n"
        "if (memcpy_s(&value, sizeof(FooUnion), valueCp, sizeof(FooUnion)) != EOK) {\n"
        "    HiLog::Error(LABEL, \"Memcpy [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaUnionTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    emitter.SetTypeName("FooUnion");
    emitter.logOn_ = false;
    std::string expectedCode =
        "const FooUnion *valueCp = reinterpret_cast<const FooUnion *>(reply.ReadUnpadBuffer(sizeof(FooUnion)));\n"
        "if (valueCp == nullptr) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n"
        "if (memcpy_s(&value, sizeof(FooUnion), valueCp, sizeof(FooUnion)) != EOK) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("reply.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppTypeDecl_001
 * @tc.desc: test SaUnionTypeEmitter EmitCppTypeDecl
 * @tc.type: FUNC
 */
HWTEST_F(SaUnionTypeEmitterTest, EmitCppTypeDecl_001, Level1)
{
    DTEST_LOG << "EmitCppTypeDecl_001 begin" << std::endl;
    SaUnionTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.AddMember("unionInfo", intEmitter);
    emitter.SetTypeName("FooUnion");
    std::string expectedCode =
        "union FooUnion {\n"
        "    int32_t unionInfo;\n"
        "} __attribute__ ((aligned(8)));";
    auto ret = emitter.EmitCppTypeDecl();
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitCppTypeDecl_001 end" << std::endl;
}
} // namespace OHOS::idl