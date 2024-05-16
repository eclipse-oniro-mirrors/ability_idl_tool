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

#ifndef OHOS_IDL_CPP_INTERFACE_CODE_EMITTER_H
#define OHOS_IDL_CPP_INTERFACE_CODE_EMITTER_H

#include "hdi_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class CppInterfaceCodeEmitter : public HDICppCodeEmitter {
public:
    CppInterfaceCodeEmitter() : HDICppCodeEmitter() {}

    ~CppInterfaceCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitInterfaceHeaderFile();

    void EmitInterfaceInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitInterfaceVersionMacro(StringBuilder &sb) const;

    void EmitInterfaceDefinition(StringBuilder &sb);

    void EmitInterfaceDescriptor(StringBuilder &sb, const std::string &prefix) const;

    void EmitGetMethodDecl(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceDestruction(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethodsDecl(StringBuilder &sb, const std::string &prefix);

    void EmitInterfaceMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceGetVersionMethod(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceIsProxyMethod(StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethodCommandsWithExtends(StringBuilder &sb, const std::string &prefix);

    void EmitGetDescMethod(StringBuilder &sb, const std::string &prefix) const;
    void EmitCastFromDecl(StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_INTERFACE_CODE_EMITTER_H