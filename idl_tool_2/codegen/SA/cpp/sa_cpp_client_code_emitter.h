/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDL_SA_CPP_CLIENT_CODE_EMITTER_H
#define OHOS_IDL_SA_CPP_CLIENT_CODE_EMITTER_H

#include "sa_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {

class SaCppClientCodeEmitter : public SACppCodeEmitter {
public:
    SaCppClientCodeEmitter() : SACppCodeEmitter() {}

    ~SaCppClientCodeEmitter() override = default;

private:
    void EmitCode() override;

    void EmitClientHeaderFile();

    void EmitInterfaceClientInHeaderFile(StringBuilder &sb) const;

    void EmitInterfaceClientConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitLoadCallback(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodDecls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodDecl(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientSetParam(StringBuilder &sb, const std::string &prefix) const;

    void EmitClientSourceFile();

    void EmitInterfaceClientMethodConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodAbility(StringBuilder &sb) const;

    void EmitInterfaceClientMethodAbilitySync(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodAbilityAsync(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodAbilityCheck(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodDeathCallback(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodImpl(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodBody(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodAbilitySF(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodStatusProxy(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodPrevent(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientMethodAbilityLoad(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceClientSamgr(StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_CPP_CLIENT_CODE_EMITTER_H