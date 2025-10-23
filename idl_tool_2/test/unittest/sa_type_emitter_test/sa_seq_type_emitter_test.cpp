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
#include "codegen/SA/sa_type_emitter.h"
#include "codegen/SA/type/sa_long_type_emitter.h"
#include "codegen/SA/type/sa_seq_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaSeqTypeEmitterTest : public testing::Test {
public:
    SaSeqTypeEmitterTest() {}
    virtual ~SaSeqTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaSeqTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaSeqTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaSeqTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaSeqTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaSeqTypeEmitter GetTypeKind
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_SEQUENCEABLE);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaSeqTypeEmitter EmitCppType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
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

    emitter.SetTypeName("IRemoteObject");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const sptr<IRemoteObject>&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "sptr<IRemoteObject>&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "sptr<IRemoteObject>&");
    type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "sptr<IRemoteObject>");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "sptr<IRemoteObject>");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaSeqTypeEmitter EmitRustType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.SetTypeName("long");
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "long");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaSeqTypeEmitter EmitTsType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.SetTypeName("long");
    std::string stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "long");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode =
        "if (!parcel.WriteRemoteObject(fd)) {\n"
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
 * @tc.desc: test SaSeqTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isParamInout = true;
    emitter.isProxy = false;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "if (!parcel.WriteParcelable(fd.get())) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_003
 * @tc.desc: test SaSeqTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppWriteVar_003, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_003 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isParamInout = false;
    emitter.isProxy = false;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "if (!parcel.WriteParcelable(&fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_004
 * @tc.desc: test SaSeqTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppWriteVar_004, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_004 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isParamInout = false;
    emitter.isProxy = true;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "if (!parcel.WriteParcelable(&fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_004 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_005
 * @tc.desc: test SaSeqTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppWriteVar_005, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_005 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = false;
    emitter.isParamInout = true;
    emitter.isProxy = true;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "if (!parcel.WriteParcelable(&fd)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_005 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode =
        "sptr<IRemoteObject> fd = parcel.ReadRemoteObject();\n"
        "if (!fd) {\n"
        "    HiLog::Error(LABEL, \"Read [IRemoteObject] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode = "fd = parcel.ReadRemoteObject();\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_003
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_003, Level1)
{
    DTEST_LOG << "EmitCppReadVar_003 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = false;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode =
        "sptr<IRemoteObject> fd = parcel.ReadRemoteObject();\n"
        "if (!fd) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_004
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_004, Level1)
{
    DTEST_LOG << "EmitCppReadVar_004 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "std::unique_ptr<long> fd(parcel.ReadParcelable<long>());\n"
        "if (!fd) {\n"
        "    HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_004 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_005
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_005, Level1)
{
    DTEST_LOG << "EmitCppReadVar_005 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "std::unique_ptr<long> fdInfo(parcel.ReadParcelable<long>());\n"
        "if (fdInfo != nullptr) {\n"
        "    fd = *fdInfo;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_005 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_006
 * @tc.desc: test SaSeqTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppReadVar_006, Level1)
{
    DTEST_LOG << "EmitCppReadVar_006 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = false;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "std::unique_ptr<long> fd(parcel.ReadParcelable<long>());\n"
        "if (!fd) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_006 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrWriteVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppPtrWriteVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "fd", "", "sptr", true };
    std::string expectedCode =
        "bool fdValid = fd != nullptr;\n"
        "if (!parcel.WriteBool(fdValid)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (fdValid) {\n"
        "    if (!parcel.WriteParcelable(fd)) {\n"
        "        HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrWriteVar(emitCppPtrVar, sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrWriteVar_002
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppPtrWriteVar_002 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "fd", "", "uptr", true };
    std::string expectedCode =
        "bool fdValid = fd != nullptr;\n"
        "if (!parcel.WriteBool(fdValid)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (fdValid) {\n"
        "    if (!parcel.WriteParcelable(fd.get())) {\n"
        "        HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrWriteVar(emitCppPtrVar, sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrWriteVar_003
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrWriteVar_003, Level1)
{
    DTEST_LOG << "EmitCppPtrWriteVar_003 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "fd", "", "uptr", false };
    std::string expectedCode =
        "if (!parcel.WriteParcelable(fd.get())) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrWriteVar(emitCppPtrVar, sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrWriteVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrWriteVar_004
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrWriteVar_004, Level1)
{
    DTEST_LOG << "EmitCppPtrWriteVar_004 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = false;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "fd", "", "uptr", true };
    std::string expectedCode =
        "bool fdValid = fd != nullptr;\n"
        "if (!parcel.WriteBool(fdValid)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (fdValid) {\n"
        "    if (!parcel.WriteParcelable(fd.get())) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrWriteVar(emitCppPtrVar, sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrWriteVar_004 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "ptr", "", "sptr", true };
    emitter.SetTypeName("long");
    std::string expectedCode =
        "sptr<long> ptr;\n"
        "if (parcel.ReadBool()) {\n"
        "    ptr = sptr<long>(parcel.ReadParcelable<long>());\n"
        "    if (!ptr) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_002
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_002 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "ptr", "", "sptr", false };
    emitter.SetTypeName("long");
    std::string expectedCode =
        "sptr<long> "
        "ptr = sptr<long>(parcel.ReadParcelable<long>());\n"
        "if (!ptr) {\n"
        "    HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_003
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrReadVar_003, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_003 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "ptr", "", "sptr", true };
    emitter.SetTypeName("long");
    std::string expectedCode =
        "if (parcel.ReadBool()) {\n"
        "    ptr = sptr<long>(parcel.ReadParcelable<long>());\n"
        "    if (!ptr) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_004
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrReadVar_004, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_004 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = true;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "ptr", "", "sptr", false };
    emitter.SetTypeName("long");
    std::string expectedCode =
        "ptr = sptr<long>(parcel.ReadParcelable<long>());\n"
        "if (!ptr) {\n"
        "    HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_004 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_005
 * @tc.desc: test SaSeqTypeEmitter EmitCppPtrReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitCppPtrReadVar_005, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_005 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.logOn_ = false;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "ptr", "", "sptr", false };
    emitter.SetTypeName("long");
    std::string expectedCode =
        "ptr = sptr<long>(parcel.ReadParcelable<long>());\n"
        "if (!ptr) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_005 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode = "parcel.writeRemoteObject(fd);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);

    emitter.SetTypeName("long");
    expectedCode = "parcel.writeParcelable(fd);\n";
    StringBuilder sb2;
    emitter.EmitTsWriteVar("parcel", "fd", sb2, "");
    EXPECT_EQ(sb2.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaSeqTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.SetTypeName("IRemoteObject");
    std::string expectedCode = "IRemoteObject fd = parcel.readRemoteObject();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd", sb, "", TypeMode::PARAM_IN);
    EXPECT_EQ(sb.ToString(), expectedCode);
    expectedCode = "fd = parcel.readRemoteObject();\n";
    StringBuilder sb2;
    emitter.EmitTsReadVar("parcel", "fd", sb2, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb2.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_002
 * @tc.desc: test SaSeqTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaSeqTypeEmitterTest, EmitTsReadVar_002, Level1)
{
    DTEST_LOG << "EmitTsReadVar_002 begin" << std::endl;
    SaSeqTypeEmitter emitter;
    emitter.SetTypeName("long");
    std::string expectedCode =
        "let fd = new long();\n"
        "parcel.readParcelable(fd);\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd", sb, "", TypeMode::PARAM_IN);
    EXPECT_EQ(sb.ToString(), expectedCode);
    expectedCode =
        "let fd = new long();\n"
        "parcel.readSequenceable(fd);\n";
    StringBuilder sb2;
    emitter.EmitTsReadVar("parcel", "fd", sb2, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb2.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_002 end" << std::endl;
}
} // namespace OHOS::idl