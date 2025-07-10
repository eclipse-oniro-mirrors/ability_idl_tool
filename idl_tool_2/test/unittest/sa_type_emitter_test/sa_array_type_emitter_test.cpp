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
#include "codegen/SA/type/sa_array_type_emitter.h"
#include "codegen/SA/type/sa_long_type_emitter.h"
#include "codegen/SA/type/sa_uchar_type_emitter.h"
#include "codegen/SA/type/sa_seq_type_emitter.h"
#include "codegen/SA/type/sa_string_type_emitter.h"
#include "codegen/SA/type/sa_cstring_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaArrayTypeEmitterTest : public testing::Test {
public:
    SaArrayTypeEmitterTest() {}
    virtual ~SaArrayTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaArrayTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaArrayTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaArrayTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaArrayTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaArrayTypeEmitter GetTypeKind
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_ARRAY);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaArrayTypeEmitter EmitCppType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "std::vector<int64_t>");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "std::vector<int64_t>");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const std::vector<int64_t>&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "std::vector<int64_t>&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "std::vector<int64_t>&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaArrayTypeEmitter EmitRustType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "[i64]");
    stringHelper = emitter.EmitRustType(false, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "Vec<i64>");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaArrayTypeEmitter EmitTsType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> ucharEmitter = new SaUcharTypeEmitter();
    emitter.SetElementEmitter(ucharEmitter);
    std::string stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "unknown type");

    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "number[]");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaArrayTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    std::string expectedCode =
        "if (fd.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The vector/array size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "parcel.WriteInt32(fd.size());\n"
        "for (auto it1 = fd.begin(); it1 != fd.end(); ++it1) {\n"
        "    if (!parcel.WriteInt64((*it1))) {\n"
        "        HiLog::Error(LABEL, \"Write [(*it1)] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaArrayTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    emitter.logOn_ = false;
    emitter.circleCount_ = 0;
    std::string expectedCode =
        "if (fd.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "parcel.WriteInt32(fd.size());\n"
        "for (auto it1 = fd.begin(); it1 != fd.end(); ++it1) {\n"
        "    if (!parcel.WriteInt64((*it1))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaArrayTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    std::string expectedCode =
        "std::vector<int64_t> fd.txt;\n"
        "int32_t txtSize = parcel.ReadInt32();\n"
        "if (txtSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The vector/array size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i1 = 0; i1 < txtSize; ++i1) {\n"
        "    int64_t value1 = parcel.ReadInt64();\n"
        "    fd.txt.push_back(value1);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd.txt", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaArrayTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    emitter.SetElementEmitter(seqEmitter);
    emitter.elementEmitter_->SetTypeName("long");
    std::string expectedCode =
        "std::vector<long> fd.txt;\n"
        "int32_t txtSize = parcel.ReadInt32();\n"
        "if (txtSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The vector/array size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i1 = 0; i1 < txtSize; ++i1) {\n"
        "    std::unique_ptr<long> value1(parcel.ReadParcelable<long>());\n"
        "    if (!value1) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n\n"
        "    fd.txt.push_back(*value1);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd.txt", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaArrayTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;

    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    emitter.elementEmitter_->SetTypeName("long");
    std::string expectedCode = "parcel.writeLongArray(fd.txt);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("parcel", "fd.txt", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> stringEmitter = new SaStringTypeEmitter();
    emitter.SetElementEmitter(stringEmitter);
    expectedCode = "parcel.writeStringArray(fd.txt);\n";
    StringBuilder sb1;
    emitter.EmitTsWriteVar("parcel", "fd.txt", sb1, "");
    EXPECT_EQ(sb1.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    emitter.SetElementEmitter(seqEmitter);
    emitter.elementEmitter_->SetTypeName("long");
    expectedCode =
        "let fd.txtArray:Array<long> = fd.txt;\n"
        "parcel.writeInt(fd.txtArray.length);\n"
        "for (let index = 0; index < fd.txtArray.length; index++) {\n"
        "    parcel.writeSequenceable(fd.txt[index]);\n"
        "}\n";
    StringBuilder sb2;
    emitter.EmitTsWriteVar("parcel", "fd.txt", sb2, "");
    EXPECT_EQ(sb2.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> cstringEmitter = new SaCStringTypeEmitter();
    emitter.SetElementEmitter(cstringEmitter);
    expectedCode = "";
    StringBuilder sb3;
    emitter.EmitTsWriteVar("parcel", "fd.txt", sb3, "");
    EXPECT_EQ(sb3.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaArrayTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaArrayTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;

    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    emitter.elementEmitter_->SetTypeName("long");
    std::string expectedCode = "let fd.txt = parcel.readLongArray();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("parcel", "fd.txt", sb, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> stringEmitter = new SaStringTypeEmitter();
    emitter.SetElementEmitter(stringEmitter);
    expectedCode = "let fd.txt = parcel.readStringArray();\n";
    StringBuilder sb1;
    emitter.EmitTsReadVar("parcel", "fd.txt", sb1, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb1.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    emitter.SetElementEmitter(seqEmitter);
    emitter.elementEmitter_->SetTypeName("long");
    expectedCode =
        "let txtSize = parcel.readInt();\n"
        "let fd.txt:Array<long> = [];\n"
        "for (let index = 0; index < txtSize; index++) {\n"
        "    let fd.txtValue = new long();\n"
        "    parcel.readSequenceable(fd.txtValue);\n"
        "    fd.txt.push(fd.txtValue);\n"
        "}\n";
    StringBuilder sb2;
    emitter.EmitTsReadVar("parcel", "fd.txt", sb2, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb2.ToString(), expectedCode);

    AutoPtr<SaTypeEmitter> cstringEmitter = new SaCStringTypeEmitter();
    emitter.SetElementEmitter(cstringEmitter);
    expectedCode = "";
    StringBuilder sb3;
    emitter.EmitTsReadVar("parcel", "fd.txt", sb3, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb3.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}

/*
 * @tc.name: SaListTypeEmitter_001
 * @tc.desc: test SaArrayTypeEmitter SaListTypeEmitter
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaArrayTypeEmitterTest, SaListTypeEmitter_001, Level1)
{
    DTEST_LOG << "SaListTypeEmitter_001 start" << std::endl;
    SaListTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter.SetElementEmitter(longEmitter);
    std::string expectedCode =
        "if (fd.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The list size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "parcel.WriteInt32(fd.size());\n"
        "for (auto it1 = fd.begin(); it1 != fd.end(); ++it1) {\n"
        "    if (!parcel.WriteInt64((*it1))) {\n"
        "        HiLog::Error(LABEL, \"Write [(*it1)] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);

    TypeKind type = emitter.GetTypeKind();
    EXPECT_EQ(type, TypeKind::TYPE_LIST);

    std::string ret = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(ret, "unknown type");
    DTEST_LOG << "SaListTypeEmitter_001 end" << std::endl;
}
} // namespace OHOS::idl