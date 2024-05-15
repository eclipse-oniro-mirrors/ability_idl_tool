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

#ifndef OHOS_IDL_HDI_C_SERVICE_IMPL_CODE_EMITTER_H
#define OHOS_IDL_HDI_C_SERVICE_IMPL_CODE_EMITTER_H

#include "hdi_c_code_emitter.h"

namespace OHOS {
namespace Idl {
class CServiceImplCodeEmitter : public HDICCodeEmitter {
public:
    CServiceImplCodeEmitter() : HDICCodeEmitter() {}

    ~CServiceImplCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    // generate service header file of low mode
    void EmitLowServiceImplHeaderFile();

    void EmitLowServiceImplInclusions(StringBuilder &sb);

    void EmitLowServiceImplDefinition(StringBuilder &sb);

    // generate service source file of low mode
    void EmitLowServiceImplSourceFile();

    void EmitLowServiceImplGetMethod(StringBuilder &sb);

    // generate service header file of ipc, passthrough, kernel mode
    void EmitServiceImplHeaderFile();

    void EmitServiceImplHeaderInclusions(StringBuilder &sb);

    void EmitServiceImplExternalMethodsDecl(StringBuilder &sb) const;

    void EmitServiceImplSourceFile();

    void EmitServiceImplSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitKernelServiceImplDef(StringBuilder &sb) const;

    void EmitServiceImplDef(StringBuilder &sb) const;

    void EmitServiceImplMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitServiceImplMethodImpl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitKernelServiceImplGetMethod(StringBuilder &sb) const;

    void EmitServiceImplGetVersionMethod(StringBuilder &sb, const std::string &prefix) const;

    void EmitServiceImplGetMethod(StringBuilder &sb) const;

    void EmitKernelServiceImplReleaseMethod(StringBuilder &sb) const;

    void EmitServiceImplReleaseMethod(StringBuilder &sb) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_C_SERVICE_IMPL_CODE_EMITTER_H