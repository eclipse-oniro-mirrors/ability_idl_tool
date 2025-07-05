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

#include "codegen/SA/type/sa_ushort_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaUshortTypeEmitterTest : public testing::Test {
public:
    SaUshortTypeEmitterTest() {}
    virtual ~SaUshortTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaUshortTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaUshortTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaUshortTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaUshortTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaUshortTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaUshortTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_USHORT);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaUshortTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaUshortTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "uint16_t");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "uint16_t&");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaUshortTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaUshortTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (!parcel.WriteUint16(value)) {\n"
        "    HiLog::Error(LABEL, \"Write [value] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaUshortTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUshortTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (!parcel.WriteUint16(value)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaUshortTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUshortTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    std::string expectedCode = "uint16_t value = data.ReadUint16();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaUshortTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaUshortTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaUshortTypeEmitter emitter;
    std::string expectedCode = "value = reply.ReadUint16();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("reply.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}
} // namespace OHOS::idl