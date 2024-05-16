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

#include "sa_code_generator.h"
#include "cpp/sa_cpp_interface_code_emitter.h"
#include "cpp/sa_cpp_client_proxy_code_emitter.h"
#include "cpp/sa_cpp_service_stub_code_emitter.h"
#include "ts/sa_ts_interface_code_emitter.h"
#include "ts/sa_ts_client_proxy_code_emitter.h"
#include "ts/sa_ts_service_stub_code_emitter.h"
#include "rust/sa_rust_interface_code_emitter.h"
#include "util/options.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
SACodeGenerator::GeneratePolicies SACodeGenerator::policies_ = {
    {Language::CPP, SACodeGenerator::GenCppCode},
    {Language::RUST, SACodeGenerator::GenRustCode},
    {Language::TS, SACodeGenerator::GenTsCode}
};

CodeEmitMap SACodeGenerator::cppCodeEmitters_ = {
    {"interface", new SaCppInterfaceCodeEmitter()},
    {"proxy", new SaCppClientProxyCodeEmitter()},
    {"stub", new SaCppServiceStubCodeEmitter()},
};

CodeEmitMap SACodeGenerator::tsCodeEmitters_ = {
    {"interface", new SaTsInterfaceCodeEmitter()},
    {"proxy",     new SaTsClientProxyCodeEmitter()},
    {"stub",     new SaTsServiceStubCodeEmitter()},
};

CodeEmitMap SACodeGenerator::rustCodeEmitters_ = {
    {"interface", new SaRustInterfaceCodeEmitter()},
};

bool SACodeGenerator::DoGenerate(const StrAstMap &allAst)
{
    auto genCodeFunc = GetCodeGenPoilcy();
    if (!genCodeFunc) {
        return false;
    }

    std::string outDir = Options::GetInstance().GetGenerationDirectory();
    for (const auto &astPair : allAst) {
        AutoPtr<AST> ast = astPair.second;
        if (ast->GetASTFileType() == ASTFileType::AST_IFACE) {
            genCodeFunc(ast, outDir);
        }
    }

    return true;
}

CodeGenFunc SACodeGenerator::GetCodeGenPoilcy()
{
    auto languageIter = policies_.find(Options::GetInstance().GetLanguage());
    if (languageIter == policies_.end()) {
        Logger::E(TAG, "the language is not supported, please check option");
        return CodeGenFunc{};
    }
    return languageIter->second;
}

void SACodeGenerator::GenCppCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    cppCodeEmitters_["interface"]->OutPut(ast, outDir, GenMode::IPC);
    cppCodeEmitters_["proxy"]->OutPut(ast, outDir, GenMode::IPC);
    cppCodeEmitters_["stub"]->OutPut(ast, outDir, GenMode::IPC);
    return;
}

void SACodeGenerator::GenRustCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    rustCodeEmitters_["interface"]->OutPut(ast, outDir, GenMode::IPC);
    return;
}

void SACodeGenerator::GenTsCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    tsCodeEmitters_["interface"]->OutPut(ast, outDir, GenMode::IPC);
    tsCodeEmitters_["proxy"]->OutPut(ast, outDir, GenMode::IPC);
    tsCodeEmitters_["stub"]->OutPut(ast, outDir, GenMode::IPC);
    return;
}

void SACodeGenerator::GeneratorInit()
{
    CodegenBuilder::GetInstance().GeneratorRegister(InterfaceType::SA, new SACodeGenerator());
}
} // namespace Idl
} // namespace OHOS