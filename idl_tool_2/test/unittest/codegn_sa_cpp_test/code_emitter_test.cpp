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
#include "codegen/SA/cpp/sa_cpp_client_proxy_code_emitter.h"
#include "codegen/SA/cpp/sa_cpp_interface_code_emitter.h"
#include "codegen/SA/ts/sa_ts_service_stub_code_emitter.h"
#include "codegen/SA/rust/sa_rust_interface_code_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class CodeEmitterTest : public testing::Test {
public:
    CodeEmitterTest() {}
    virtual ~CodeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CodeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void CodeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void CodeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void CodeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: EmitLicense_001
 * @tc.desc: test EmitLicense function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitLicense_001, Level1)
{
    DTEST_LOG << "EmitLicense_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string prefix;
    emitter->logOn_ = false;
    emitter->ast_ = new AST();
    std::string license = "license";
    emitter->ast_->SetLicense(license);
    emitter->EmitLicense(sb);
    std::string expectedCode =
        "license"
        "\n\n";
    EXPECT_EQ(sb.ToString(), expectedCode);

    StringBuilder sb1;
    license = "";
    emitter->ast_->SetLicense(license);
    emitter->EmitLicense(sb1);
    expectedCode = "";
    EXPECT_EQ(sb1.ToString(), expectedCode);
    DTEST_LOG << "EmitLicense_001 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_001
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_001, Level1)
{
    DTEST_LOG << "NeedFlag_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    AutoPtr<ASTParameter> parameter = nullptr;
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_001 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_002
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_002, Level1)
{
    DTEST_LOG << "NeedFlag_002 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_OUT;
    AutoPtr<ASTType> type = new ASTStringType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "NeedFlag_002 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_003
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_003, Level1)
{
    DTEST_LOG << "NeedFlag_003 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_OUT;
    AutoPtr<ASTType> type = new ASTArrayType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "NeedFlag_003 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_004
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_004, Level1)
{
    DTEST_LOG << "NeedFlag_004 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_OUT;
    AutoPtr<ASTType> type = new ASTListType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, true);
    DTEST_LOG << "NeedFlag_004 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_005
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_005, Level1)
{
    DTEST_LOG << "NeedFlag_005 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_OUT;
    AutoPtr<ASTType> type = new ASTIntegerType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_005 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_006
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_006, Level1)
{
    DTEST_LOG << "NeedFlag_006 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_IN;
    AutoPtr<ASTType> type = new ASTStringType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_006 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_007
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_007, Level1)
{
    DTEST_LOG << "NeedFlag_007 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_IN;
    AutoPtr<ASTType> type = new ASTArrayType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_007 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_008
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_008, Level1)
{
    DTEST_LOG << "NeedFlag_008 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_IN;
    AutoPtr<ASTType> type = new ASTListType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_008 end" << std::endl;
}

