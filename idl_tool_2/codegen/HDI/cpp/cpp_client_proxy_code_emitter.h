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

#ifndef OHOS_IDL_CPP_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_IDL_CPP_CLIENT_PROXY_CODE_EMITTER_H

#include "hdi_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class CppClientProxyCodeEmitter : public HDICppCodeEmitter {
public:
    CppClientProxyCodeEmitter() : HDICppCodeEmitter(),
        devmgrDeathRecipientName_("IServiceManagerDeathRecipient"),
        devmgrVersionName_("OHOS::HDI::ServiceManager::V1_0") {}

    ~CppClientProxyCodeEmitter() override = default;

protected:
    std::string devmgrDeathRecipientName_;
    std::string devmgrVersionName_;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitProxyHeaderFile();

    void EmitProxyHeaderInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void GetSourceOtherFileInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitProxyDecl(StringBuilder &sb, const std::string &prefix);

    void EmitProxyConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodDecls(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodDecl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyConstants(StringBuilder &sb, const std::string &prefix) const;

    void EmitPassthroughProxySourceFile();

    void EmitPassthroughProxySourceInclusions(StringBuilder &sb);

    void EmitPassthroughGetInstanceMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxySourceFile();

    void EmitProxySourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitGetMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitGetInstanceMethodImpl(StringBuilder &sb, const std::string &prefix);

    void EmitProxyPassthroughtLoadImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodImpls(StringBuilder &sb, const std::string &prefix);

    void EmitProxyMethodImpl(const AutoPtr<ASTInterfaceType> interface, const AutoPtr<ASTMethod> &method,
        StringBuilder &sb, const std::string &prefix);

    void EmitProxyMethodBody(const AutoPtr<ASTInterfaceType> interface, const AutoPtr<ASTMethod> &method,
        StringBuilder &sb, const std::string &prefix);

    void EmitWriteInterfaceToken(const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const;

    void EmitWriteFlagOfNeedSetMem(const AutoPtr<ASTMethod> &method, const std::string &dataBufName, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitUtilMethods(StringBuilder &sb, bool isDecl) override;

    void EmitProxyStaticMethodDecl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyStaticMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix);

    void EmitProxyStaticMethodBody(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix);

    void EmitProxyIsProxyMethodImpl(StringBuilder &sb, const std::string &prefix) const;
    void EmitProxyCastFromMethodImpls(StringBuilder &sb, const std::string &prefix) const;
    void EmitProxyCastFromMethodImpl(const AutoPtr<ASTInterfaceType> interface, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitPassthroughProxyCastFromMethodImplTemplate(StringBuilder &sb, const std::string &prefix) const;
    void EmitProxyCastFromMethodImplTemplate(StringBuilder &sb, const std::string &prefix) const;

    std::string GetNameSpaceByInterface(AutoPtr<ASTInterfaceType> interface, const std::string &name);

    void EmitProxyDevmgrDeathRecipient(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyPublicMembers(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyDevmgrDeathCallBack(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyReconnectMethodDecl(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyGetRemoteMethodDecl(StringBuilder &sb, const std::string &prefix) const;

    void EmitGetInstanceMethodInitProxyImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyCppReconnectMethodImpl(StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_CLIENT_PROXY_CODE_EMITTER_H