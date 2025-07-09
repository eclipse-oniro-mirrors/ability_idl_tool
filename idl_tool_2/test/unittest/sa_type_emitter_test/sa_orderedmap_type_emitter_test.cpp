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

#include "codegen/SA/type/sa_int_type_emitter.h"
#include "codegen/SA/type/sa_orderedmap_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaOrderedMapTypeEmitterTest : public testing::Test {
public:
    SaOrderedMapTypeEmitterTest() {}
    virtual ~SaOrderedMapTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaOrderedMapTypeEmitterTest::SetUpTestCase() {}

void SaOrderedMapTypeEmitterTest::TearDownTestCase() {}

void SaOrderedMapTypeEmitterTest::SetUp() {}

void SaOrderedMapTypeEmitterTest::TearDown() {}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test EmitCppReadVar EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaOrderedMapTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    SaOrderedMapTypeEmitter emitter;
    StringBuilder sb;
    std::string parcelName = "parcel.";
    std::string name = "inParam";
    std::string prefix = "    ";
    bool emitType = true;

    emitter.logOn_ = true;
    AutoPtr<SaTypeEmitter> intEmitter = new SaIntTypeEmitter();
    emitter.SetKeyEmitter(intEmitter);
    emitter.SetValueEmitter(intEmitter);
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, emitType);

    std::string expectedCode = "    std::map<int32_t, int32_t> inParam;\n"
                               "    int32_t inParamSize = parcel.ReadInt32();\n"
                               "    if (inParamSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
                               "        HiLog::Error(LABEL, \"The map size exceeds the security limit!\");\n"
                               "        return ERR_INVALID_DATA;\n"
                               "    }\n"
                               "\n"
                               "    for (int32_t i1 = 0; i1 < inParamSize; ++i1) {\n"
                               "        int32_t key1 = parcel.ReadInt32();\n"
                               "        int32_t value1 = parcel.ReadInt32();\n"
                               "        inParam[key1] = value1;\n"
                               "    }\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test EmitCppReadVar EmitCppReadVar
 * @tc.type: FUNC
 */
HWTEST_F(SaOrderedMapTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    SaOrderedMapTypeEmitter emitter;
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

    std::string expectedCode = "    std::map<int32_t, int32_t> inParam;\n"
                               "    int32_t inParamSize = parcel.ReadInt32();\n"
                               "    if (inParamSize > static_cast<int32_t>(MAP_MAX_SIZE)) {\n"
                               "        return ERR_INVALID_DATA;\n"
                               "    }\n"
                               "\n"
                               "    for (int32_t i2 = 0; i2 < inParamSize; ++i2) {\n"
                               "        int32_t key2 = parcel.ReadInt32();\n"
                               "        int32_t value2 = parcel.ReadInt32();\n"
                               "        inParam[key2] = value2;\n"
                               "    }\n";

    EXPECT_EQ(sb.ToString(), expectedCode);
}
} // namespace OHOS::idl