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
    CSTRING,  // "const char *"
    STRING,  // "std::string"
    STRING16, // "std::string16"
    U16STRING, // "std::u16string"
    FLOAT,   // "float"
    DOUBLE,  // "double"
    FD,      // "FileDescriptor"
    FDSAN,      // "FileDescriptorSan"
    ASHMEM,  // "Ashmem"
    NATIVE_BUFFER, // "NativeBuffer"
    POINTER, // "Pointer"
    LIST,    // "List"
    SET,    // "Set"
    MAP,     // "Map"
    ORDEREDMAP,     // "OrderedMap"
    SMQ,     // "SharedMemQueue"
    CHAR,    // "char"
    UNSIGNED, // "unsigned"
    SHAREDPTR,    // "sharedptr"
    UNIQUEPTR,    // "uniqueptr"
    SPTR,         // "sptr"
    NULL_SHAREDPTR,    // "null_sharedptr"
    NULL_UNIQUEPTR,    // "null_uniqueptr"
    NULL_SPTR,         // "null_sptr"
    // qualifier
    // custom types
    ENUM,   // "enum"
    STRUCT, // "struct"
    UNION,  // "union"
    // keywords
    PACKAGE,              // "package"
    INTERFACE_TOKEN,      // "interface_token"
    SUPPORT_DELEGATOR,    // "support_delegator"
    OPTION_STUB_HOOKS,    // "option_stub_hooks"
    OPTION_PARCEL_HOOKS,    // "option_parcel_hooks"
    SEQ,                  // "sequenceable"
    RAWDATA,              // "rawdata"
    IMPORT,               // "import"
    INTERFACE,            // "interface"
    EXTENDS,              // "extends"
    ONEWAY,               // "oneway"
    CUSTOM_MSG_OPTION,    // "customMsgOption"
    CALLBACK,             // "callback"
    FREEZECONTROL,        // "freezecontrol"
    FULL,                 // "full"
    LITE,                 // "lite"
    MINI,                 // "mini"
    CACHEABLE,            // "cacheable"
    IPCCODE,              // "ipccode"
    IPC_IN_CAPACITY,      // "ipcincapacity"
    IPC_OUT_CAPACITY,     // "ipcoutcapacity"
    MACRODEF,             // "macrodef"
    MACRONDEF,            // "macrondef"
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