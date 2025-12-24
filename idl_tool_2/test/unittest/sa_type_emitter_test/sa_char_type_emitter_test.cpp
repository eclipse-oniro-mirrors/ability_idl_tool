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

#ifdef IDLTOOL_GTEST
#define private   public
#define protected public
#endif
#include "codegen/SA/type/sa_char_type_emitter.h"
#include "codegen/SA/type/sa_long_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaCharTypeEmitterTest : public testing::Test {
public:
    SaCharTypeEmitterTest() {}
    virtual ~SaCharTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaCharTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaCharTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaCharTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaCharTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaCharTypeEmitter GetTypeKind
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_CHAR);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaCharTypeEmitter EmitCppType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "char");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "char");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "char");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "char&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "char&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaCharTypeEmitter EmitRustType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "char");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaCharTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
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
 * @tc.desc: test SaCharTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaCharTypeEmitter emitter;
    emitter.logOn_ = false;
    emitter.circleCount_ = 0;
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
 * @tc.desc: test SaCharTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string expectedCode = "char fd = (char)parcel.ReadInt32();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaCharTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string expectedCode = "fd = (char)parcel.ReadInt32();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaCharTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string expectedCode = "parcel.writeInt(fd);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaCharTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaCharTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaCharTypeEmitter emitter;
    std::string expectedCode = "let fd = parcel.readInt();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd", sb, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}
} // namespace OHOS::Idl