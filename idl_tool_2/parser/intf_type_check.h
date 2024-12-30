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

#ifndef OHOS_IDL_INTF_TYPE_CHECK_H
#define OHOS_IDL_INTF_TYPE_CHECK_H

#include "ast/ast.h"
#include "ast/ast_parameter.h"
#include "lexer/token.h"

namespace OHOS {
namespace Idl {

class IntfTypeChecker {
public:
    explicit IntfTypeChecker(AutoPtr<AST> ast) : ast_(std::move(ast)){};

    ~IntfTypeChecker() = default;

    bool CheckIntegrity();

    static bool CheckBasicType(const Token& token);

    static bool CheckUserDefType(const Token& token);

private:
    bool CheckIntfSaAst() const;

    bool CheckIntfSaAstTypes() const;

    bool CheckIntfSaAstMethods() const;

    bool CheckIntfHdiAst();

    bool CheckIntfHdiAstFileType() const;

    bool CheckIntfHdiAstTypes() const;

    bool CheckIntfHdiAstParam(const AutoPtr<ASTParameter>& param, size_t methodIdx, size_t paramIdx);

    bool CheckInterfaceAst() const;

    bool CheckCallbackAst() const;

    bool CheckSmInterfaceAst() const;

    bool CheckIntfSmAstFileType() const;

    bool CheckIntfSmAstTypes() const;

    bool CheckIntfSmAst() const;

    AutoPtr<AST> ast_;
};
} // namespace Idl
} // namespace OHOS


#endif // OHOS_IDL_INTF_TYPE_CHECK_H