/*
 * @tc.name: NeedFlag_009
 * @tc.desc: test NeedFlag function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, NeedFlag_009, Level1)
{
    DTEST_LOG << "NeedFlag_009 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    std::string name = "uu23";
    ASTParamAttr::ParamAttr attribute = ASTParamAttr::PARAM_IN;
    AutoPtr<ASTType> type = new ASTIntegerType();
    AutoPtr<ASTParameter> parameter = new ASTParameter(name, attribute, type);
    method->AddParameter(parameter);
    bool ret = emitter->NeedFlag(method);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedFlag_009 end" << std::endl;
}

/*
 * @tc.name: PackageToFilePath_001
 * @tc.desc: test PackageToFilePath function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, PackageToFilePath_001, Level1)
{
    DTEST_LOG << "PackageToFilePath_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    emitter->ast_ = new AST();
    emitter->ast_->SetHasCacheableProxyMethods(true);
    emitter->ast_->packageName_ = "AstPck1.AstPck2.AstPck3";
    std::string packageName = "AstPck1.AstPck2.AstPck3";
#ifndef __MINGW32__
    std::string expectedCode = "ast_pck1/ast_pck2/ast_pck3";
#else
    std::string expectedCode = "ast_pck1\\ast_pck2\\ast_pck3";
#endif
    std::string ret = emitter->PackageToFilePath(packageName);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "PackageToFilePath_001 end" << std::endl;
}

/*
 * @tc.name: InterfaceToFilePath_001
 * @tc.desc: test InterfaceToFilePath function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, InterfaceToFilePath_001, Level1)
{
    DTEST_LOG << "InterfaceToFilePath_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string interfaceName = "inter.Iuu[2]";
    std::string expectedCode = "inter/uu_proxy";
    std::string ret = emitter->InterfaceToFilePath(interfaceName);
    EXPECT_EQ(ret, expectedCode);

    interfaceName = "inter.Iuu";
    expectedCode = "inter/uu_proxy";
    ret = emitter->InterfaceToFilePath(interfaceName);
    EXPECT_EQ(ret, expectedCode);

    interfaceName = "interuu";
    expectedCode = "interuu_proxy";
    ret = emitter->InterfaceToFilePath(interfaceName);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "InterfaceToFilePath_001 end" << std::endl;
}

/*
 * @tc.name: EmitMethodCmdID_001
 * @tc.desc: test EmitMethodCmdID function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitMethodCmdID_001, Level1)
{
    DTEST_LOG << "EmitMethodCmdID_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    emitter->baseName_ = "EmitMethod";
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    method->isOverload_ = false;
    std::string expectedCode = "CMD_EMIT_METHOD_METHOD_NAME";
    std::string ret = emitter->EmitMethodCmdID(method);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitMethodCmdID_001 end" << std::endl;
}

/*
 * @tc.name: EmitMethodCmdID_002
 * @tc.desc: test EmitMethodCmdID function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitMethodCmdID_002, Level1)
{
    DTEST_LOG << "EmitMethodCmdID_002 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    emitter->baseName_ = "EmitMethod";
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    method->isOverload_ = true;
    method->cmdId_ = 23;

    std::string expectedCode = "CMD_EMIT_METHOD_METHOD_NAME_23";
    std::string ret = emitter->EmitMethodCmdID(method);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitMethodCmdID_002 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceMethodCommands_001
 * @tc.desc: test EmitInterfaceMethodCommands function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitInterfaceMethodCommands_001, Level1)
{
    DTEST_LOG << "EmitInterfaceMethodCommands_001 begin" << std::endl;
    AutoPtr<SaRustInterfaceCodeEmitter> emitter = new SaRustInterfaceCodeEmitter;
    StringBuilder sb;
    emitter->baseName_ = "EmitMethod";
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    method->isOverload_ = false;
    method->attr_ = new ASTAttr(ASTAttr::CACHEABLE);
    method->attr_->cacheableTime_ = 23;
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->AddVersionMethod(method);

    std::string methodName1 = "methodName1";
    ASTMethod* method1 = new ASTMethod();
    method1->SetName(methodName1);
    method1->isOverload_ = false;
    method1->attr_ = new ASTAttr(ASTAttr::FULL);
    emitter->interface_->AddMethod(method1);

    EXPECT_TRUE(Options::GetInstance().SetSystemLevel("full"));
    
    std::string expectedCode =
        "enum {\n"
        "    CMD_EMIT_METHOD_METHOD_NAME = 0,\n"
        "    CMD_EMIT_METHOD_METHOD_NAME1 = 1,\n"
        "};\n";
    emitter->EmitInterfaceMethodCommands(sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitInterfaceMethodCommands_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceMethodCommands_002
 * @tc.desc: test EmitInterfaceMethodCommands function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitInterfaceMethodCommands_002, Level1)
{
    DTEST_LOG << "EmitInterfaceMethodCommands_002 begin" << std::endl;
    AutoPtr<SaRustInterfaceCodeEmitter> emitter = new SaRustInterfaceCodeEmitter;
    StringBuilder sb;
    emitter->baseName_ = "EmitMethod";
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    method->isOverload_ = false;
    method->attr_ = new ASTAttr(ASTAttr::CACHEABLE);
    method->attr_->cacheableTime_ = 23;
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->AddVersionMethod(method);
    
    std::string expectedCode =
        "enum {\n"
        "    CMD_EMIT_METHOD_METHOD_NAME = 0,\n"
        "};\n";
    emitter->EmitInterfaceMethodCommands(sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitInterfaceMethodCommands_002 end" << std::endl;
}

/*
 * @tc.name: EmitVersionHeaderName_001
 * @tc.desc: test EmitVersionHeaderName function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitVersionHeaderName_001, Level1)
{
    DTEST_LOG << "EmitVersionHeaderName_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    Options::GetInstance().interfaceType = InterfaceType::SA;
    std::string expectedCode = "data";
    std::string ret = emitter->EmitVersionHeaderName("data");
    EXPECT_EQ(ret, expectedCode);

    Options::GetInstance().interfaceType = InterfaceType::SM;
    expectedCode = "v23_23/data";
    emitter->ast_ = new AST();
    emitter->ast_->majorVersion_ = 23;
    emitter->ast_->minorVersion_ = 23;
    ret = emitter->EmitVersionHeaderName("data");
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "EmitVersionHeaderName_001 end" << std::endl;
}

/*
 * @tc.name: EmitLogTagMacro_001
 * @tc.desc: test EmitLogTagMacro function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitLogTagMacro_001, Level1)
{
    DTEST_LOG << "EmitLogTagMacro_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string expectedCode = "#define HDF_LOG_TAG    data\n";
    emitter->EmitLogTagMacro(sb, "data");
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitLogTagMacro_001 end" << std::endl;
}

/*
 * @tc.name: ConstantName_001
 * @tc.desc: test ConstantName function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, ConstantName_001, Level1)
{
    DTEST_LOG << "ConstantName_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string name = "";
    std::string expectedCode = "";
    std::string ret = emitter->ConstantName(name);
    EXPECT_EQ(ret, expectedCode);

    name = "NameUU";
    expectedCode = "NAME_U_U";
    ret = emitter->ConstantName(name);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "ConstantName_001 end" << std::endl;
}

/*
 * @tc.name: PascalName_001
 * @tc.desc: test PascalName function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, PascalName_001, Level1)
{
    DTEST_LOG << "PascalName_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string name = "";
    std::string expectedCode = "";
    std::string ret = emitter->PascalName(name);
    EXPECT_EQ(ret, expectedCode);

    name = "name_name_UU";
    expectedCode = "NameNameUU";
    ret = emitter->PascalName(name);
    EXPECT_EQ(ret, expectedCode);

    name = "Name_name_UU";
    expectedCode = "NameNameUU";
    ret = emitter->PascalName(name);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "PascalName_001 end" << std::endl;
}

/*
 * @tc.name: FileName_001
 * @tc.desc: test FileName function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, FileName_001, Level1)
{
    DTEST_LOG << "FileName_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string name = "";
    std::string expectedCode = "";
    std::string ret = emitter->FileName(name);
    EXPECT_EQ(ret, expectedCode);

    name = "NameUU.U..U.UUUu";
    expectedCode = "name_u_u/u//u/uu_uu";
    ret = emitter->FileName(name);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "FileName_001 end" << std::endl;
}

/*
 * @tc.name: GetNamespace_001
 * @tc.desc: test GetNamespace function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, GetNamespace_001, Level1)
{
    DTEST_LOG << "GetNamespace_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string fpnp = "data";
    std::string expectedCode = "data";
    std::string ret = emitter->GetNamespace(fpnp);
    EXPECT_EQ(ret, expectedCode);

    fpnp = "data..UU";
    expectedCode = "UU";
    ret = emitter->GetNamespace(fpnp);
    EXPECT_EQ(ret, expectedCode);
    DTEST_LOG << "GetNamespace_001 end" << std::endl;
}

/*
 * @tc.name: EmitHeadMacro_001
 * @tc.desc: test EmitHeadMacro function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitHeadMacro_001, Level1)
{
    DTEST_LOG << "EmitHeadMacro_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string expectedCode =
        "#ifndef \n"
        "#define \n";
    emitter->EmitHeadMacro(sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);

    StringBuilder sb1;
    expectedCode =
        "#ifndef NAME_UU_H\n"
        "#define NAME_UU_H\n";
    emitter->EmitHeadMacro(sb1, "name.uu");
    EXPECT_EQ(sb1.ToString(), expectedCode);
    DTEST_LOG << "EmitHeadMacro_001 end" << std::endl;
}

/*
 * @tc.name: EmitTailMacro_001
 * @tc.desc: test EmitTailMacro function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitTailMacro_001, Level1)
{
    DTEST_LOG << "EmitTailMacro_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string expectedCode = "#endif // \n\n";
    emitter->EmitTailMacro(sb, "");
    EXPECT_EQ(sb.ToString(), expectedCode);

    StringBuilder sb1;
    expectedCode = "#endif // NAME_UU_H\n\n";
    emitter->EmitTailMacro(sb1, "name.uu");
    EXPECT_EQ(sb1.ToString(), expectedCode);
    DTEST_LOG << "EmitTailMacro_001 end" << std::endl;
}

/*
 * @tc.name: EmitHeadExternC_001
 * @tc.desc: test EmitHeadExternC function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitHeadExternC_001, Level1)
{
    DTEST_LOG << "EmitHeadExternC_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string expectedCode =
        "#ifdef __cplusplus\n"
        "extern \"C\" {\n"
        "#endif /* __cplusplus */\n";
    emitter->EmitHeadExternC(sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitHeadExternC_001 end" << std::endl;
}

/*
 * @tc.name: EmitTailExternC_001
 * @tc.desc: test EmitTailExternC function.
 * @tc.type: FUNC
 */
HWTEST_F(CodeEmitterTest, EmitTailExternC_001, Level1)
{
    DTEST_LOG << "EmitTailExternC_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    StringBuilder sb;
    std::string expectedCode =
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif /* __cplusplus */\n";
    emitter->EmitTailExternC(sb);
    EXPECT_EQ(sb.ToString(), expectedCode);
    DTEST_LOG << "EmitTailExternC_001 end" << std::endl;
}
} // namespace OHOS::Idl