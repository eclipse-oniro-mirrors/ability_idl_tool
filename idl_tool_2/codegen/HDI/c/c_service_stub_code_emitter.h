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

#ifndef OHOS_IDL_HDI_C_SERVICE_STUB_CODEE_MITTER_H
#define OHOS_IDL_HDI_C_SERVICE_STUB_CODEE_MITTER_H

#include "hdi_c_code_emitter.h"

namespace OHOS {
namespace Idl {
class CServiceStubCodeEmitter : public HDICCodeEmitter {
public:
    CServiceStubCodeEmitter() : HDICCodeEmitter() {}

    ~CServiceStubCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitServiceStubHeaderFile();

    void EmitStubHeaderInclusions(StringBuilder &sb);

    void EmitCbServiceStubDef(StringBuilder &sb) const;

    void EmitCbServiceStubMethodsDcl(StringBuilder &sb) const;

    void EmitServiceStubSourceFile();

    void EmitStubSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    // get or release method for driver interface object
    void EmitExternalMethodImpl(StringBuilder &sb);

    void EmitGetMethodImpl(StringBuilder &sb) const;

    void EmitGetInstanceMehtodImpl(StringBuilder &sb) const;

    void EmitReleaseMethodImpl(StringBuilder &sb) const;

    void EmitReleaseInstanceMethodImpl(StringBuilder &sb) const;

    void EmitServiceStubMethodImpls(StringBuilder &sb, const std::string &prefix);

    void EmitServiceStubMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitReadFlagVariable(bool readFlag, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubLocalVariable(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const;

    void EmitReadStubMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &gotoLabel,
        StringBuilder &sb, const std::string &prefix) const;

    void EmitReadCStringStubMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &gotoLabel,
        StringBuilder &sb, const std::string &prefix, AutoPtr<HdiTypeEmitter> &typeEmitter) const;

    void EmitOutVarMemInitialize(const AutoPtr<ASTParameter> &param, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitStubCallMethod(const AutoPtr<ASTMethod> &method,
        const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const;

    void EmitCallParameter(StringBuilder &sb, const AutoPtr<ASTType> &type, ASTParamAttr::ParamAttr attribute,
        const std::string &name) const;

    void EmitStubGetVerMethodImpl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubAsObjectMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitKernelStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix);

    void EmitKernelStubConstruct(StringBuilder &sb) const;

    void EmitStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix);

    void EmitStubRemoteDispatcher(StringBuilder &sb) const;

    void EmitStubNewInstance(StringBuilder &sb) const;

    void EmitStubReleaseMethod(StringBuilder &sb) const;

    void EmitStubConstructor(StringBuilder &sb) const;

    void EmitStubRegAndUnreg(StringBuilder &sb) const;

    void EmitUtilMethods(StringBuilder &sb, bool isDecl) override;

    void EmitParamLocalVar(const AutoPtr<ASTParameter> &param, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitErrorHandle(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_C_SERVICE_STUB_CODEE_MITTER_H