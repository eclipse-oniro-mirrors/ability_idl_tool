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

#ifndef OHOS_IDL_SA_TS_CODE_EMITTER_H
#define OHOS_IDL_SA_TS_CODE_EMITTER_H

#include "codegen/SA/sa_code_emitter.h"

namespace OHOS {
namespace Idl {
    constexpr const char *ERR_CODE = "errCode";
    constexpr const char *RETURN_VALUE = "returnValue";
class SATsCodeEmitter : public SACodeEmitter {
public:
    ~SATsCodeEmitter() override = default;

protected:
    void EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix) override;

    void EmitInterfaceImports(StringBuilder &sb) const;

    void EmitInterfaceMethodHead(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceSelfDefinedTypeImports(StringBuilder &sb) const;

    const std::string SuffixAdded(const std::string &originName) const;

    std::string FileName(const std::string &name) const;

    std::string MethodName(const std::string &name) const;

    bool CheckInterfaceType() const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_TS_CODE_EMITTER_H