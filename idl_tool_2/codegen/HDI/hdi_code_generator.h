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

#ifndef OHOS_IDL_HDI_CODEGENERATOR_H
#define OHOS_IDL_HDI_CODEGENERATOR_H

#include "codegen/code_generator.h"

namespace OHOS {
namespace Idl {

class HDICodeGenerator : public CodeGenerator {
public:
    using GeneratePolicies = std::map<SystemLevel, std::map<GenMode, std::map<Language, CodeGenFunc>>>;
    bool DoGenerate(const StrAstMap &allAst) override;

private:
    static CodeGenFunc GetCodeGenPoilcy();

    static void GenIpcCCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenIpcCppCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenIpcJavaCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenPassthroughCCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenPassthroughCppCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenKernelCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GenLowCCode(const AutoPtr<AST> &ast, const std::string &outDir);

    static void GeneratorInit() __attribute__((constructor));

    static GeneratePolicies policies_;
    static CodeEmitMap cCodeEmitters_;
    static CodeEmitMap cppCodeEmitters_;
    static CodeEmitMap javaCodeEmitters_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CODEGENERATOR_H