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

#ifndef OHOS_IDL_SA_CPP_INTERFACE_CODE_EMITTER_H
#define OHOS_IDL_SA_CPP_INTERFACE_CODE_EMITTER_H

#include "sa_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class SaCppInterfaceCodeEmitter : public SACppCodeEmitter {
public:
    SaCppInterfaceCodeEmitter() : SACppCodeEmitter() {}

    ~SaCppInterfaceCodeEmitter() override = default;

private:
    void EmitCode() override;

    void EmitInterfaceHeaderFile();

    void EmitInterfaceInclusions(StringBuilder &sb) const;

    void EmitInterfaceDBinderInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitInterfaceSelfDefinedTypeInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    std::string GetFilePath(const std::string &fpnp) const;

    std::string GetFilePathNoPoint(const std::string &fpnp) const;

    std::string CppFullName(const std::string &name) const;

    bool EmitInterfaceUsings(StringBuilder &sb) const;

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceBody(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethod(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMemberVariables(StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_CPP_INTERFACE_CODE_EMITTER_H