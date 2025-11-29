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
#include "codegen/SA/type/sa_interface_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaInterfaceTypeEmitterTest : public testing::Test {
public:
    SaInterfaceTypeEmitterTest() {}
    virtual ~SaInterfaceTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaInterfaceTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaInterfaceTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaInterfaceTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaInterfaceTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaInterfaceTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_INTERFACE);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "sptr<long>");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "sptr<long>");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const sptr<long>&");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "sptr<long>&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "sptr<long>&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitRustType
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    auto ret = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(ret, "long");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsType_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitTsType
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitTsType_001, Level1)
{
    DTEST_LOG << "EmitTsType_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    auto ret = emitter.EmitTsType(TypeMode::NO_MODE);
    EXPECT_EQ(ret, "long");
    DTEST_LOG << "EmitTsType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (value == nullptr) {\n"
        "    HiLog::Error(LABEL, \"value is nullptr!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (!data.WriteRemoteObject(value->AsObject())) {\n"
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
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (value == nullptr) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "if (!data.WriteRemoteObject(value->AsObject())) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode =
        "sptr<long> value = iface_cast<long>(data.ReadRemoteObject());\n"
        "if (value == nullptr) {\n"
        "    HiLog::Error(LABEL, \"Read [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = false;
    std::string expectedCode =
        "sptr<long> value = iface_cast<long>(data.ReadRemoteObject());\n"
        "if (value == nullptr) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_003
 * @tc.desc: test SaInterfaceTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitCppReadVar_003, Level1)
{
    DTEST_LOG << "EmitCppReadVar_003 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode =
        "value = iface_cast<long>(data.ReadRemoteObject());\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_003 end" << std::endl;
}

/*
 * @tc.name: StubName_001
 * @tc.desc: test SaInterfaceTypeEmitter StubName
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, StubName_001, Level1)
{
    DTEST_LOG << "StubName_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    std::string expectedCode = "RemoteStub";
    std::string sb = emitter.StubName("IRemote");
    EXPECT_EQ(sb, expectedCode);

    expectedCode = "UVStub";
    sb = emitter.StubName("UV");
    EXPECT_EQ(sb, expectedCode);
    DTEST_LOG << "StubName_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitTsWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode = "data.writeRemoteObject(value as longStub);\n";
    StringBuilder sb;
    emitter.EmitTsWriteVar("data", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test SaInterfaceTypeEmitter EmitTsReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaInterfaceTypeEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaInterfaceTypeEmitter emitter;
    emitter.SetTypeName("long");
    emitter.logOn_ = true;
    std::string expectedCode = "let value = data.readRemoteObject();\n";
    StringBuilder sb;
    emitter.EmitTsReadVar("data", "value", sb, "", TypeMode::NO_MODE);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}
} // namespace OHOS::idl