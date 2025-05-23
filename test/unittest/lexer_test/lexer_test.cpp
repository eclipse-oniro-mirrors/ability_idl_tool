/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#define private public
#define protected public
#include "parser/lexer.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Idl {

class LexerUnitTest : public testing::Test {
public:
    LexerUnitTest() {}

    virtual ~LexerUnitTest() {}

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void LexerUnitTest::SetUpTestCase() {}

void LexerUnitTest::TearDownTestCase() {}

void LexerUnitTest::SetUp() {}

void LexerUnitTest::TearDown() {}

/*
 * @tc.name: OpenSourceFileTest_0100
 * @tc.desc: test Lexer's OpenSourceFile function return false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, OpenSourceFileTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, OpenSourceFileTest_0100, TestSize.Level1";
    Lexer imageLexer;
    String filePath = "";
    bool result = imageLexer.OpenSourceFile(filePath);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: TokenToCharTest_0100
 * @tc.desc: test the token in Lexer's TokenToChar function is ANGLE_BRACKETS_LEFT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0100, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::ANGLE_BRACKETS_LEFT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 60);
}

/*
 * @tc.name: TokenToCharTest_0200
 * @tc.desc: test the token in Lexer's TokenToChar function is ANGLE_BRACKETS_RIGHT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0200, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0200, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::ANGLE_BRACKETS_RIGHT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 62);
}

/*
 * @tc.name: TokenToCharTest_0300
 * @tc.desc: test the token in Lexer's TokenToChar function is BRACES_LEFT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0300, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0300, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::BRACES_LEFT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 123);
}

/*
 * @tc.name: TokenToCharTest_0400
 * @tc.desc: test the token in Lexer's TokenToChar function is BRACES_RIGHT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0400, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0400, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::BRACES_RIGHT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 125);
}

/*
 * @tc.name: TokenToCharTest_0500
 * @tc.desc: test the token in Lexer's TokenToChar function is BRACKETS_LEFT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0500, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0500, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::BRACKETS_LEFT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 91);
}

/*
 * @tc.name: TokenToCharTest_0600
 * @tc.desc: test the token in Lexer's TokenToChar function is BRACKETS_RIGHT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0600, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0600, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::BRACKETS_RIGHT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 93);
}

/*
 * @tc.name: TokenToCharTest_0700
 * @tc.desc: test the token in Lexer's TokenToChar function is COMMA.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0700, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0700, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::COMMA;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 44);
}

/*
 * @tc.name: TokenToCharTest_0800
 * @tc.desc: test the token in Lexer's TokenToChar function is DOT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0800, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0800, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::DOT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 46);
}

/*
 * @tc.name: TokenToCharTest_0900
 * @tc.desc: test the token in Lexer's TokenToChar function is PARENTHESES_LEFT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_0900, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_0900, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::PARENTHESES_LEFT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 40);
}

/*
 * @tc.name: TokenToCharTest_1000
 * @tc.desc: test the token in Lexer's TokenToChar function is PARENTHESES_RIGHT.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_1000, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_1000, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::PARENTHESES_RIGHT;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, 41);
}

/*
 * @tc.name: TokenToCharTest_1100
 * @tc.desc: test the token in Lexer's TokenToChar function is other.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, TokenToCharTest_1100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO)
        << "LexerUnitTest, TokenToCharTest_1100, TestSize.Level1";
    Lexer imageLexer;
    Token token = Token::IDENTIFIER;
    int result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, -1);
    token = Token::SEMICOLON;
    result = imageLexer.TokenToChar(token);
    EXPECT_EQ(result, ';');
}

/*
 * @tc.name: strToIntTest_0100
 * @tc.desc: test str to int
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, strToIntTest_0100, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "LexerUnitTest, strToIntTest_0100, TestSize.Level1";
    Lexer imageLexer;
    int number;
    {
        const char *str = "100";
        bool result = imageLexer.strToInt(str, 3, number);
        EXPECT_EQ(result, true);
        EXPECT_EQ(number, 100);
    }

    {
        const char *str = "0000100";
        bool result = imageLexer.strToInt(str, 7, number);
        EXPECT_EQ(result, true);
        EXPECT_EQ(number, 100);
    }

    {
        const char *str = "0";
        bool result = imageLexer.strToInt(str, 1, number);
        EXPECT_EQ(result, true);
        EXPECT_EQ(number, 0);
    }

    {
        const char *str = "-1";
        bool result = imageLexer.strToInt(str, 2, number);
        EXPECT_EQ(result, false);
    }

    // exceed the upper limit of int
    {
        const char *str = "2147483647";
        bool result = imageLexer.strToInt(str, 10, number);
        EXPECT_EQ(result, true);
        EXPECT_EQ(number, 2147483647);
    }

    // exceed the upper limit of int
    {
        const char *str = "2147483648";
        bool result = imageLexer.strToInt(str, 10, number);
        EXPECT_EQ(result, false);
    }

    {
        const char *str = "abdc";
        bool result = imageLexer.strToInt(str, 4, number);
        EXPECT_EQ(result, false);
    }

    {
        const char *str = "0000";
        bool result = imageLexer.strToInt(str, 4, number);
        EXPECT_EQ(result, true);
        EXPECT_EQ(number, 0);
    }
}

/*
 * @tc.name: DumpTokenSecondTest_1100
 * @tc.desc: test the token in Lexer's DumpTokenSecond function is other.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(LexerUnitTest, DumpTokenSecondTest_1100, Function | MediumTest | Level1)
{
    Lexer* lexer = new Lexer();
    lexer->currentToken_ = Token::DOUBLE;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("double"));
    lexer->currentToken_ = Token::END_OF_FILE;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("eof"));
    lexer->currentToken_ = Token::FLOAT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("float"));
    lexer->currentToken_ = Token::IDENTIFIER;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String());
    lexer->currentToken_ = Token::IN;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("in"));
    lexer->currentToken_ = Token::INOUT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("inout"));
    lexer->currentToken_ = Token::INTEGER;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("int"));
    lexer->currentToken_ = Token::LIST;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("List"));
    lexer->currentToken_ = Token::LONG;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("long"));
    lexer->currentToken_ = Token::MAP;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("Map"));
    lexer->currentToken_ = Token::ONEWAY;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("oneway"));
    lexer->currentToken_ = Token::OUT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("out"));
    lexer->currentToken_ = Token::SEQUENCEABLE;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("sequenceable"));
    lexer->currentToken_ = Token::PARENTHESES_LEFT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("("));
    lexer->currentToken_ = Token::PARENTHESES_RIGHT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String(")"));
    lexer->currentToken_ = Token::SEMICOLON;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String(";"));
    lexer->currentToken_ = Token::SHORT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("short"));
    lexer->currentToken_ = Token::STRING;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("String"));
    lexer->currentToken_ = Token::ANGLE_BRACKETS_RIGHT;
    EXPECT_STREQ(lexer->DumpTokenSecond(), String("unknown token"));

    delete lexer;
}
} // namespace idl
} // namespace OHOS