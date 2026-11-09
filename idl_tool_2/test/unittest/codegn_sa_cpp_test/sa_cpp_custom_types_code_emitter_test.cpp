/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ast/ast_struct_type.h"
#include "ast/base/ast_boolean_type.h"
#include "ast/base/ast_integer_type.h"
#include "codegen/SA/cpp/sa_cpp_custom_types_code_emitter.h"
#include "util/common.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
/*
 * Tests for boolean normalization added by commit 1567c1a:
 * POD structs containing boolean members must be normalized before/after
 * block marshalling/unmarshalling (WriteUnpadBuffer/ReadUnpadBuffer).
 *
 * Branch coverage notes:
 * The defensive branch `structType != nullptr` (false path) in
 * EmitNormalizeBoolStatements is unreachable without undefined behavior:
 * members are stored via ASTStructType::AddMember which dereferences the
 * type pointer, and AutoPtr::operator->() does not perform null checks,
 * so a null TYPE_STRUCT member cannot be constructed through public APIs.
 * It is classified as defensive code and is not exercised here.
 */
class SaCppCustomTypesCodeEmitterTest : public testing::Test {
public:
    SaCppCustomTypesCodeEmitterTest() {}
    virtual ~SaCppCustomTypesCodeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaCppCustomTypesCodeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaCppCustomTypesCodeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaCppCustomTypesCodeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaCppCustomTypesCodeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeMarshallingImpl_001
 * @tc.desc: test EmitCustomTypeMarshallingImpl with pod struct containing boolean member
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeMarshallingImpl_001, Level1)
{
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_001 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("MyStruct");
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType->AddMember(booleanType, "enabled");

    StringBuilder sb;
    emitter->EmitCustomTypeMarshallingImpl(sb, structType);
    std::string expectedCode =
        "ErrCode MyStructBlockMarshalling(OHOS::MessageParcel& data, const MyStruct& dataBlock)\n"
        "{\n"
        "    MyStruct dataBlockCp = dataBlock;\n"
        "    {\n"
        "        unsigned char enabledNorm = *reinterpret_cast<const unsigned char *>(&dataBlockCp.enabled);\n"
        "        dataBlockCp.enabled = (enabledNorm == 1);\n"
        "    }\n"
        "    if (!data.WriteUnpadBuffer(&dataBlockCp, sizeof(MyStruct))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_001 end" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeMarshallingImpl_002
 * @tc.desc: test EmitCustomTypeMarshallingImpl with pod struct without boolean member
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeMarshallingImpl_002, Level1)
{
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_002 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("Point");
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    structType->AddMember(integerType, "x");

    StringBuilder sb;
    emitter->EmitCustomTypeMarshallingImpl(sb, structType);
    std::string expectedCode =
        "ErrCode PointBlockMarshalling(OHOS::MessageParcel& data, const Point& dataBlock)\n"
        "{\n"
        "    if (!data.WriteUnpadBuffer(&dataBlock, sizeof(Point))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_002 end" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeMarshallingImpl_003
 * @tc.desc: test EmitCustomTypeMarshallingImpl with pod struct containing nested boolean struct
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeMarshallingImpl_003, Level1)
{
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_003 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> innerStruct = new ASTStructType();
    innerStruct->SetName("Inner");
    AutoPtr<ASTType> enabledType = new ASTBooleanType();
    innerStruct->AddMember(enabledType, "enabled");

    AutoPtr<ASTStructType> outerStruct = new ASTStructType();
    outerStruct->SetName("Outer");
    AutoPtr<ASTType> innerType = static_cast<ASTType *>(innerStruct.Get());
    outerStruct->AddMember(innerType, "inner");
    AutoPtr<ASTType> flagType = new ASTBooleanType();
    outerStruct->AddMember(flagType, "flag");

    StringBuilder sb;
    emitter->EmitCustomTypeMarshallingImpl(sb, outerStruct);
    std::string expectedCode =
        "ErrCode OuterBlockMarshalling(OHOS::MessageParcel& data, const Outer& dataBlock)\n"
        "{\n"
        "    Outer dataBlockCp = dataBlock;\n"
        "    {\n"
        "        unsigned char enabledNorm = *reinterpret_cast<const unsigned char *>(&dataBlockCp.inner.enabled);\n"
        "        dataBlockCp.inner.enabled = (enabledNorm == 1);\n"
        "    }\n"
        "    {\n"
        "        unsigned char flagNorm = *reinterpret_cast<const unsigned char *>(&dataBlockCp.flag);\n"
        "        dataBlockCp.flag = (flagNorm == 1);\n"
        "    }\n"
        "    if (!data.WriteUnpadBuffer(&dataBlockCp, sizeof(Outer))) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeMarshallingImpl_003 end" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeUnmarshallingImpl_001
 * @tc.desc: test EmitCustomTypeUnmarshallingImpl with pod struct containing boolean member
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeUnmarshallingImpl_001, Level1)
{
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_001 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("MyStruct");
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType->AddMember(booleanType, "enabled");

    StringBuilder sb;
    emitter->EmitCustomTypeUnmarshallingImpl(sb, structType);
    std::string expectedCode =
        "ErrCode MyStructBlockUnmarshalling(OHOS::MessageParcel& data, MyStruct& dataBlock)\n"
        "{\n"
        "    const MyStruct *dataBlockPtr = reinterpret_cast<const MyStruct *>"
        "(data.ReadUnpadBuffer(sizeof(MyStruct)));\n"
        "    if (dataBlockPtr == nullptr) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "\n"
        "    if (memcpy_s(&dataBlock, sizeof(MyStruct), dataBlockPtr, sizeof(MyStruct)) != EOK) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "\n"
        "    {\n"
        "        unsigned char enabledNorm = *reinterpret_cast<const unsigned char *>(&dataBlock.enabled);\n"
        "        dataBlock.enabled = (enabledNorm == 1);\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_001 end" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeUnmarshallingImpl_002
 * @tc.desc: test EmitCustomTypeUnmarshallingImpl with pod struct without boolean member
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeUnmarshallingImpl_002, Level1)
{
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_002 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("Point");
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    structType->AddMember(integerType, "x");

    StringBuilder sb;
    emitter->EmitCustomTypeUnmarshallingImpl(sb, structType);
    std::string expectedCode =
        "ErrCode PointBlockUnmarshalling(OHOS::MessageParcel& data, Point& dataBlock)\n"
        "{\n"
        "    const Point *dataBlockPtr = reinterpret_cast<const Point *>(data.ReadUnpadBuffer(sizeof(Point)));\n"
        "    if (dataBlockPtr == nullptr) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "\n"
        "    if (memcpy_s(&dataBlock, sizeof(Point), dataBlockPtr, sizeof(Point)) != EOK) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_002 end" << std::endl;
}

