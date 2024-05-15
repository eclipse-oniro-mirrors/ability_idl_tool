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

#ifndef OHOS_IDL_JAVA_CODE_EMITTER_H
#define OHOS_IDL_JAVA_CODE_EMITTER_H

#include <cctype>

#include "codegen/HDI/hdi_code_emitter.h"

namespace OHOS {
namespace Idl {
class HDIJavaCodeEmitter : public HDICodeEmitter {
public:
    ~HDIJavaCodeEmitter() override = default;

protected:
    bool CreateDirectory();

    void EmitPackage(StringBuilder &sb);

    void EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix) override;

    std::string MethodName(const std::string &name) const;

    std::string SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const;

    void EmitInterfaceMethodParameter(const AutoPtr<ASTParameter> &param, StringBuilder &sb,
        const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_JAVA_CODE_EMITTER_H