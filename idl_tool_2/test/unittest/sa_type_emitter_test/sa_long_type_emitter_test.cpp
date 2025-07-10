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
#include "codegen/SA/type/sa_long_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaLongTypeEmitterTest : public testing::Test {
public:
    SaLongTypeEmitterTest() {}
    virtual ~SaLongTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaLongTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaLongTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaLongTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaLongTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaLongTypeEmitter GetTypeKind
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_LONG);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaLongTypeEmitter EmitCppType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "int64_t");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "int64_t");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "int64_t");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "int64_t&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "int64_t&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaLongTypeEmitter EmitRustType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "i64");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaLongTypeEmitter EmitTsType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "number");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaLongTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!parcel.WriteInt64(fd)) {\n"
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
 * @tc.desc: test SaLongTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaLongTypeEmitter emitter;
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.WriteInt64(fd)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaLongTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string expectedCode = "int64_t fd = parcel.ReadInt64();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaLongTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string expectedCode = "fd = parcel.ReadInt64();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaLongTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string expectedCode = "parcel.writeLong(fd);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaLongTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaLongTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaLongTypeEmitter emitter;
    std::string expectedCode = "let fd = parcel.readLong();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd", sb, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}
} // namespace OHOS::idl