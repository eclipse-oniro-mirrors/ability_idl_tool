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

#ifndef OHOS_IDL_HDI_C_CODE_EMITTER_H
#define OHOS_IDL_HDI_C_CODE_EMITTER_H

#include "codegen/HDI/hdi_code_emitter.h"

namespace OHOS {
namespace Idl {
class HDICCodeEmitter : public HDICodeEmitter {
public:
    ~HDICCodeEmitter() override = default;

protected:
    void GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitInterfaceMethodParameter(
        const AutoPtr<ASTParameter> &parameter, StringBuilder &sb, const std::string &prefix) const;

    void EmitMethodNeedLoopVar(const AutoPtr<ASTMethod> &method,
        bool needRW, bool needFree, StringBuilder &sb, const std::string &prefix) const;

    bool EmitNeedLoopVar(const AutoPtr<ASTType> &type, bool needRW, bool needFree) const;

    void EmitHeadMacro(StringBuilder &sb, const std::string &fullName) const;

    void EmitTailMacro(StringBuilder &sb, const std::string &fullName) const;

    void EmitHeadExternC(StringBuilder &sb) const;

    void EmitTailExternC(StringBuilder &sb) const;

    std::string MacroName(const std::string &name) const;

    std::string SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const;

    std::string EmitDescMacroName() const;

    static constexpr const char *errorsLabel_   = "ERRORS";
    static constexpr const char *finishedLabel_ = "FINISHED";
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_C_CODE_EMITTER_H