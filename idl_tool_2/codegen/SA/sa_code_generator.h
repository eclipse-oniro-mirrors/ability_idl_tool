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

#ifndef OHOS_IDL_SA_CODEGENERATOR_H
#define OHOS_IDL_SA_CODEGENERATOR_H

#include <functional>
#include <map>
#include <string>

#include "codegen/code_generator.h"

namespace OHOS {
namespace Idl {
class SACodeGenerator : public CodeGenerator {
public:
    using GeneratePolicies = std::map<Language, CodeGenFunc>;
    bool DoGenerate(const StrAstMap &allAst) override;

private:
    static CodeGenFunc GetCodeGenPoilcy();

    static void GenCppPath(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenCppCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenRustCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenTsCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GeneratorInit() __attribute__((constructor));

    static void GenCodeInit();

    static GeneratePolicies policies_;
    static CodeEmitMap cppCodeEmitters_;
    static CodeEmitMap tsCodeEmitters_;
    static CodeEmitMap rustCodeEmitters_;
    static std::unordered_map<std::string, std::string> genPath_;
    static std::string mainFilePath_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CODEGENERATOR_H