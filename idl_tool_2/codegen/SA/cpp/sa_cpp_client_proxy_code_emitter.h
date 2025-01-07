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

#ifndef OHOS_IDL_SA_CPP_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_IDL_SA_CPP_CLIENT_PROXY_CODE_EMITTER_H

#include "sa_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class SaCppClientProxyCodeEmitter : public SACppCodeEmitter {
public:
    SaCppClientProxyCodeEmitter() : SACppCodeEmitter() {}

    ~SaCppClientProxyCodeEmitter() override = default;

private:
    void EmitCode() override;

    void EmitInterfaceProxyHeaderFile();

    void EmitInterfaceProxyInHeaderFile(StringBuilder &sb) const;

    void EmitInterfaceProxyRegisterDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyAddCacheApi(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyUnRegisterDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodDecls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;

    void EmitInterfaceProxyDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyConstants(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyCppFile();

    void EmitInterfaceProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;

    void EmitInterfaceProxyMethodBody(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;

    void EmitInterfaceProxyMethodPreSendRequest(const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitInterfaceProxyMethodPostSendRequest(const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitInterfaceProxyMethodErrCode(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodReply(const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitInterfaceProxyMethodRetValue(const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitInterfaceProxyIpcCapacityValues(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceSetIpcCapacity(const AutoPtr <ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;
};
} // namespace SA
} // namespace OHOS

#endif // OHOS_IDL_SA_CPP_CLIENT_PROXY_CODE_EMITTER_H