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

#ifndef OHOS_IDL_TOKEN_H
#define OHOS_IDL_TOKEN_H

#include <string>

namespace OHOS {
namespace Idl {
enum class TokenType {
    UNKNOWN = 0,
    // types
    VOID,    // "void"
    BOOLEAN, // "boolean"
    BYTE,    // "byte"
    SHORT,   // "short"
    INT,     // "int"
    LONG,    // "long"
    STRING,  // "std::string"
    STRING16, // "std::string16"
    FLOAT,   // "float"
    DOUBLE,  // "double"
    FD,      // "FileDescriptor"
    ASHMEM,  // "Ashmem"
    NATIVE_BUFFER, // "NativeBuffer"
    POINTER, // "Pointer"
    LIST,    // "List"
    MAP,     // "Map"
    SMQ,     // "SharedMemQueue"
    CHAR,    // "char"
    // qualifier
    UNSIGNED, // "unsigned"
    // custom types
    ENUM,   // "enum"
    STRUCT, // "struct"
    UNION,  // "union"
    // keywords
    PACKAGE,              // "package"
    INTERFACE_TOKEN,      // "interface_token"
    SUPPORT_DELEGATOR,    // "support_delegator"
    SEQ,                  // "sequenceable"
    IMPORT,               // "import"
    INTERFACE,            // "interface"
    EXTENDS,              // "extends"
    ONEWAY,               // "oneway"
    CALLBACK,             // "callback"
    FREEZECONTROL,        // "freezecontrol"
    FULL,                 // "full"
    LITE,                 // "lite"
    MINI,                 // "mini"
    CACHEABLE,            // "cacheable"
    IPCCODE,              // "ipccode"
    IN,                   // "in"
    OUT,                  // "out"
    INOUT,                // "inout"
    DOT,                  // "."
    COMMA,                // ","
    COLON,                // ":"
    ASSIGN,               // "="
    SEMICOLON,            // ";"
    BRACES_LEFT,          // "{"
    BRACES_RIGHT,         // "}"
    BRACKETS_LEFT,        // "["
    BRACKETS_RIGHT,       // "]"
    PARENTHESES_LEFT,     // "("
    PARENTHESES_RIGHT,    // ")"
    ANGLE_BRACKETS_LEFT,  // "<"
    ANGLE_BRACKETS_RIGHT, // ">"
    ADD,                  // "+"
    SUB,                  // "-"
    STAR,                 // "*"
    SLASH,                // "/"
    PERCENT_SIGN,         // "%""
    LEFT_SHIFT,           // "<<"
    RIGHT_SHIFT,          // ">>"
    AND,                  // "&"
    XOR,                  // "^"
    OR,                   // "|"
    TILDE,                // "~"
    PPLUS,                // "++"
    MMINUS,               // "--"

    // others
    ID,
    NUM,
    COMMENT_BLOCK,
    COMMENT_LINE,
    END_OF_FILE,
};

struct Location {
    std::string filePath;
    size_t row;
    size_t col;
};

struct Token {
    TokenType kind;
    Location location;
    std::string value;

    std::string Dump();
};

struct TokenTypeCompare {
    bool operator()(const Token &lhs, const Token &rhs) const
    {
        return lhs.kind > rhs.kind;
    }
};

std::string LocInfo(const Token &token);
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_TOKEN_H