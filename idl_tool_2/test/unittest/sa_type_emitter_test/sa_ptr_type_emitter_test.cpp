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
#include "codegen/SA/type/sa_ptr_type_emitter.h"
#include "codegen/SA/type/sa_long_type_emitter.h"
#include "codegen/SA/type/sa_seq_type_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaPtrTypeEmitterTest : public testing::Test {
public:
    SaPtrTypeEmitterTest() {}
    virtual ~SaPtrTypeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaPtrTypeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaPtrTypeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaPtrTypeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaPtrTypeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetTypeKind_001
 * @tc.desc: test SaPtrTypeEmitter GetTypeKind
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaPtrTypeEmitterTest, GetTypeKind_001, Level1)
{
    DTEST_LOG << "GetTypeKind_001 begin" << std::endl;
    std::shared_ptr<SaPtrTypeEmitter> emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_SHAREDPTR);
    auto ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_SHAREDPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_NULL_SHAREDPTR);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_NULL_SHAREDPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_UNIQUEPTR);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_UNIQUEPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_NULL_UNIQUEPTR);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_NULL_UNIQUEPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_SPTR);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_SPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_NULL_SPTR);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_NULL_SPTR);
    emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_UNKNOWN);
    ret = emitter->GetTypeKind();
    EXPECT_EQ(ret, TypeKind::TYPE_UNKNOWN);
    DTEST_LOG << "GetTypeKind_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppType_001
 * @tc.desc: test SaPtrTypeEmitter EmitCppType
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaPtrTypeEmitterTest, EmitCppType_001, Level1)
{
    DTEST_LOG << "EmitCppType_001 begin" << std::endl;
    std::shared_ptr<SaPtrTypeEmitter> emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_SHAREDPTR);
    AutoPtr<SaTypeEmitter> longEmitter = new SaLongTypeEmitter();
    emitter->SetElementEmitter(longEmitter);
    std::string type = emitter->EmitCppType(TypeMode::NO_MODE);
    EXPECT_EQ(type, "std::shared_ptr<int64_t>");
    type = emitter->EmitCppType(TypeMode::LOCAL_VAR);
    EXPECT_EQ(type, "std::shared_ptr<int64_t>");
    type = emitter->EmitCppType(TypeMode::PARAM_IN);
    EXPECT_EQ(type, "const std::shared_ptr<int64_t>&");
    type = emitter->EmitCppType(TypeMode::PARAM_OUT);
    EXPECT_EQ(type, "std::shared_ptr<int64_t>&");
    type = emitter->EmitCppType(TypeMode::PARAM_INOUT);
    EXPECT_EQ(type, "std::shared_ptr<int64_t>&");
    type = emitter->EmitCppType(static_cast<TypeMode>(9));
    EXPECT_EQ(type, "unknown type");
    DTEST_LOG << "EmitCppType_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppWriteVar_001
 * @tc.desc: test SaPtrTypeEmitter EmitCppWriteVar with log
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaPtrTypeEmitterTest, EmitCppWriteVar_001, Level1)
{
    DTEST_LOG << "EmitCppWriteVar_001 begin" << std::endl;
    std::shared_ptr<SaPtrTypeEmitter> emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_NULL_SHAREDPTR);
    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    emitter->SetElementEmitter(seqEmitter);
    emitter->logOn_ = true;
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
    emitter->EmitCppWriteVar("parcel.", "fd", sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppWriteVar_001 end" << std::endl;
}

/*
 * @tc.name: EmitCppReadVar_001
 * @tc.desc: test SaPtrTypeEmitter EmitCppReadVar
 * @tc.type: FUNC
 * @tc.require: issueICL05Z
 */
HWTEST_F(SaPtrTypeEmitterTest, EmitCppReadVar_001, Level1)
{
    DTEST_LOG << "EmitCppReadVar_001 begin" << std::endl;
    std::shared_ptr<SaPtrTypeEmitter> emitter = std::make_shared<SaPtrTypeEmitter>(TypeKind::TYPE_NULL_SHAREDPTR);
    AutoPtr<SaTypeEmitter> seqEmitter = new SaSeqTypeEmitter();
    seqEmitter->SetTypeName("long");
    emitter->SetElementEmitter(seqEmitter);
    emitter->logOn_ = true;
    std::string expectedCode =
        "std::shared_ptr<long> ptr;\n"
        "if (parcel.ReadBool()) {\n"
        "    ptr = std::shared_ptr<long>(parcel.ReadParcelable<long>());\n"
        "    if (!ptr) {\n"
        "        HiLog::Error(LABEL, \"Read [long] failed!\");\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "}\n\n";
    StringBuilder sb;
    emitter->EmitCppReadVar("parcel.", "ptr", sb, "", true);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitCppReadVar_001 end" << std::endl;
}
} // namespace OHOS::Idl