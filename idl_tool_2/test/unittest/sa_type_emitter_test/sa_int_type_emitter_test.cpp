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

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaIntTypeEmitterTest : public testing::Test {
public:
    SaIntTypeEmitterTest() {}
    virtual ~SaIntTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaIntTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaIntTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaIntTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaIntTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaIntTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_INT);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaIntTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "int32_t");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "int32_t");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "int32_t");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "int32_t&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "int32_t&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaIntTypeEmitter EmitRustType
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "i32");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaIntTypeEmitter EmitTsType
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "number");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaIntTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!parcel.WriteInt32(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaIntTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaIntTypeEmitter emitter;
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.WriteInt32(fd)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaIntTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string expectedCode = "int32_t fd = parcel.ReadInt32();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaIntTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string expectedCode = "fd = parcel.ReadInt32();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaIntTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string expectedCode = "parcel.writeInt(fd);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_002
 * @tc.desc: test SaIntTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaIntTypeEmitterTest, EmitTsReadVar_002, Level1)
{
    DTEST_LOG << "EmitTsReadVar_002 begin" << std::endl;
    SaIntTypeEmitter emitter;
    std::string expectedCode = "let fd = parcel.readInt();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd", sb, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_002 end" << std::endl;
}
} // namespace OHOS::Idl