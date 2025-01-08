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

#include "ast/ast_expr.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
std::string ASTUnaryExpr::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);
    if (isParenExpr) {
        sb.Append("(");
    }

    sb.AppendFormat("%s%s", UnaryOpToString(op_).c_str(), expr_->Dump("").c_str());

    if (isParenExpr) {
        sb.Append(")");
    }

    return sb.ToString();
}

std::string ASTUnaryExpr::UnaryOpToString(UnaryOpKind op) const
{
    switch (op) {
        case UnaryOpKind::PLUS:
            return "+";
        case UnaryOpKind::MINUS:
            return "-";
        case UnaryOpKind::TILDE:
            return "~";
        default:
            return "unknown";
    }
}

std::string ASTBinaryExpr::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);
    if (isParenExpr) {
        sb.Append("(");
    }

    sb.AppendFormat("%s %s %s", lExpr_->Dump("").c_str(), BinaryOpToString(op_).c_str(), rExpr_->Dump("").c_str());

    if (isParenExpr) {
        sb.Append(")");
    }

    return sb.ToString();
}

std::string ASTBinaryExpr::BinaryOpToString(BinaryOpKind op) const
{
    switch (op) {
        case BinaryOpKind::MUL:
            return "*";
        case BinaryOpKind::DIV:
            return "/";
        case BinaryOpKind::MOD:
            return "%";
        case BinaryOpKind::ADD:
            return "+";
        case BinaryOpKind::SUB:
            return "-";
        case BinaryOpKind::LSHIFT:
            return "<<";
        case BinaryOpKind::RSHIFT:
            return ">>";
        case BinaryOpKind::AND:
            return "&";
        case BinaryOpKind::XOR:
            return "^";
        case BinaryOpKind::OR:
            return "|";
        default:
            return "unknown";
    }
}

std::string ASTNumExpr::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);
    if (isParenExpr) {
        sb.Append("(");
    }

    sb.AppendFormat("%s", value_.c_str());

    if (isParenExpr) {
        sb.Append(")");
    }

    return sb.ToString();
}

std::string ASTEnumExpr::Dump(const std::string &prefix)
{
    StringBuilder sb;
    sb.Append(prefix);
    if (isParenExpr) {
        sb.Append("(");
    }

    sb.AppendFormat("%s", value_.c_str());

    if (isParenExpr) {
        sb.Append(")");
    }

    return sb.ToString();
}
} // namespace Idl
} // namespace OHOS