/*
 * @tc.name: EmitCustomTypeUnmarshallingImpl_003
 * @tc.desc: test EmitCustomTypeUnmarshallingImpl with pod struct containing nested boolean struct
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitCustomTypeUnmarshallingImpl_003, Level1)
{
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_003 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> innerStruct = new ASTStructType();
    innerStruct->SetName("Inner");
    AutoPtr<ASTType> enabledType = new ASTBooleanType();
    innerStruct->AddMember(enabledType, "enabled");

    AutoPtr<ASTStructType> outerStruct = new ASTStructType();
    outerStruct->SetName("Outer");
    AutoPtr<ASTType> innerType = static_cast<ASTType *>(innerStruct.Get());
    outerStruct->AddMember(innerType, "inner");
    AutoPtr<ASTType> flagType = new ASTBooleanType();
    outerStruct->AddMember(flagType, "flag");

    StringBuilder sb;
    emitter->EmitCustomTypeUnmarshallingImpl(sb, outerStruct);
    std::string expectedCode =
        "ErrCode OuterBlockUnmarshalling(OHOS::MessageParcel& data, Outer& dataBlock)\n"
        "{\n"
        "    const Outer *dataBlockPtr = reinterpret_cast<const Outer *>(data.ReadUnpadBuffer(sizeof(Outer)));\n"
        "    if (dataBlockPtr == nullptr) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "\n"
        "    if (memcpy_s(&dataBlock, sizeof(Outer), dataBlockPtr, sizeof(Outer)) != EOK) {\n"
        "        return ERR_INVALID_DATA;\n"
        "    }\n"
        "\n"
        "    {\n"
        "        unsigned char enabledNorm = *reinterpret_cast<const unsigned char *>(&dataBlock.inner.enabled);\n"
        "        dataBlock.inner.enabled = (enabledNorm == 1);\n"
        "    }\n"
        "    {\n"
        "        unsigned char flagNorm = *reinterpret_cast<const unsigned char *>(&dataBlock.flag);\n"
        "        dataBlock.flag = (flagNorm == 1);\n"
        "    }\n"
        "    return ERR_NONE;\n"
        "}\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitCustomTypeUnmarshallingImpl_003 end" << std::endl;
}

/*
 * @tc.name: EmitNormalizeBoolStatements_001
 * @tc.desc: test EmitNormalizeBoolStatements with value object and mixed members
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitNormalizeBoolStatements_001, Level1)
{
    DTEST_LOG << "EmitNormalizeBoolStatements_001 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("MixStruct");
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType->AddMember(booleanType, "flag");
    AutoPtr<ASTType> integerType = new ASTIntegerType();
    structType->AddMember(integerType, "count");

    StringBuilder sb;
    emitter->EmitNormalizeBoolStatements(structType, "obj", false, sb, TAB);
    std::string expectedCode =
        "    {\n"
        "        unsigned char flagNorm = *reinterpret_cast<const unsigned char *>(&obj.flag);\n"
        "        obj.flag = (flagNorm == 1);\n"
        "    }\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitNormalizeBoolStatements_001 end" << std::endl;
}

/*
 * @tc.name: EmitNormalizeBoolStatements_002
 * @tc.desc: test EmitNormalizeBoolStatements with pointer object
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitNormalizeBoolStatements_002, Level1)
{
    DTEST_LOG << "EmitNormalizeBoolStatements_002 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("MyStruct");
    AutoPtr<ASTType> booleanType = new ASTBooleanType();
    structType->AddMember(booleanType, "flag");

    StringBuilder sb;
    emitter->EmitNormalizeBoolStatements(structType, "obj", true, sb, TAB);
    std::string expectedCode =
        "    {\n"
        "        unsigned char flagNorm = *reinterpret_cast<const unsigned char *>(&obj->flag);\n"
        "        obj->flag = (flagNorm == 1);\n"
        "    }\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitNormalizeBoolStatements_002 end" << std::endl;
}

/*
 * @tc.name: EmitNormalizeBoolStatements_003
 * @tc.desc: test EmitNormalizeBoolStatements recurses into nested struct needing normalize
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitNormalizeBoolStatements_003, Level1)
{
    DTEST_LOG << "EmitNormalizeBoolStatements_003 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> innerStruct = new ASTStructType();
    innerStruct->SetName("Inner");
    AutoPtr<ASTType> enabledType = new ASTBooleanType();
    innerStruct->AddMember(enabledType, "enabled");

    AutoPtr<ASTStructType> outerStruct = new ASTStructType();
    outerStruct->SetName("Outer");
    AutoPtr<ASTType> innerType = static_cast<ASTType *>(innerStruct.Get());
    outerStruct->AddMember(innerType, "inner");

    StringBuilder sb;
    emitter->EmitNormalizeBoolStatements(outerStruct, "obj", false, sb, TAB);
    std::string expectedCode =
        "    {\n"
        "        unsigned char enabledNorm = *reinterpret_cast<const unsigned char *>(&obj.inner.enabled);\n"
        "        obj.inner.enabled = (enabledNorm == 1);\n"
        "    }\n";
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitNormalizeBoolStatements_003 end" << std::endl;
}

/*
 * @tc.name: EmitNormalizeBoolStatements_004
 * @tc.desc: test EmitNormalizeBoolStatements skips nested struct not needing normalize
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitNormalizeBoolStatements_004, Level1)
{
    DTEST_LOG << "EmitNormalizeBoolStatements_004 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> innerStruct = new ASTStructType();
    innerStruct->SetName("Inner");
    AutoPtr<ASTType> innerIntType = new ASTIntegerType();
    innerStruct->AddMember(innerIntType, "x");

    AutoPtr<ASTStructType> outerStruct = new ASTStructType();
    outerStruct->SetName("Outer");
    AutoPtr<ASTType> innerType = static_cast<ASTType *>(innerStruct.Get());
    outerStruct->AddMember(innerType, "inner");
    AutoPtr<ASTType> countType = new ASTIntegerType();
    outerStruct->AddMember(countType, "count");

    StringBuilder sb;
    emitter->EmitNormalizeBoolStatements(outerStruct, "obj", false, sb, TAB);
    EXPECT_EQ(sb.ToString(), "");
    DTEST_LOG << "EmitNormalizeBoolStatements_004 end" << std::endl;
}

/*
 * @tc.name: EmitNormalizeBoolStatements_005
 * @tc.desc: test EmitNormalizeBoolStatements with struct without members
 * @tc.type: FUNC
 */
HWTEST_F(SaCppCustomTypesCodeEmitterTest, EmitNormalizeBoolStatements_005, Level1)
{
    DTEST_LOG << "EmitNormalizeBoolStatements_005 begin" << std::endl;
    AutoPtr<SaCppCustomTypesCodeEmitter> emitter = new SaCppCustomTypesCodeEmitter;
    AutoPtr<ASTStructType> structType = new ASTStructType();
    structType->SetName("EmptyStruct");

    StringBuilder sb;
    emitter->EmitNormalizeBoolStatements(structType, "obj", false, sb, TAB);
    EXPECT_EQ(sb.ToString(), "");
    DTEST_LOG << "EmitNormalizeBoolStatements_005 end" << std::endl;
}
} // namespace OHOS::Idl
