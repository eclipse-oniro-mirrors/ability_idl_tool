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

#ifndef OHOS_IDL_AST_EXPRE_H
#define OHOS_IDL_AST_EXPRE_H

#include "ast/ast_node.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTExpr : public ASTNode {
public:
    inline std::string EmitCode()
    {
        return Dump("");
    }

    bool isParenExpr = false;
};

enum class UnaryOpKind {
    PLUS,  // +
    MINUS, // -
    TILDE, // ~
};

class ASTUnaryExpr : public ASTExpr {
public:
    std::string Dump(const std::string &prefix) override;
    std::string UnaryOpToString(UnaryOpKind op) const;

public:
    UnaryOpKind op_;
    AutoPtr<ASTExpr> expr_;
};

enum class BinaryOpKind {
    MUL,    // *
    DIV,    // /
    MOD,    // %
    ADD,    // +
    SUB,    // -
    LSHIFT, // <<
    RSHIFT, // >>
    AND,    // &
    XOR,    // ^
    OR,     // |
};

class ASTBinaryExpr : public ASTExpr {
public:
    std::string Dump(const std::string &prefix) override;
    std::string BinaryOpToString(BinaryOpKind op) const;

public:
    BinaryOpKind op_;
    AutoPtr<ASTExpr> lExpr_;
    AutoPtr<ASTExpr> rExpr_;
};

class ASTNumExpr : public ASTExpr {
public:
    std::string Dump(const std::string &prefix) override;
    std::string value_;
};

/**
 * @brief Defines the enumeration object structure in expression.
 *
 * This structure includes the enumeration information when using enum nesting identify in idl.
 *
 * @since 5.0
 */
class ASTEnumExpr : public ASTExpr {
public:
    std::string Dump(const std::string &prefix) override;
    std::string value_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_AST_EXPRE_H