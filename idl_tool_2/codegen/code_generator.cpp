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

#include "codegen/code_generator.h"
#include "util/options.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
bool CodeGenerator::DoGenerate(const StrAstMap &allAst)
{
    return false;
}

CodegenBuilder &CodegenBuilder::GetInstance()
{
    static CodegenBuilder genBuilder;
    return genBuilder;
}

bool CodegenBuilder::Generate(const StrAstMap &allAst)
{
    auto generator = generators.find(Options::GetInstance().GetInterfaceType());
    if (generator == generators.end()) {
        Logger::E(TAG, "the interface type is not supported, please check option");
        return false;
    }

    return generator->second->DoGenerate(allAst);
}

void CodegenBuilder::GeneratorRegister(InterfaceType type, AutoPtr<CodeGenerator> generator)
{
    generators[type] = generator;
}
} // namespace Idl
} // namespace OHOS