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
#include "codegen/SA/cpp/sa_cpp_service_stub_code_emitter.h"
#include "codegen/SA/cpp/sa_cpp_interface_code_emitter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaCppServiceStubCodeEmitterTest : public testing::Test {
public:
    SaCppServiceStubCodeEmitterTest() {}
    virtual ~SaCppServiceStubCodeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaCppServiceStubCodeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaCppServiceStubCodeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaCppServiceStubCodeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaCppServiceStubCodeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubMethodDecls_001
 * @tc.desc: test EmitInterfaceStubMethodDecls function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubMethodDecls_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubMethodDecls_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;
    emitter->interface_ = new ASTInterfaceType();

    AutoPtr<ASTMethod> method0 = new ASTMethod();
    method0->SetName("StubMethod1");
    AutoPtr<ASTType> returnType0 = new ASTVoidType();
    method0->SetReturnType(returnType0);
    emitter->interface_->AddMethod(method0);

    AutoPtr<ASTMethod> method1 = new ASTMethod();
    method1->SetName("StubMethod2");
    AutoPtr<ASTType> returnType1 = new ASTIntegerType();
    method1->SetReturnType(returnType1);
    emitter->interface_->AddMethod(method1);

    emitter->EmitInterfaceStubMethodDecls(sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("int32_t OnRemoteRequest"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubMethodDecls_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubMethodImpls_001
 * @tc.desc: test EmitInterfaceStubMethodImpls function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubMethodImpls_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubMethodImpls_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;
    emitter->stubName_ = "TestStub";
    emitter->interfaceName_ = "ITest";
    emitter->interface_ = new ASTInterfaceType();

    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("TestMethod");
    AutoPtr<ASTType> returnType = new ASTVoidType();
    method->SetReturnType(returnType);
    emitter->interface_->AddMethod(method);

    emitter->EmitInterfaceStubMethodImpls(sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("TestStub::OnRemoteRequest"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubMethodImpls_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubUseHooks_001
 * @tc.desc: test EmitInterfaceStubUseHooks function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubUseHooks_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubUseHooks_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;
    emitter->ast_ = new AST();
    std::string optionStubHooksOn = "on";
    emitter->ast_->SetOptionStubHooks(optionStubHooksOn);
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->SetName("ITest");
    emitter->EmitInterfaceStubUseHooks(sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("CallbackEnter"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubUseHooks_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceParcelUseHooks_001
 * @tc.desc: test EmitInterfaceParcelUseHooks function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceParcelUseHooks_001, Level1)
{
    DTEST_LOG << "EmitInterfaceParcelUseHooks_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;
    emitter->ast_ = new AST();
    std::string optionParcelHooksOn = "on";
    emitter->ast_->SetOptionParcelHooks(optionParcelHooksOn);
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->SetName("ITest");
    emitter->EmitInterfaceParcelUseHooks(sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("Parcel"), std::string::npos);
    DTEST_LOG << "EmitInterfaceParcelUseHooks_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubMethodImpl_001
 * @tc.desc: test EmitInterfaceStubMethodImpl function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubMethodImpl_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubMethodImpl_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;
    emitter->stubName_ = "TestStub";
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->SetName("ITest");

    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("TestMethodImpl");
    AutoPtr<ASTType> returnType = new ASTVoidType();
    method->SetReturnType(returnType);
    emitter->interface_->AddMethod(method);

    emitter->EmitInterfaceStubMethodImpl(method, sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("ITestIpcCode::COMMAND"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubMethodImpl_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubMethodService_001
 * @tc.desc: test EmitInterfaceStubMethodService function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubMethodService_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubMethodService_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;

    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("ServiceMethod");
    AutoPtr<ASTType> returnType = new ASTVoidType();
    method->SetReturnType(returnType);

    AutoPtr<ASTParameter> param = new ASTParameter("inputParam", ASTParamAttr::PARAM_IN, new ASTIntegerType());
    method->AddParameter(param);

    emitter->EmitInterfaceStubMethodService(method, sb, prefix, false);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("ServiceMethod"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubMethodService_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubMethodCall_001
 * @tc.desc: test EmitInterfaceStubMethodCall function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubMethodCall_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubMethodCall_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;

    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("CallMethod");
    AutoPtr<ASTType> returnType = new ASTVoidType();
    method->SetReturnType(returnType);

    emitter->EmitInterfaceStubMethodCall(method, sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("CallMethod"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubMethodCall_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceStubFdClose_001
 * @tc.desc: test EmitInterfaceStubFdClose function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceStubFdClose_001, Level1)
{
    DTEST_LOG << "EmitInterfaceStubFdClose_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;

    emitter->EmitInterfaceStubFdClose(TypeKind::TYPE_FILEDESCRIPTOR, "testFd", sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("testFd"), std::string::npos);
    EXPECT_NE(result.find("close"), std::string::npos);
    DTEST_LOG << "EmitInterfaceStubFdClose_001 end" << std::endl;
}

/*
 * @tc.name: EmitLocalVariable_001
 * @tc.desc: test EmitLocalVariable function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitLocalVariable_001, Level1)
{
    DTEST_LOG << "EmitLocalVariable_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;

    AutoPtr<ASTParameter> param = new ASTParameter("localVar", ASTParamAttr::PARAM_IN, new ASTStringType());

    emitter->EmitLocalVariable(param, sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("localVar"), std::string::npos);
    DTEST_LOG << "EmitLocalVariable_001 end" << std::endl;
}

/*
 * @tc.name: EmitSaReturnParameter_001
 * @tc.desc: test EmitSaReturnParameter static function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitSaReturnParameter_001, Level1)
{
    DTEST_LOG << "EmitSaReturnParameter_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";

    SaCppServiceStubCodeEmitter::EmitSaReturnParameter("retVal", TypeKind::TYPE_INT, sb);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("retVal"), std::string::npos);
    DTEST_LOG << "EmitSaReturnParameter_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceSetIpcCapacity_001
 * @tc.desc: test EmitInterfaceSetIpcCapacity function.
 */
HWTEST_F(SaCppServiceStubCodeEmitterTest, EmitInterfaceSetIpcCapacity_001, Level1)
{
    DTEST_LOG << "EmitInterfaceSetIpcCapacity_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix = "    ";
    AutoPtr<SaCppServiceStubCodeEmitter> emitter = new SaCppServiceStubCodeEmitter;

    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("TestIpcMethod");

    emitter->EmitInterfaceSetIpcCapacity(method, sb, prefix);

    std::string result = sb.buffer_;
    EXPECT_NE(result.find("SetMaxCapacity"), std::string::npos);
    DTEST_LOG << "EmitInterfaceSetIpcCapacity_001 end" << std::endl;
}
} // namespace OHOS::Idl