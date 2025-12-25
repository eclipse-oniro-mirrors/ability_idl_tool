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
#include "codegen/SA/type/sa_rawdata_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaRawDataTypeEmitterTest : public testing::Test {
public:
    SaRawDataTypeEmitterTest() {}
    virtual ~SaRawDataTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaRawDataTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaRawDataTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaRawDataTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaRawDataTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaRawDataTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_RAWDATA);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaRawDataTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    emitter.SetTypeName("long");
    std::string type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const long&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "long&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "long&");
    type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "long");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "long");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaRawDataTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!parcel.WriteUint32(fd.size)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (!parcel.WriteRawData(fd.data, fd.size)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd RawData] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaRawDataTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.WriteUint32(fd.size)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (!parcel.WriteRawData(fd.data, fd.size)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaRawDataTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode =
        "long fd;\n"
        "if (!parcel.ReadUint32(fd.size)) {\n"
        "    HiLog::Error(LABEL, \"Read [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "auto readfd = parcel.ReadRawData(fd.size);\n"
        "if (readfd == nullptr) {\n"
        "    HiLog::Error(LABEL, \"Read [fd RawData] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "ErrCode fdoutError = fd.RawDataCpy(readfd);\n"
        "if (fdoutError) {\n"
        "    HiLog::Error(LABEL, \"RawDataCpy [fd] failed!\");\n"
        "    return fdoutError;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaRawDataTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaRawDataTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaRawDataTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.ReadUint32(fd.size)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "auto readfd = parcel.ReadRawData(fd.size);\n"
        "if (readfd == nullptr) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "ErrCode fdoutError = fd.RawDataCpy(readfd);\n"
        "if (fdoutError) {\n"
        "    return fdoutError;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}
} // namespace OHOS::Idl