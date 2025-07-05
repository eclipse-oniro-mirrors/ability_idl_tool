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
#include "codegen/SA/type/sa_set_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaSetTypeEmitterTest : public testing::Test {
public:
    SaSetTypeEmitterTest() {}
    virtual ~SaSetTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaSetTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaSetTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaSetTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaSetTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaSetTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaSetTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaSetTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_SET);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaSetTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaSetTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaSetTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetElementEmitter(intEmitter);
    auto ret = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(ret, "std::set<int32_t>");
    ret = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(ret, "const std::set<int32_t>&");
    ret = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(ret, "std::set<int32_t>&");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaSetTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaSetTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaSetTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetElementEmitter(intEmitter);
    emitter.logOn_ = true;
    std::string expectedCode =
        "if (value.size() > static_cast<size_t>(SET_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The set size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "data.WriteInt32(value.size());\n"
        "for (auto it1 = value.begin(); it1 != value.end(); ++it1) {\n"
        "    if (!data.WriteInt32((*it1))) {\n"
        "        HiLog::Error(LABEL, \"Write [(*it1)] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaSetTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaSetTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaSetTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetElementEmitter(intEmitter);
    emitter.logOn_ = false;
    std::string expectedCode =
        "if (value.size() > static_cast<size_t>(SET_MAX_SIZE)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "data.WriteInt32(value.size());\n"
        "for (auto it2 = value.begin(); it2 != value.end(); ++it2) {\n"
        "    if (!data.WriteInt32((*it2))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("data.", "value", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_002 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaSetTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaSetTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaSetTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetElementEmitter(intEmitter);
    emitter.logOn_ = true;
    std::string expectedCode =
        "std::set<int32_t> value;\n"
        "int32_t valueSize = data.ReadInt32();\n"
        "if (valueSize > static_cast<int32_t>(SET_MAX_SIZE)) {\n"
        "    HiLog::Error(LABEL, \"The set size exceeds the security limit!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i3 = 0; i3 < valueSize; ++i3) {\n"
        "    int32_t value3 = data.ReadInt32();\n"
        "    value.insert(value3);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("data.", "value", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaSetTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaSetTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaSetTypeEmitter emitter;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetElementEmitter(intEmitter);
    emitter.logOn_ = false;
    std::string expectedCode =
        "int32_t valueSize = reply.ReadInt32();\n"
        "if (valueSize > static_cast<int32_t>(SET_MAX_SIZE)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n"
        "for (int32_t i4 = 0; i4 < valueSize; ++i4) {\n"
        "    int32_t value4 = reply.ReadInt32();\n"
        "    value.insert(value4);\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("reply.", "value", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}
} // namespace OHOS::idl