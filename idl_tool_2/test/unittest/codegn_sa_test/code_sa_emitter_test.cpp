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
#define private public
#define protected public
#endif
#include "codegen/SA/sa_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class CodeSaEmitterTest : public testing::Test {
public:
    CodeSaEmitterTest() {}
    virtual ~CodeSaEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CodeSaEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void CodeSaEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void CodeSaEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void CodeSaEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test GetTypeKind function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_UNKNOWN);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test EmitCppType function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "unknow");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustType_001
 * @tc.desc: test EmitRustType function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitRustType_001, Level1)
{
    DTEST_LOG << "EmitRustType_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string type = emitter.EmitRustType(true, TypeMode::NO_MODE);
    EXPECT_EQ(type, "()");
    DTEST_LOG << "EmitRustType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppTypeDecl_001
 * @tc.desc: test EmitCppTypeDecl function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppTypeDecl_001, Level1)
{
    DTEST_LOG << "EmitCppTypeDecl_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string type = emitter.EmitCppTypeDecl();
    EXPECT_EQ(type, "");
    DTEST_LOG << "EmitCppTypeDecl_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test EmitCppWriteVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string parcelName;
    std::string name;
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitCppWriteVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test EmitCppReadVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string parcelName;
    std::string name;
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitCppReadVar(parcelName, name, sb, prefix, true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrWriteVar_001
 * @tc.desc: test EmitCppPtrWriteVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppPtrWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppPtrWriteVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "data", "", "sptr", true };
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitCppPtrWriteVar(emitCppPtrVar, sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppPtrReadVar_001
 * @tc.desc: test EmitCppPtrReadVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitCppPtrReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppPtrReadVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    EmitCppPtrVar emitCppPtrVar = { "parcel.", "data", "", "sptr", true };
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitCppPtrReadVar(emitCppPtrVar, sb, true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppPtrReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustReadVar_001
 * @tc.desc: test EmitRustReadVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitRustReadVar_001, Level1)
{
    DTEST_LOG << "EmitRustReadVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string result = "result";
    std::string name = "data";
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "let data : () = result.read()?;\n";
    emitter.EmitRustReadVar(result, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitRustReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitRustWriteVar_001
 * @tc.desc: test EmitRustWriteVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitRustWriteVar_001, Level1)
{
    DTEST_LOG << "EmitRustWriteVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string result = "result";
    std::string name = "data";
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "result.write(&data)?;\n";
    emitter.EmitRustWriteVar(result, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitRustWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsWriteVar_001
 * @tc.desc: test EmitTsWriteVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitTsWriteVar_001, Level1)
{
    DTEST_LOG << "EmitTsWriteVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string parcelName;
    std::string name;
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitTsWriteVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitTsReadVar_001
 * @tc.desc: test EmitTsReadVar function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeSaEmitterTest, EmitTsReadVar_001, Level1)
{
    DTEST_LOG << "EmitTsReadVar_001 begin" << std::endl;
    SaTypeEmitter emitter;
    std::string parcelName;
    std::string name;
    std::string prefix;
    StringBuilder sb;
    std::string expectedCode = "";
    emitter.EmitTsReadVar(parcelName, name, sb, prefix);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTsReadVar_001 end" << std::endl;
}
} // namespace OHOS::Idl