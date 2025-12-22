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

#include "ast/ast_expr.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl;

namespace OHOS::Idl {
class ASTExprTest : public testing::Test {
public:
    ASTExprTest() {}
    virtual ~ASTExprTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ASTExprTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ASTExprTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ASTExprTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ASTExprTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/*
 * @tc.name: ASTUnaryExpr_001
 * @tc.desc: test ASTUnaryExpr Dump
 * @tc.type: FUNC
 */
HWTEST_F(ASTExprTest, ASTUnaryExpr_001, Level1)
{
    DTEST_LOG << "ASTUnaryExpr_001 begin" << std::endl;
    ASTUnaryExpr expr;
    expr.op_ = UnaryOpKind::PLUS;
    expr.expr_ = new ASTNumExpr();
    auto ret = expr.Dump("");
    EXPECT_EQ(ret, "+");
    expr.isParenExpr = true;
    ret = expr.Dump("");
    EXPECT_EQ(ret, "(+)");
    DTEST_LOG << "ASTUnaryExpr_001 end" << std::endl;
}

/*
 * @tc.name: ASTUnaryExpr_002
 * @tc.desc: test ASTUnaryExpr UnaryOpToString
 * @tc.type: FUNC
 */
HWTEST_F(ASTExprTest, ASTUnaryExpr_002, Level1)
{
    DTEST_LOG << "ASTUnaryExpr_002 begin" << std::endl;
    ASTUnaryExpr expr;
    auto ret = expr.UnaryOpToString(UnaryOpKind::PLUS);
    EXPECT_EQ(ret, "+");
    ret = expr.UnaryOpToString(UnaryOpKind::MINUS);
    EXPECT_EQ(ret, "-");
    ret = expr.UnaryOpToString(UnaryOpKind::TILDE);
    EXPECT_EQ(ret, "~");
    ret = expr.UnaryOpToString(static_cast<UnaryOpKind>(-1));
    EXPECT_EQ(ret, "unknown");
    DTEST_LOG << "ASTUnaryExpr_002 end" << std::endl;
}

/*
 * @tc.name: ASTBinaryExpr_001
 * @tc.desc: test ASTBinaryExpr Dump
 * @tc.type: FUNC
 */
HWTEST_F(ASTExprTest, ASTBinaryExpr_001, Level1)
{
    DTEST_LOG << "ASTBinaryExpr_001 begin" << std::endl;
    ASTBinaryExpr expr;
    expr.op_ = BinaryOpKind::MUL;
    expr.lExpr_ = new ASTNumExpr();
    expr.rExpr_ = new ASTEnumExpr();
    expr.lExpr_->isParenExpr = true;
    expr.rExpr_->isParenExpr = true;
    auto ret = expr.Dump("");
    EXPECT_EQ(ret, "() * ()");
    DTEST_LOG << "ASTBinaryExpr_001 end" << std::endl;
}

/*
 * @tc.name: ASTBinaryExpr_002
 * @tc.desc: test ASTBinaryExpr BinaryOpToString
 * @tc.type: FUNC
 */
HWTEST_F(ASTExprTest, ASTBinaryExpr_002, Level1)
{
    DTEST_LOG << "ASTBinaryExpr_002 begin" << std::endl;
    ASTBinaryExpr expr;
    auto ret = expr.BinaryOpToString(BinaryOpKind::MUL);
    EXPECT_EQ(ret, "*");
    ret = expr.BinaryOpToString(BinaryOpKind::DIV);
    EXPECT_EQ(ret, "/");
    ret = expr.BinaryOpToString(BinaryOpKind::MOD);
    EXPECT_EQ(ret, "%");
    ret = expr.BinaryOpToString(BinaryOpKind::ADD);
    EXPECT_EQ(ret, "+");
    ret = expr.BinaryOpToString(BinaryOpKind::SUB);
    EXPECT_EQ(ret, "-");
    ret = expr.BinaryOpToString(BinaryOpKind::LSHIFT);
    EXPECT_EQ(ret, "<<");
    ret = expr.BinaryOpToString(BinaryOpKind::RSHIFT);
    EXPECT_EQ(ret, ">>");
    ret = expr.BinaryOpToString(BinaryOpKind::AND);
    EXPECT_EQ(ret, "&");
    ret = expr.BinaryOpToString(BinaryOpKind::XOR);
    EXPECT_EQ(ret, "^");
    ret = expr.BinaryOpToString(BinaryOpKind::OR);
    EXPECT_EQ(ret, "|");
    ret = expr.BinaryOpToString(static_cast<BinaryOpKind>(-1));
    EXPECT_EQ(ret, "unknown");
    DTEST_LOG << "ASTBinaryExpr_002 end" << std::endl;
}
} // namespace OHOS::idl