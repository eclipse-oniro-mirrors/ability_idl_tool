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

#ifndef OHOS_IDL_SA_TS_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_IDL_SA_TS_CLIENT_PROXY_CODE_EMITTER_H

#include "sa_ts_code_emitter.h"

namespace OHOS {
namespace Idl {
class SaTsClientProxyCodeEmitter : public SATsCodeEmitter {
public:
    SaTsClientProxyCodeEmitter() : SATsCodeEmitter() {}

    ~SaTsClientProxyCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitInterfaceProxyFile();

    void EmitInterfaceProxyImpl(StringBuilder &sb);

    void EmitInterfaceProxyConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceProxyMethodBody(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethodCallback(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix, bool haveOutPara) const;

    void EmitInterfaceMethodErrorCallback(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix,
        bool haveOutPara) const;

    void EmitInterfaceMethodCallbackInner(AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;
};
} // namespace SA
} // namespace OHOS

#endif // OHOS_IDL_SA_TS_CLIENT_PROXY_CODE_EMITTER_H