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

#include "ast/ast.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTTest : public testing::Test {
public:
    ASTTest() {}
    virtual ~ASTTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: SetIdlFile_001
 * @tc.desc: test AST SetIdlFile
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, SetIdlFile_001, Level1)
{
    DTEST_LOG << "SetIdlFile_001 begin" << std::endl;
    AST ast;
    ast.SetIdlFile("path/to/file");
    EXPECT_FALSE(ast.IsValid());
    EXPECT_EQ(ast.GetIdlFile(), "path/to/file");
    EXPECT_EQ(ast.GetName(), "file");
    DTEST_LOG << "SetIdlFile_001 end" << std::endl;
}

/*
 * @tc.name: SetFullName_001
 * @tc.desc: test AST SetFullName
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, SetFullName_001, Level1)
{
    DTEST_LOG << "SetFullName_001 begin" << std::endl;
    AST ast;
    ast.SetFullName("example..module");
    EXPECT_EQ(ast.GetPackageName(), "");
    EXPECT_EQ(ast.GetName(), "module");
    DTEST_LOG << "SetFullName_001 end" << std::endl;
}

/*
 * @tc.name: ParseNamespace_001
 * @tc.desc: test AST ParseNamespace
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, ParseNamespace_001, Level1)
{
    DTEST_LOG << "ParseNamespace_001 begin" << std::endl;
    AST ast;
    AutoPtr<ASTNamespace> nspace = ast.ParseNamespace("package");
    EXPECT_NE(nspace, nullptr);
    EXPECT_EQ(nspace->ToShortString(), "");
    DTEST_LOG << "ParseNamespace_001 end" << std::endl;
}

/*
 * @tc.name: GetNamespace_001
 * @tc.desc: test AST GetNamespace
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, GetNamespace_001, Level1)
{
    DTEST_LOG << "GetNamespace_001 begin" << std::endl;
    AST ast;
    AutoPtr<ASTNamespace> nspace = new ASTNamespace("com.example.package");
    ast.AddNamespace(nullptr);
    ast.AddNamespace(nspace);
    EXPECT_NE(ast.GetNamespace(0), nullptr);
    EXPECT_EQ(ast.GetNamespace(1), nullptr);
    DTEST_LOG << "GetNamespace_001 end" << std::endl;
}

/*
 * @tc.name: IndexOfInterfaceDef_001
 * @tc.desc: test AST IndexOfInterfaceDef
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, IndexOfInterfaceDef_001, Level1)
{
    DTEST_LOG << "IndexOfInterfaceDef_001 begin" << std::endl;
    AST ast;
    ast.AddInterfaceDef(nullptr);
    AutoPtr<ASTInterfaceType> interface1 = new ASTInterfaceType();
    AutoPtr<ASTInterfaceType> interface2 = new ASTInterfaceType();
    ast.AddInterfaceDef(interface1);
    ast.SetIdlFile("path/to/file");
    EXPECT_TRUE(ast.IsValid());
    EXPECT_EQ(ast.IndexOf(interface1), 0);
    EXPECT_EQ(ast.IndexOf(interface2), -1);
    DTEST_LOG << "IndexOfInterfaceDef_001 end" << std::endl;
}

/*
 * @tc.name: IndexOfSequenceableDef_001
 * @tc.desc: test AST IndexOfSequenceableDef
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, IndexOfSequenceableDef_001, Level1)
{
    DTEST_LOG << "IndexOfSequenceableDef_001 begin" << std::endl;
    AST ast;
    ast.AddSequenceableDef(nullptr);
    AutoPtr<ASTSequenceableType> sequenceable1 = new ASTSequenceableType();
    AutoPtr<ASTSequenceableType> sequenceable2 = new ASTSequenceableType();
    ast.AddSequenceableDef(sequenceable1);
    EXPECT_EQ(ast.GetSequenceableDef(1), nullptr);
    EXPECT_EQ(ast.IndexOf(sequenceable1), 0);
    EXPECT_EQ(ast.IndexOf(sequenceable2), -1);
    DTEST_LOG << "IndexOfSequenceableDef_001 end" << std::endl;
}

/*
 * @tc.name: IndexOfRawDataDef_001
 * @tc.desc: test AST IndexOfRawDataDef
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, IndexOfRawDataDef_001, Level1)
{
    DTEST_LOG << "IndexOfRawDataDef_001 begin" << std::endl;
    AST ast;
    ast.AddRawDataDef(nullptr);
    AutoPtr<ASTRawDataType> rawdata1 = new ASTRawDataType();
    AutoPtr<ASTRawDataType> rawdata2 = new ASTRawDataType();
    ast.AddRawDataDef(rawdata1);
    EXPECT_EQ(ast.GetRawDataDef(1), nullptr);
    EXPECT_EQ(ast.IndexOf(rawdata1), 0);
    EXPECT_EQ(ast.IndexOf(rawdata2), -1);
    DTEST_LOG << "IndexOfRawDataDef_001 end" << std::endl;
}

/*
 * @tc.name: IndexOfASTTypeDef_001
 * @tc.desc: test AST IndexOfASTTypeDef
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, IndexOfASTTypeDef_001, Level1)
{
    DTEST_LOG << "IndexOfASTTypeDef_001 begin" << std::endl;
    AST ast;
    ast.AddType(nullptr);
    AutoPtr<ASTType> type1 = new ASTArrayType();
    AutoPtr<ASTType> type2 = new ASTEnumType();
    ast.AddType(type1);
    EXPECT_EQ(ast.FindType("", false), nullptr);
    EXPECT_EQ(ast.IndexOf(type1), 0);
    EXPECT_EQ(ast.IndexOf(type2), -1);
    DTEST_LOG << "IndexOfASTTypeDef_001 end" << std::endl;
}

/*
 * @tc.name: AddTypeDefinition_001
 * @tc.desc: test AST AddTypeDefinition
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, AddTypeDefinition_001, Level1)
{
    DTEST_LOG << "AddTypeDefinition_001 begin" << std::endl;
    AST ast;
    ast.AddTypeDefinition(nullptr);
    EXPECT_EQ(ast.GetTypeDefintion(0), nullptr);
    DTEST_LOG << "AddTypeDefinition_001 end" << std::endl;
}

/*
 * @tc.name: ASTDump_001
 * @tc.desc: test AST Dump
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, ASTDump_001, Level1)
{
    DTEST_LOG << "ASTDump_001 begin" << std::endl;
    AST ast;
    auto res = ast.Dump("prefix");
    EXPECT_EQ(res, "prefixModule[name:  file: ]\n");
    AutoPtr<ASTInterfaceType> interface1 = new ASTInterfaceType();
    AutoPtr<ASTInterfaceType> interface2 = new ASTInterfaceType();
    AutoPtr<ASTNamespace> nspace = new ASTNamespace("com.example.package");
    interface1->SetNamespace(nspace);
    interface2->SetNamespace(nspace);
    interface2->SetExternal(true);
    ast.AddInterfaceDef(interface1);
    ast.AddInterfaceDef(interface2);
    AutoPtr<ASTType> interface3 = new ASTInterfaceType();
    interface3->SetNamespace(nspace);
    ast.AddTypeDefinition(interface3);
    AutoPtr<AST> importAst = new AST();
    importAst->SetFullName("com.example.import");
    ast.AddImport(importAst);
    ast.SetPackageName("test.PackageName");
    res = ast.Dump("prefix");
    EXPECT_EQ(res, "prefixModule[name:  file: ]\npackage test.PackageName;"
        "\n\nsequenceable com.example.import;\n\ninterface com.example.package. {\n}"
        "\n\ninterface com.example.package.;\n\ninterface com.example.package. {\n}\n\n");
    DTEST_LOG << "ASTDump_001 end" << std::endl;
}

/*
 * @tc.name: AddImport_001
 * @tc.desc: test AST AddImport
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, AddImport_001, Level1)
{
    DTEST_LOG << "AddImport_001 begin" << std::endl;
    AST ast;
    AutoPtr<AST> importAst = new AST();
    importAst->SetFullName("com.example.import");
    ast.AddImport(importAst);
    ast.AddImport(importAst);
    EXPECT_FALSE(ast.GetImports().empty());
    DTEST_LOG << "AddImport_001 end" << std::endl;
}

/*
 * @tc.name: SetVersion_001
 * @tc.desc: test AST SetVersion
 * @tc.type: FUNC
 */
HWTEST_F(ASTTest, SetVersion_001, Level1)
{
    DTEST_LOG << "SetVersion_001 begin" << std::endl;
    AST ast;
    size_t majorVer = 1;
    size_t minorVer = 1;
    ast.SetVersion(majorVer, minorVer);
    EXPECT_NE(ast.GetMajorVer(), 0);
    EXPECT_NE(ast.GetMinorVer(), 0);
    DTEST_LOG << "SetVersion_001 end" << std::endl;
}
} // namespace OHOS::Idl