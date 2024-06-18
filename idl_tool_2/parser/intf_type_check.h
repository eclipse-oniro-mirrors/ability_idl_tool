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
#include "util/light_refcount_base.h"

namespace OHOS {
namespace Idl {

class IntfTypeChecker {
public:
    IntfTypeChecker(AutoPtr<AST> ast) : ast_(ast){};

    ~IntfTypeChecker() = default;

    bool CheckIntegrity();

private:
    bool CheckIntfSaAst();

    bool CheckIntfSaAstTypes();

    bool CheckIntfSaAstMethods();

    bool CheckIntfHdiAst();

    bool CheckIntfHdiAstFileType();

    bool CheckIntfHdiAstTypes();

    bool CheckIntfHdiAstParam(AutoPtr<ASTParameter> param, size_t methodIdx, size_t paramIdx);

    bool CheckInterfaceAst();

    bool CheckCallbackAst();

    bool CheckSmInterfaceAst();

    bool CheckIntfSmAstFileType();

    bool CheckIntfSmAstTypes();

    bool CheckIntfSmAst();

    AutoPtr<AST> ast_;
};
} // namespace Idl
} // namespace OHOS


#endif // OHOS_IDL_INTF_TYPE_CHECK_H