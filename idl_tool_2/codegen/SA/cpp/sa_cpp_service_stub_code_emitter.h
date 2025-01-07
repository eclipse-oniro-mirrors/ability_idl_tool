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

#ifndef OHOS_IDL_SA_CPP_SERVICE_STUB_CODE_EMITTER_H
#define OHOS_IDL_SA_CPP_SERVICE_STUB_CODE_EMITTER_H

#include "sa_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class SaCppServiceStubCodeEmitter : public SACppCodeEmitter {
public:
    SaCppServiceStubCodeEmitter() : SACppCodeEmitter() {}

    ~SaCppServiceStubCodeEmitter() override = default;

private:
    void EmitCode() override;

    // ISample.idl -> sample_service_stub.h
    void EmitStubHeaderFile();

    void EmitInterfaceStubInHeaderFile(StringBuilder &sb) const;

    static void EmitInterfaceStubMethodDecls(StringBuilder &sb, const std::string &prefix);

    void EmitStubSourceFile();

    void EmitInterfaceStubMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceStubMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;

    void EmitInterfaceStubMethodCall(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;

    void EmitLocalVariable(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const;

    static void EmitSaReturnParameter(const std::string &name, TypeKind kind, StringBuilder &sb);

    void EmitInterfaceProxyIpcCapacityValues(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceSetIpcCapacity(const AutoPtr <ASTMethod> &method, StringBuilder &sb, const std::string &prefix)
        const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_CPP_SERVICE_STUB_CODE_EMITTER_H