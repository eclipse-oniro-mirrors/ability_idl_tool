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

#ifndef OHOS_IDL_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H
#define OHOS_IDL_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H

#include "hdi_c_code_emitter.h"

namespace OHOS {
namespace Idl {
class CInterfaceCodeEmitter : public HDICCodeEmitter {
public:
    CInterfaceCodeEmitter() : HDICCodeEmitter() {}

    ~CInterfaceCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitLowModeInterfaceHeaderFile();

    void EmitLowModeExternalMethod(StringBuilder &sb) const;

    void EmitInterfaceHeaderFile();

    void EmitImportInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitPreDeclaration(StringBuilder &sb) const;

    void EmitInterfaceDesc(StringBuilder &sb) const;

    void EmitInterfaceVersionMacro(StringBuilder &sb) const;

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceMethods(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethod(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitAsObjectMethod(StringBuilder &sb, const std::string &prefix) const;

    void EmitExternalMethod(StringBuilder &sb) const;

    void EmitInterfaceGetMethodDecl(StringBuilder &sb) const;

    void EmitInterfaceReleaseMethodDecl(StringBuilder &sb) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_C_CLIENT_INTERFACE_CODE_EMITTER_H