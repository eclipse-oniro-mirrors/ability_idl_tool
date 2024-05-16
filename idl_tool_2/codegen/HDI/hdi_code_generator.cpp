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

#include "hdi_code_generator.h"
#include "codegen/HDI/c/c_client_proxy_code_emitter.h"
#include "codegen/HDI/c/c_custom_types_code_emitter.h"
#include "codegen/HDI/c/c_interface_code_emitter.h"
#include "codegen/HDI/c/c_service_driver_code_emitter.h"
#include "codegen/HDI/c/c_service_impl_code_emitter.h"
#include "codegen/HDI/c/c_service_stub_code_emitter.h"
#include "codegen/HDI/cpp/cpp_client_proxy_code_emitter.h"
#include "codegen/HDI/cpp/cpp_custom_types_code_emitter.h"
#include "codegen/HDI/cpp/cpp_interface_code_emitter.h"
#include "codegen/HDI/cpp/cpp_service_driver_code_emitter.h"
#include "codegen/HDI/cpp/cpp_service_impl_code_emitter.h"
#include "codegen/HDI/cpp/cpp_service_stub_code_emitter.h"
#include "codegen/HDI/java/java_client_interface_code_emitter.h"
#include "codegen/HDI/java/java_client_proxy_code_emitter.h"
#include "util/options.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
HDICodeGenerator::GeneratePolicies HDICodeGenerator::policies_ = {
    {
        SystemLevel::MINI,
        {
            {
                GenMode::LOW,
                {
                    {Language::C, HDICodeGenerator::GenLowCCode}
                }
            }
        }
    },
    {
        SystemLevel::LITE,
        {
            {
                GenMode::KERNEL,
                {
                    {Language::C, HDICodeGenerator::GenKernelCode}
                }
            },
            {
                GenMode::PASSTHROUGH,
                {
                    {Language::C, HDICodeGenerator::GenPassthroughCCode},
                    {Language::CPP, HDICodeGenerator::GenPassthroughCppCode}
                }
            }
        }
    },
    {
        SystemLevel::FULL,
        {
            {
                GenMode::KERNEL,
                {
                    {Language::C, HDICodeGenerator::GenKernelCode}
                }
            },
            {
                GenMode::PASSTHROUGH,
                {
                    {Language::C, HDICodeGenerator::GenPassthroughCCode},
                    {Language::CPP, HDICodeGenerator::GenPassthroughCppCode}
                }
            },
            {
                GenMode::IPC,
                {
                    {Language::C, HDICodeGenerator::GenIpcCCode},
                    {Language::CPP, HDICodeGenerator::GenIpcCppCode},
                    {Language::JAVA, HDICodeGenerator::GenIpcJavaCode}
                }
            }
        }
    }
};

CodeEmitMap HDICodeGenerator::cCodeEmitters_ = {
    {"types",     new CCustomTypesCodeEmitter()  },
    {"interface", new CInterfaceCodeEmitter()    },
    {"proxy",     new CClientProxyCodeEmitter()  },
    {"driver",    new CServiceDriverCodeEmitter()},
    {"stub",      new CServiceStubCodeEmitter()  },
    {"service",   new CServiceImplCodeEmitter()  },
};

CodeEmitMap HDICodeGenerator::cppCodeEmitters_ = {
    {"types",     new CppCustomTypesCodeEmitter()  },
    {"interface", new CppInterfaceCodeEmitter()    },
    {"proxy",     new CppClientProxyCodeEmitter()  },
    {"driver",    new CppServiceDriverCodeEmitter()},
    {"stub",      new CppServiceStubCodeEmitter()  },
    {"service",   new CppServiceImplCodeEmitter()  },
};

CodeEmitMap HDICodeGenerator::javaCodeEmitters_ = {
    {"interface", new JavaClientInterfaceCodeEmitter()},
    {"proxy",     new JavaClientProxyCodeEmitter()    },
};

bool HDICodeGenerator::DoGenerate(const StrAstMap &allAst)
{
    auto genCodeFunc = GetCodeGenPoilcy();
    if (!genCodeFunc) {
        return false;
    }

    std::string outDir = Options::GetInstance().GetGenerationDirectory();
    std::set<std::string> sourceFile = Options::GetInstance().GetSourceFiles();
    Language language = Options::GetInstance().GetLanguage();
    if (language == Language::CPP) {
        for (const auto &ast : allAst) {
            if (sourceFile.find(ast.second->GetIdlFilePath()) != sourceFile.end()) {
                genCodeFunc(ast.second, outDir);
            }
        }
    } else if (language == Language::C) {
        for (const auto &ast : allAst) {
            genCodeFunc(ast.second, outDir);
        }
    }

    return true;
}

CodeGenFunc HDICodeGenerator::GetCodeGenPoilcy()
{
    auto systemPolicies = policies_.find(Options::GetInstance().GetSystemLevel());
    if (systemPolicies == policies_.end()) {
        Logger::E(TAG, "the system level is not supported, please check option");
        return CodeGenFunc{};
    }

    auto genModePolicies = systemPolicies->second;
    auto genModeIter = genModePolicies.find(Options::GetInstance().GetGenMode());
    if (genModeIter == genModePolicies.end()) {
        Logger::E(TAG, "the generate mode is not supported, please check option");
        return CodeGenFunc{};
    }

    auto languagePolicies = genModeIter->second;
    auto languageIter = languagePolicies.find(Options::GetInstance().GetLanguage());
    if (languageIter == languagePolicies.end()) {
        Logger::E(TAG, "the language is not supported, please check option");
        return CodeGenFunc{};
    }
    return languageIter->second;
}

void HDICodeGenerator::GenIpcCCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::IPC;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["driver"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenIpcCppCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::IPC;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cppCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cppCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["driver"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cppCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenIpcJavaCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::IPC;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            javaCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE:
        case ASTFileType::AST_ICALLBACK: {
            javaCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            javaCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenPassthroughCCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::PASSTHROUGH;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenPassthroughCppCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::PASSTHROUGH;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cppCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cppCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cppCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cppCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenKernelCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::KERNEL;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["proxy"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["driver"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["stub"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GenLowCCode(const AutoPtr<AST> &ast, const std::string &outDir)
{
    GenMode mode = GenMode::LOW;
    switch (ast->GetASTFileType()) {
        case ASTFileType::AST_TYPES: {
            cCodeEmitters_["types"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_IFACE: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["driver"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        case ASTFileType::AST_ICALLBACK: {
            cCodeEmitters_["interface"]->OutPut(ast, outDir, mode);
            cCodeEmitters_["service"]->OutPut(ast, outDir, mode);
            break;
        }
        default:
            break;
    }
}

void HDICodeGenerator::GeneratorInit()
{
    CodegenBuilder::GetInstance().GeneratorRegister(InterfaceType::HDI, new HDICodeGenerator());
}
} // namespace Idl
} // namespace OHOS