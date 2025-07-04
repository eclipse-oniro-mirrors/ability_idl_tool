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

#include "sa_type_emitter_test.h"
#include "codegen/SA/type/sa_fd_type_emitter.h"
#include "codegen/SA/type/sa_fdsan_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS {

void SaTypeEmitterTest::SetUpTestCase() {}

void SaTypeEmitterTest::TearDownTestCase() {}

void SaTypeEmitterTest::SetUp() {}

void SaTypeEmitterTest::TearDown() {}

/*
 * @tc.name: SaFdTypeEmitterTest_001
 * @tc.desc: test SaFdTypeEmitter EmitCppWriteVar stub
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaTypeEmitterTest, SaFdTypeEmitterTest_001, Level1)
{
    SaFdTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = false;
    std::string expectedCode =
        "if (!parcelWriteFileDescriptor(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    if (fd >= 0) {\n"
        "        close(fd);\n"
        "    }\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
}

/*
 * @tc.name: SaFdTypeEmitterTest_002
 * @tc.desc: test SaFdTypeEmitter EmitCppWriteVar proxy
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaTypeEmitterTest, SaFdTypeEmitterTest_002, Level1)
{
    SaFdTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = true;
    std::string expectedCode =
        "if (!parcelWriteFileDescriptor(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
}

/*
 * @tc.name: SaFdSanTypeEmitterTest_001
 * @tc.desc: test SaFdSanTypeEmitter EmitCppWriteVar stub
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaTypeEmitterTest, SaFdSanTypeEmitterTest_001, Level1)
{
    SaFdSanTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = false;
    emitter.domainId_ = "testDomainId";
    std::string expectedCode =
        "if (!parcelWriteFileDescriptor(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    if (fd >= 0) {\n"
        "        fdsan_close_with_tag(fd, testDomainId);\n"
        "    }\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
}

/*
 * @tc.name: SaFdSanTypeEmitterTest_002
 * @tc.desc: test SaFdSanTypeEmitter EmitCppWriteVar
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaTypeEmitterTest, SaFdSanTypeEmitterTest_002, Level1)
{
    SaFdSanTypeEmitter emitter;
    emitter.logOn_ = true;
    emitter.isProxy = true;
    emitter.domainId_ = "testDomainId";
    std::string expectedCode =
        "if (!parcelWriteFileDescriptor(fd)) {\n"
        "    HiLog::Error(LABEL, \"Write [fd] failed!\");\n"
        "    return ERR_INVALID_DATA;\n"
        "}\n";
    StringBuilder sb;
    emitter.EmitCppWriteVar("parcel", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
}
} // namespace OHOS