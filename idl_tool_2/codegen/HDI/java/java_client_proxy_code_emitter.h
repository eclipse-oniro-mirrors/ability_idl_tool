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

#ifndef OHOS_IDL_JAVA_CLIENT_PROXY_CODE_EMITTER_H
#define OHOS_IDL_JAVA_CLIENT_PROXY_CODE_EMITTER_H

#include "hdi_java_code_emitter.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {
class JavaClientProxyCodeEmitter : public HDIJavaCodeEmitter {
public:
    JavaClientProxyCodeEmitter() : HDIJavaCodeEmitter() {}

    ~JavaClientProxyCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitProxyFile();

    void EmitProxyImports(StringBuilder &sb) const;

    void EmitProxyCorelibImports(StringBuilder &sb) const;

    void EmitProxySelfDefinedTypeImports(StringBuilder &sb) const;

    void EmitProxyDBinderImports(StringBuilder &sb) const;

    void EmitProxyImpl(StringBuilder &sb);

    void EmitProxyConstants(StringBuilder &sb, const std::string &prefix);

    void EmitProxyConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitProxyMethodBody(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitReadOutArrayVariable(const std::string &parcelName, const std::string &name,
        const AutoPtr<ASTArrayType> &arrayType, StringBuilder &sb, const std::string &prefix);

    void EmitReadOutVariable(const std::string &parcelName, const std::string &name, const AutoPtr<ASTType> &type,
        StringBuilder &sb, const std::string &prefix);

    void EmitLocalVariable(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const;

    void EmitParamWriteVar(const AutoPtr<ASTParameter> &param,
        const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_JAVA_CLIENT_PROXY_CODE_EMITTER_H