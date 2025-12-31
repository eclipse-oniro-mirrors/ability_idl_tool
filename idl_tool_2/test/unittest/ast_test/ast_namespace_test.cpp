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

#include "ast/ast_interface_type.h"
#include "ast/ast_namespace.h"
#include "ast/ast_rawdata_type.h"
#include "ast/ast_sequenceable_type.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTNamespaceTest : public testing::Test {
public:
    ASTNamespaceTest() {}
    virtual ~ASTNamespaceTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTNamespaceTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTNamespaceTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTNamespaceTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTNamespaceTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: GetNamespace_001
 * @tc.desc: test ASTNamespace GetNamespace
 * @tc.type: FUNC
 */
HWTEST_F(ASTNamespaceTest, GetNamespace_001, Level1)
{
    DTEST_LOG << "GetNamespace_001 begin" << std::endl;
    ASTNamespace ns("com.example.package");
    AutoPtr<ASTNamespace> nspace = new ASTNamespace("com.example.test");
    ns.AddNamespace(nullptr);
    auto ret = ns.FindNamespace("");
    EXPECT_EQ(ret, nullptr);
    ret = ns.GetNamespace(0);
    EXPECT_EQ(ret, nullptr);
    ns.AddNamespace(nspace);
    ret = ns.GetNamespace(0);
    EXPECT_NE(ret, nullptr);
    DTEST_LOG << "GetNamespace_001 end" << std::endl;
}

/*
 * @tc.name: GetInterface_001
 * @tc.desc: test ASTNamespace GetInterface
 * @tc.type: FUNC
 */
HWTEST_F(ASTNamespaceTest, GetInterface_001, Level1)
{
    DTEST_LOG << "GetInterface_001 begin" << std::endl;
    ASTNamespace ns("com.example.package");
    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType();
    ns.AddInterface(nullptr);
    auto ret = ns.GetInterface(0);
    EXPECT_EQ(ret, nullptr);
    ns.AddInterface(interfaceType);
    ret = ns.GetInterface(0);
    EXPECT_NE(ret, nullptr);
    DTEST_LOG << "GetInterface_001 end" << std::endl;
}

/*
 * @tc.name: GetSequenceable_001
 * @tc.desc: test ASTNamespace GetSequenceable
 * @tc.type: FUNC
 */
HWTEST_F(ASTNamespaceTest, GetSequenceable_001, Level1)
{
    DTEST_LOG << "GetSequenceable_001 begin" << std::endl;
    ASTNamespace ns("com.example.package");
    AutoPtr<ASTSequenceableType> sequenceable = new ASTSequenceableType();
    ns.AddSequenceable(nullptr);
    auto ret = ns.GetSequenceable(0);
    EXPECT_EQ(ret, nullptr);
    ns.AddSequenceable(sequenceable);
    ret = ns.GetSequenceable(0);
    EXPECT_NE(ret, nullptr);
    DTEST_LOG << "GetSequenceable_001 end" << std::endl;
}

/*
 * @tc.name: GetRawData_001
 * @tc.desc: test ASTNamespace GetRawData
 * @tc.type: FUNC
 */
HWTEST_F(ASTNamespaceTest, GetRawData_001, Level1)
{
    DTEST_LOG << "GetRawData_001 begin" << std::endl;
    ASTNamespace ns("com.example.package");
    AutoPtr<ASTRawDataType> rawdata = new ASTRawDataType();
    ns.AddRawData(nullptr);
    auto ret = ns.GetRawData(0);
    EXPECT_EQ(ret, nullptr);
    ns.AddRawData(rawdata);
    ret = ns.GetRawData(0);
    EXPECT_NE(ret, nullptr);
    DTEST_LOG << "GetRawData_001 end" << std::endl;
}
} // namespace OHOS::Idl