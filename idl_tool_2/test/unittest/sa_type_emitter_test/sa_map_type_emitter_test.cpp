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
#include "codegen/SA/type/sa_int_type_emitter.h"
#include "codegen/SA/type/sa_map_type_emitter.h"
#include "codegen/SA/type/sa_uchar_type_emitter.h"
#include "codegen/SA/type/sa_seq_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaMapTypeEmitterTest : public testing::Test {
public:
    SaMapTypeEmitterTest() {}
    virtual ~SaMapTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaMapTypeEmitterTest::SetUpTestCase() {}

void SaMapTypeEmitterTest::TearDownTestCase() {}

void SaMapTypeEmitterTest::SetUp() {}

void SaMapTypeEmitterTest::TearDown() {}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaMapTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_MAP);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaMapTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);

    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "std::unordered_map<int32_t, int32_t>");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "std::unordered_map<int32_t, int32_t>");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const std::unordered_map<int32_t, int32_t>&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "std::unordered_map<int32_t, int32_t>&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "std::unordered_map<int32_t, int32_t>&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaMapTypeEmitter EmitRustType
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    std::string stringHelper = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "HashMap<i32, i32>");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaMapTypeEmitter EmitTsType
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    std::string stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "Map<number, number>");

    AutoPtr<SaTypeEmitter> ucharEmitter = new SaUcharTypeEmitter();
    emitter.SetKeyEmitter(ucharEmitter);
    emitter.SetValueEmitter(ucharEmitter);
    stringHelper = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(stringHelper, "unknown type");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaMapTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    std::string expectedCode =
        "if (data.size() > static_cast<size_t>(MAP_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "\n"
        "parcel.WriteInt32(data.size());\n"
        "for (auto it1 = data.begin(); it1 != data.end(); ++it1) {\n"
        "    if (!parcel.WriteInt32((it1->first))) {\n"
        "        HiLog::Error(LABEL, \"Write [(it1->first)] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    if (!parcel.WriteInt32((it1->second))) {\n"
        "        HiLog::Error(LABEL, \"Write [(it1->second)] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "data", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaMapTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaMapTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.logOn_ = false;
    emitter.circleCount_ = 0;
    std::string expectedCode =
        "if (data.size() > static_cast<size_t>(MAP_MAX_SIZE)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "\n"
        "parcel.WriteInt32(data.size());\n"
        "for (auto it1 = data.begin(); it1 != data.end(); ++it1) {\n"
        "    if (!parcel.WriteInt32((it1->first))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    if (!parcel.WriteInt32((it1->second))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "data", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaMapTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "inParam.txt";
    std::string prefix = "    ";
    bool emitType = true;

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode =
        "    std::unordered_map<int32_t, int32_t> inParam.txt;\n"
        "    int32_t txtSize = parcel.ReadInt32();\n"
        "    if (txtSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
        "        HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    for (int32_t i1 = 0; i1 < txtSize; ++i1) {\n"
        "        int32_t key1 = parcel.ReadInt32();\n"
        "        int32_t value1 = parcel.ReadInt32();\n"
        "        inParam.txt[key1] = value1;\n"
        "    }\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaMapTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "inParam";
    std::string prefix = "    ";
    bool emitType = true;

    emitter.logOn_ = false;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode =
        "    std::unordered_map<int32_t, int32_t> inParam;\n"
        "    int32_t inParamSize = parcel.ReadInt32();\n"
        "    if (inParamSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    for (int32_t i2 = 0; i2 < inParamSize; ++i2) {\n"
        "        int32_t key2 = parcel.ReadInt32();\n"
        "        int32_t value2 = parcel.ReadInt32();\n"
        "        inParam[key2] = value2;\n"
        "    }\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_003
 * @tc.desc: test SaMapTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppReadVar_003, Level1)
{
    DTEST_LOG << "EmitCppReadVar_003 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "data";
    std::string prefix = "";
    bool emitType = false;

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    seqEmitter->SetTypeName("IRemoteObject");
    emitter.SetKeyEmitter(seqEmitter);
    emitter.SetValueEmitter(seqEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode =
        "int32_t dataSize = parcel.ReadInt32();\n"
        "if (dataSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i1 = 0; i1 < dataSize; ++i1) {\n"
        "    sptr<IRemoteObject> key1 = parcel.ReadRemoteObject();\n"
        "    if (!key1) {\n"
        "        HiLog::Error(LABEL, \"Read [IRemoteObject] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n\n"
        "    sptr<IRemoteObject> value1 = parcel.ReadRemoteObject();\n"
        "    if (!value1) {\n"
        "        HiLog::Error(LABEL, \"Read [IRemoteObject] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n\n"
        "    data[key1] = value1;\n"
        "}\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_004
 * @tc.desc: test SaMapTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppReadVar_004, Level1)
{
    DTEST_LOG << "EmitCppReadVar_004 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "data";
    std::string prefix = "";
    bool emitType = true;

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    seqEmitter->SetTypeName("long");
    emitter.SetKeyEmitter(seqEmitter);
    emitter.SetValueEmitter(seqEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode =
        "std::unordered_map<long, long> data;\n"
        "int32_t dataSize = parcel.ReadInt32();\n"
        "if (dataSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i1 = 0; i1 < dataSize; ++i1) {\n"
        "    std::unique_ptr<long> key1(parcel.ReadParcelable<long>());\n"
        "    if (!key1) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n\n"
        "    std::unique_ptr<long> value1(parcel.ReadParcelable<long>());\n"
        "    if (!value1) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n\n"
        "    data[*key1] = *value1;\n"
        "}\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_004 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_005
 * @tc.desc: test SaMapTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitCppReadVar_005, Level1)
{
    DTEST_LOG << "EmitCppReadVar_005 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "data";
    std::string prefix = "";
    bool emitType = true;

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    intEmitter->SetTypeName("IRemoteObject");
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode =
        "std::unordered_map<int32_t, int32_t> data;\n"
        "int32_t dataSize = parcel.ReadInt32();\n"
        "if (dataSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i1 = 0; i1 < dataSize; ++i1) {\n"
        "    int32_t key1 = parcel.ReadInt32();\n"
        "    int32_t value1 = parcel.ReadInt32();\n"
        "    data[key1] = value1;\n"
        "}\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_005 end" << std::endl;
}

/*
 * @tc.name: EmitRustReadVar_001
 * @tc.desc: test SaMapTypeEmitter EmitRustReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitRustReadVar_001, Level1)
{
    DTEST_LOG << "EmitRustReadVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "result";
    std::string name = "data";
    std::string prefix = "";

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);

    std::string expectedCode =
        "let mut data = HashMap::new();\n"
        "let len = result.read()?;\n"
        "for i in 0..len {\n"
        "    let datak : i32 = result.read()?;\n"
        "    let datav : i32 = result.read()?;\n"
        "    data.insert(datak, datav);\n"
        "}\n";
    
    emitter.EmitRustReadVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitRustReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustWriteVar_001
 * @tc.desc: test SaMapTypeEmitter EmitRustWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitRustWriteVar_001, Level1)
{
    DTEST_LOG << "EmitRustWriteVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "result";
    std::string name = "data";
    std::string prefix = "";

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    
    std::string expectedCode =
        "result.write(&(data.len() as u32))?;\n"
        "for (key, value) in data.iter() {\n"
        "    result.write(&key)?;\n"
        "    result.write(&value)?;\n"
        "}\n";
    
    emitter.EmitRustWriteVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitRustWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaMapTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel";
    std::string name = "data";
    std::string prefix = "";

    emitter.logOn_ = true;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);

    std::string expectedCode =
        "parcel.writeInt(data.size);\n"
        "for (let [key, value] of data) {\n"
        "    parcel.writeInt(key);\n"
        "    parcel.writeInt(value);\n"
        "}\n";
    
    emitter.EmitTsWriteVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaMapTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel";
    std::string name = "data.txt";
    std::string prefix = "";

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);

    std::string expectedCode =
        "let data.txt = new Map();\n"
        "let txtSize = parcel.readInt();\n"
        "for (let i = 0; i < txtSize; ++i) {\n"
        "    let key = parcel.readInt();\n"
        "    let value = parcel.readInt();\n"
        "    data.txt.set(key, value);\n"
        "}\n";
    
    emitter.EmitTsReadVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_002
 * @tc.desc: test SaMapTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaMapTypeEmitterTest, EmitTsReadVar_002, Level1)
{
    DTEST_LOG << "EmitTsReadVar_002 begin" << std::endl;
    SaMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel";
    std::string name = "data";
    std::string prefix = "";

    emitter.logOn_ = true;
    emitter.circleCount_ = 0;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);

    std::string expectedCode =
        "let data = new Map();\n"
        "let dataSize = parcel.readInt();\n"
        "for (let i = 0; i < dataSize; ++i) {\n"
        "    let key = parcel.readInt();\n"
        "    let value = parcel.readInt();\n"
        "    data.set(key, value);\n"
        "}\n";
    
    emitter.EmitTsReadVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_002 end" << std::endl;
}
} // namespace OHOS::Idl