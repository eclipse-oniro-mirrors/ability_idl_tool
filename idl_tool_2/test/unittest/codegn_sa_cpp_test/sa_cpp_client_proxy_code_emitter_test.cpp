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

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class SaCppClientProxyCodeEmitterTest : public testing::Test {
public:
    SaCppClientProxyCodeEmitterTest() {}
    virtual ~SaCppClientProxyCodeEmitterTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaCppClientProxyCodeEmitterTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SaCppClientProxyCodeEmitterTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SaCppClientProxyCodeEmitterTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SaCppClientProxyCodeEmitterTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: EmitInterfaceProxyConstructor_001
 * @tc.desc: test EmitInterfaceProxyConstructor function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(SaCppClientProxyCodeEmitterTest, EmitInterfaceProxyConstructor_001, Level1)
{
    DTEST_LOG << "EmitInterfaceProxyConstructor_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix;
    std::string methodName0 = "methodName0";
    std::string methodName1 = "methodName1";
    AutoPtr<SaCppClientProxyCodeEmitter> emitter = new SaCppClientProxyCodeEmitter;
    ASTMethod* method0 = new ASTMethod();
    ASTType* methodType0 = new ASTVoidType();
    method0->SetName(methodName0);
    method0->SetReturnType(methodType0);
    AutoPtr<ASTAttr> attr0 = new ASTAttr(ASTAttr::ONEWAY);
    attr0->SetCacheableTimeString("0");
    method0->SetCacheable(attr0);
    
    ASTMethod* method1 = new ASTMethod();
    ASTType* methodType1 = new ASTVoidType();
    method1->SetName(methodName1);
    method1->SetReturnType(methodType1);
    AutoPtr<ASTAttr> attr1 = new ASTAttr(ASTAttr::CACHEABLE);
    attr1->cacheableTime_ = 1;
    method1->SetCacheable(attr1);
    emitter->logOn_ = false;
    emitter->ast_ = new AST();
    emitter->proxyName_ = "FooProxy";
    emitter->interfaceName_ = "IFoo";
    emitter->ast_->SetHasCacheableProxyMethods(true);
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->AddMethod(method0);
    emitter->interface_->AddMethod(method1);
    emitter->EmitInterfaceProxyConstructor(sb, prefix);
    std::string expectedCode =
        "explicit FooProxy(\n"
        "    const sptr<IRemoteObject>& remote)\n"
        "    : IRemoteProxy<IFoo>(remote)\n"
        "{\n"
        "ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(),\n"
        "    static_cast<uint32_t>(IpcCode::COMMAND_METHOD_NAME1), 0);\n"
        "\n"
        "    if (remote_) {\n"
        "        if (!remote->IsProxyObject()) {\n"
        "            return;\n"
        "        }\n"
        "        deathRecipient_ = new (std::nothrow) (*this);\n"
        "        if (!deathRecipient_) {\n"
        "            return;\n"
        "        }\n"
        "        if (!remote->AddDeathRecipient(deathRecipient_)) {\n"
        "            return;\n"
        "        }\n"
        "        remote_ = remote;\n"
        "    }\n"
        "}\n"
        "\n"
        "virtual ~FooProxy()\n"
        "{\n"
        "    if (remote_ == nullptr) {\n"
        "        return;\n"
        "    }\n"
        "    if (deathRecipient_ == nullptr) {\n"
        "        return;\n"
        "    }\n"
        "    remote_->RemoveDeathRecipient(deathRecipient_);\n"
        "    remote_ = nullptr;\n"
        "\n"
        "ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(),\n"
        "    static_cast<uint32_t>(IpcCode::COMMAND_METHOD_NAME1));\n"
        "}\n";

    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitInterfaceProxyConstructor_001 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceProxyConstructor_002
 * @tc.desc: test EmitInterfaceProxyConstructor function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(SaCppClientProxyCodeEmitterTest, EmitInterfaceProxyConstructor_002, Level1)
{
    DTEST_LOG << "EmitInterfaceProxyConstructor_002 begin" << std::endl;
    StringBuilder sb;
    std::string prefix;
    AutoPtr<SaCppClientProxyCodeEmitter> emitter = new SaCppClientProxyCodeEmitter;
    emitter->interface_ = new ASTInterfaceType();
    std::string methodName = "methodName";
    ASTMethod* method = new ASTMethod();
    method->SetName(methodName);
    method->attr_ = new ASTAttr(ASTAttr::CACHEABLE);
    method->attr_->cacheableTime_ = 23;
    emitter->interface_->AddMethod(method);
    emitter->logOn_ = true;
    emitter->proxyName_ = "UProxy";
    emitter->interfaceName_ = "Iuu";
    emitter->ast_ = new AST();
    emitter->ast_->SetHasCacheableProxyMethods(true);

    emitter->EmitInterfaceProxyConstructor(sb, prefix);
    std::string expectedCode =
        "explicit UProxy(\n"
        "    const sptr<IRemoteObject>& remote)\n"
        "    : IRemoteProxy<Iuu>(remote)\n"
        "{\n"
        "ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(),\n"
        "    static_cast<uint32_t>(IpcCode::COMMAND_METHOD_NAME), 23);\n"
        "\n"
        "    if (remote_) {\n"
        "        if (!remote->IsProxyObject()) {\n"
        "            HiLog::Error(LABEL, \"remote is not proxy object!\");\n"
        "            return;\n"
        "        }\n"
        "        deathRecipient_ = new (std::nothrow) (*this);\n"
        "        if (!deathRecipient_) {\n"
        "            HiLog::Error(LABEL, \"deathRecipient_ is nullptr!\");\n"
        "            return;\n"
        "        }\n"
        "        if (!remote->AddDeathRecipient(deathRecipient_)) {\n"
        "            HiLog::Error(LABEL, \"AddDeathRecipient failed!\");\n"
        "            return;\n"
        "        }\n"
        "        remote_ = remote;\n"
        "    }\n"
        "}\n"
        "\n"
        "virtual ~UProxy()\n"
        "{\n"
        "    if (remote_ == nullptr) {\n"
        "        return;\n"
        "    }\n"
        "    if (deathRecipient_ == nullptr) {\n"
        "        return;\n"
        "    }\n"
        "    remote_->RemoveDeathRecipient(deathRecipient_);\n"
        "    remote_ = nullptr;\n"
        "\n"
        "ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(),\n"
        "    static_cast<uint32_t>(IpcCode::COMMAND_METHOD_NAME));\n"
        "}\n";

    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitInterfaceProxyConstructor_002 end" << std::endl;
}

/*
 * @tc.name: EmitInterfaceProxyConstants_001
 * @tc.desc: test EmitInterfaceProxyConstants function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(SaCppClientProxyCodeEmitterTest, EmitInterfaceProxyConstants_001, Level1)
{
    DTEST_LOG << "EmitInterfaceProxyConstants_001 begin" << std::endl;
    StringBuilder sb;
    std::string prefix;
    std::string methodName = "methodName";
    AutoPtr<SaCppClientProxyCodeEmitter> emitter = new SaCppClientProxyCodeEmitter;
    ASTMethod* method = new ASTMethod();
    ASTType* methodType = new ASTVoidType();
    method->SetName(methodName);
    method->SetReturnType(methodType);
    AutoPtr<ASTAttr> attr = new ASTAttr(ASTAttr::IPC_IN_CAPACITY);
    attr->SetCacheableTimeString("0");
    method->SetCacheable(attr);
    emitter->ast_ = new AST();
    emitter->ast_->SetHasCacheableProxyMethods(true);
    emitter->interface_ = new ASTInterfaceType();
    emitter->interface_->AddMethod(method);
    emitter->deathRecipientName_ = "FooRecipient";
    emitter->proxyName_ = "FooProxy";
    emitter->EmitInterfaceProxyConstants(sb, prefix);
    std::string expectedCode =
        "class FooRecipient : public IRemoteObject::DeathRecipient {\n"
        "public:\n"
        "    explicit FooRecipient(FooProxy &client) : client_(client) {}\n"
        "    ~FooRecipient() override = default;\n"
        "    void OnRemoteDied(const wptr<IRemoteObject> &remote) override\n"
        "    {\n"
        "        client_.OnRemoteDied(remote);\n"
        "    }\n"
        "private:\n"
        "    FooProxy &client_;\n"
        "};\n"
        "\n"
        "void OnRemoteDied(const wptr<IRemoteObject> &remoteObject)\n"
        "{\n"
        "    (void)remoteObject;\n"
        "    ApiCacheManager::GetInstance().ClearCache(GetDescriptor());\n"
        "}\n"
        "sptr<IRemoteObject> remote_ = nullptr;\n"
        "sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;\n"
        "static inline BrokerDelegator<FooProxy> delegator_;\n";

    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
    DTEST_LOG << "EmitInterfaceProxyConstants_001 end" << std::endl;
}

/*
 * @tc.name: UnitPreprocess_test_001
 * @tc.desc: test UnitPreprocess function.
 * @tc.type: FUNC
  * @tc.require: #I8JQUO
 */
HWTEST_F(SaCppClientProxyCodeEmitterTest, GetFilePathNoPoint_001, Level1)
{
    DTEST_LOG << "GetFilePathNoPoint_001 begin" << std::endl;
    AutoPtr<SaCppInterfaceCodeEmitter> emitter = new SaCppInterfaceCodeEmitter;
    std::string filePath = "myTest/../fcallback/ICallback";
    auto ret = emitter->GetFilePath(filePath);
    EXPECT_EQ(ret, "myTest/.");
    ret = emitter->GetFilePathNoPoint(filePath);
    EXPECT_EQ(ret, "myTest/");
    filePath = "/fcallback/ICallback";
    ret = emitter->GetFilePath(filePath);
    ret = emitter->GetFilePathNoPoint(filePath);
    EXPECT_EQ(ret, "");
    filePath = "";
    ret = emitter->CppFullName(filePath);
    EXPECT_EQ(ret, "");
    filePath = "test.MySeq2";
    ret = emitter->CppFullName(filePath);
    EXPECT_EQ(ret, "test::MySeq2");
    DTEST_LOG << "GetFilePathNoPoint_001 end" << std::endl;
}
} // namespace OHOS::idl
