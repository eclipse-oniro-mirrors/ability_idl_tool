/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDL_RUSTCODEEMITTER_H
#define OHOS_IDL_RUSTCODEEMITTER_H

#include <string>

#include "codegen/code_emitter.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
class RustCodeEmitter : public CodeEmitter {
public:
    RustCodeEmitter(MetaComponent* mc)
        : CodeEmitter(mc)
    {}

    void EmitInterface() override;

    void EmitInterfaceProxy() override;

    void EmitInterfaceStub() override;
private:
    void EmitInterface(StringBuilder& sb);

    String FileName(const String& name);

    void EmitLicense(StringBuilder& sb);

    void EmitHeaders(StringBuilder& sb);

    void EmitIPCHeaders(StringBuilder& sb);

    void EmitCommonHeaders(StringBuilder& sb);

    bool EmitCustomHeaders(StringBuilder& sb);

    String TrimDot(const String& fpnp);

    String GeneratePath(const String& fpnp);

    bool AppendRealPath(StringBuilder& sb, const String& fpnp);

    void AppendCommandEnums(StringBuilder& sb);

    String GetCodeFromMethod(const char * name);

    void EmitCommandEnums(StringBuilder& sb);

    void EmitCommandImpls(StringBuilder& sb);

    void EmitCommands(StringBuilder& sb);

    void EmitRemoteObject(StringBuilder& sb);

    void EmitBrokers(StringBuilder& sb);

    void AppendBrokerMethods(StringBuilder& sb);

    void AppendBrokerParameters(StringBuilder& sb, MetaParameter* mp);

    String ConvertType(MetaType* mt);

    void EmitRemoteRequest(StringBuilder& sb);

    void AddRemoteRequestMethods(StringBuilder& sb);

    void EmitStub(StringBuilder& sb);

    void AppendStubMethods(StringBuilder& sb);

    void EmitProxy(StringBuilder& sb);

    void AppendProxyMethods(StringBuilder& sb);
};
}
}
#endif