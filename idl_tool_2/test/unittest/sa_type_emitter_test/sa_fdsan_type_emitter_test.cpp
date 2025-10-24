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

#include "codegen/SA/type/sa_fdsan_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaFdSanTypeEmitterTest : public testing::Test {
public:
    SaFdSanTypeEmitterTest() {}
    virtual ~SaFdSanTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaFdSanTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaFdSanTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaFdSanTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaFdSanTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaFdSanTypeEmitter GetTypeKind
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    auto ret = emitter.GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_FILEDESCRIPTORSAN);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaFdSanTypeEmitter EmitCppType
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    std::string type = emitter.EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "int");
    type = emitter.EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "int");
    type = emitter.EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "int");
    type = emitter.EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "int&");
    type = emitter.EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "int&");
    type = emitter.EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaFdSanTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = false;
    emitter.domainId_ = "testDomainId";
    std::string expectedCode =
        "if (!parcel.WriteFileDescriptor(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    if (fd >= 0) {\n"
        "        fdsan_close_with_tag(fd, testDomainId);\n"
        "    }\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_002
 * @tc.desc: test SaFdSanTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppWriteVar_002, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_002 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = true;
    emitter.domainId_ = "testDomainId";
    std::string expectedCode =
        "if (!parcel.WriteFileDescriptor(fd)) {\n"
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
 * @tc.desc: test SaFdSanTypeEmitter EmitCppWriteVar without log
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppWriteVar_003, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_003 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    emitter.logOn_ = false;
    emitter.isProxy = true;
    emitter.domainId_ = "testDomainId";
    std::string expectedCode =
        "if (!parcel.WriteFileDescriptor(fd)) {\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_003 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaFdSanTypeEmitter EmitCppReadVar with type
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    std::string expectedCode = "int fd = parcel.ReadFileDescriptor();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_002
 * @tc.desc: test SaFdSanTypeEmitter EmitCppReadVar without type
 * @tc.type: FUNC
 */
HWTEST_F(SaFdSanTypeEmitterTest, EmitCppReadVar_002, Level1)
{
    DTEST_LOG << "EmitCppReadVar_002 begin" << std::endl;
    SaFdSanTypeEmitter emitter;
    std::string expectedCode = "fd = parcel.ReadFileDescriptor();\n";
    StringBuilder sb;
    emitter.EmitCppReadVar("parcel.", "fd", sb, "", false);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_002 end" << std::endl;
}
} // namespace OHOS::idl