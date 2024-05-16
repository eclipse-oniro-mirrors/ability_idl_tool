/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDL_LEXER_H
#define OHOS_IDL_LEXER_H

#include <cstdlib>
#include <cctype>
#include <memory>
#include <unordered_map>

#include "lexer/token.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {
class Lexer {
public:
    enum class ParseMode {
        DECL_MODE,
        EXPR_MODE,
    };

    Lexer();

    ~Lexer() = default;

    bool Reset(const std::string &filePath);

    inline std::string GetFilePath() const
    {
        return (file_ != nullptr) ? file_->GetPath() : "";
    }

    Token PeekToken(bool skipComment = true);

    Token GetToken(bool skipComment = true);

    void SkipCurrentLine();

    bool SkipCurrentLine(char untilChar);

    void Skip(char untilChar);

    void SkipToken(TokenType tokenType);

    void SkipUntilToken(TokenType tokenType);

    void SkipEof();

    inline void SetParseMode(ParseMode mode)
    {
        mode_ = mode;
    }

private:
    void ReadToken(Token &token, bool skipComment = true);

    void InitCurToken(Token &token);

    void ReadId(Token &token);

    void ReadNum(Token &token);

    void ReadBinaryNum(Token &token);

    void ReadOctNum(Token &token);

    void ReadHexNum(Token &token);

    void ReadDecNum(Token &token);

    void ReadNumSuffix(Token &token);

    void ReadShiftLeftOp(Token &token);

    void ReadShiftRightOp(Token &token);

    void ReadPPlusOp(Token &token);

    void ReadMMinusOp(Token &token);

    void ReadComment(Token &token);

    void ReadLineComment(Token &token);

    void ReadBlockComment(Token &token);

    void ReadSymbolToken(Token &token);

private:
    std::string filePath_;
    std::unique_ptr<File> file_;

    ParseMode mode_;
    bool havePeek_;
    Token curToken_;

private:
    using StrTokenTypeMap = std::unordered_map<std::string, TokenType>;
    static StrTokenTypeMap keyWords_;
    static StrTokenTypeMap symbols_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_LEXER_H