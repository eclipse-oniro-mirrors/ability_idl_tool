/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

    void EmitLicense(StringBuilder& sb);

    void EmitMacros(StringBuilder& sb);

    void EmitHeaders(StringBuilder& sb);

    void EmitIPCHeaders(StringBuilder& sb);

    void EmitCommonHeaders(StringBuilder& sb);

    bool EmitCustomHeaders(StringBuilder& sb);

    String TrimDot(const String& fpnp);

    String GeneratePath(const String& fpnp);

    bool AppendRealPath(StringBuilder& sb, const String& fpnp);

    void AppendCommandEnums(StringBuilder& sb);

    String GetCodeFromMethod(const char* name);

    String GetNameFromParameter(const char* name);

    void EmitCommandEnums(StringBuilder& sb);

    void EmitCommands(StringBuilder& sb);

    void EmitRemoteObject(StringBuilder& sb);

    void EmitBrokers(StringBuilder& sb);

    void WrapLine(StringBuilder& sb, int index, const String& prefix);

    void AppendBrokerMethods(StringBuilder& sb);

    void AppendBrokerParameters(StringBuilder& sb, MetaParameter* mp);

    String ConvertType(MetaType* mt, bool pt = false);

    void EmitRemoteRequest(StringBuilder& sb);

    void ReadListFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void ReadMapFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void ReadFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void WriteListToParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void WriteMapToParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void WriteToParcel(StringBuilder& sb, MetaType* mt, const String& result,
        const String& name, const String& prefix);

    void AddRemoteRequestMethods(StringBuilder& sb);

    void AddRemoteRequestParameters(StringBuilder& sb, MetaMethod* mm);

    void EmitStub(StringBuilder& sb);

    void AppendStubMethods(StringBuilder& sb);

    void AppendStubParameters(StringBuilder& sb, MetaMethod* mm);

    void EmitProxy(StringBuilder& sb);

    void AppendProxyMethods(StringBuilder& sb);
};
}
}
#endif
