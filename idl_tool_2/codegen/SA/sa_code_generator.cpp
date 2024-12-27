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
#include "cpp/sa_cpp_custom_types_code_emitter.h"
#include "ts/sa_ts_interface_code_emitter.h"
#include "ts/sa_ts_client_proxy_code_emitter.h"
#include "ts/sa_ts_service_stub_code_emitter.h"
#include "rust/sa_rust_interface_code_emitter.h"
#include "util/options.h"
#include "util/logger.h"
#include "parser/parser.h"

namespace OHOS {
namespace Idl {
SACodeGenerator::GeneratePolicies SACodeGenerator::policies_ = {
    {Language::CPP, SACodeGenerator::GenCppCode},
    {Language::RUST, SACodeGenerator::GenRustCode},
    {Language::TS, SACodeGenerator::GenTsCode}
};

CodeEmitMap SACodeGenerator::cppCodeEmitters_ = {
    {"types",     new SaCppCustomTypesCodeEmitter()},
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

// stores the path where the generated code will be saved
std::unordered_map<std::string, std::string> SACodeGenerator::genPath_;
// The path of the IDL file passed with the -c option
std::string SACodeGenerator::mainFilePath_;

void SACodeGenerator::GenCodeInit()
{
    genPath_ = {};
    mainFilePath_ = "";
}

bool SACodeGenerator::DoGenerate(const StrAstMap &allAst)
{
    GenCodeInit();

    auto genCodeFunc = GetCodeGenPoilcy();
    if (!genCodeFunc) {
        return false;
    }

    std::string outDir = Options::GetInstance().GetGenerationDirectory();

    for (const auto &astPair : allAst) {
        AutoPtr<AST> ast = astPair.second;
        if (ast->GetASTFileType() == ASTFileType::AST_IFACE) {
            mainFilePath_ = ast->GetIdlFile();
            GenCppPath(ast, outDir);
        }
    }

    for (const auto &astPair : allAst) {
        AutoPtr<AST> ast = astPair.second;
        if (ast->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
            GenCppPath(ast, outDir);
        }
    }

    for (const auto &astPair : allAst) {
        AutoPtr<AST> ast = astPair.second;
        if (ast->GetASTFileType() == ASTFileType::AST_IFACE ||
            ast->GetASTFileType() == ASTFileType::AST_TYPES ||
            ast->GetASTFileType() == ASTFileType::AST_ICALLBACK) {
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

void SACodeGenerator::GenCppPath(const AutoPtr<AST> &ast, const std::string &outDir)
{
    for (const auto &importPath : ast->GetImportNames()) {
        std::string idlFilePath = ast->GetIdlFile();
        size_t index = idlFilePath.rfind(SEPARATOR);
        std::string importFilePath = File::CanonicalPath(idlFilePath.substr(0, index + 1) + importPath + ".idl");
        std::string pathIdl = idlFilePath.substr(0, index);
        index = mainFilePath_.rfind(SEPARATOR);
        std::string pathMainFile = mainFilePath_.substr(0, index);
        std::string relaPath = File::RelativePath(pathIdl, pathMainFile);
        if (!relaPath.empty()) {
            relaPath = SEPARATOR + relaPath;
        }

        index = importPath.rfind(SEPARATOR);
        if (index == std::string::npos) {
            index = 0;
        }
        std::string relativePath = outDir + relaPath + SEPARATOR + importPath.substr(0, index);

#ifdef __MINGW32__
        std::replace(relativePath.begin(), relativePath.end(), '/', '\\');
#endif

        std::string absolutePath = File::AbsolutePath(relativePath);
        std::string resolvePath = File::CanonicalPath(absolutePath);
        if (!File::CreateParentDir(resolvePath)) {
            Logger::E(TAG, "Failed to create directory: %s.", resolvePath.c_str());
        }

        genPath_[importFilePath] = resolvePath;
    }
}

void SACodeGenerator::GenCppCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::IPC;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cppCodeEmitters_["types"]->OutPut(ast, genPath_[ast->GetIdlFile()], mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cppCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cppCodeEmitters_["interface"]->OutPut(ast, genPath_[ast->GetIdlFile()], mode);
            cppCodeEmitters_["proxy"]->OutPut(ast, genPath_[ast->GetIdlFile()], mode);
            cppCodeEmitters_["stub"]->OutPut(ast, genPath_[ast->GetIdlFile()], mode);
            break;
        }
        default:
            break;
    }
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