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

#ifndef OHOS_IDL_CPP_SERVICE_STUB_CODE_EMITTER_H
#define OHOS_IDL_CPP_SERVICE_STUB_CODE_EMITTER_H

#include "hdi_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class CppServiceStubCodeEmitter : public HDICppCodeEmitter {
public:
    CppServiceStubCodeEmitter() : HDICppCodeEmitter() {}

    ~CppServiceStubCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    // ISample.idl -> sample_service_stub.h
    void EmitStubHeaderFile();

    void EmitStubHeaderInclusions(StringBuilder &sb);

    void GetHeaderOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitStubUsingNamespace(StringBuilder &sb) const;

    void EmitStubDecl(StringBuilder &sb);

    void EmitStubBody(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubConstructorDecl(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubOnRequestDecl(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubMethodDecls(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubPrivateData(StringBuilder &sb, const std::string &prefix) const;

    // ISample.idl -> sample_service_stub.cpp
    void EmitStubSourceFile();

    void EmitStubSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void GetSourceOtherFileInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitInterfaceGetMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitGetMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitGetInstanceMethodImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubConstructorImpl(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubOnRequestMethodImpl(StringBuilder &sb, const std::string &prefix);

    void EmitStubMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitStubMethodImpl(AutoPtr<ASTInterfaceType> interface, const AutoPtr<ASTMethod> &method, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitStubStaticMethodImpl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubCallMethod(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubReadInterfaceToken(const std::string &parcelName, StringBuilder &sb, const std::string &prefix) const;

    void EmitStubReadMemFlag(const AutoPtr<ASTMethod> &method, const std::string &parcelName, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitLocalVariable(const AutoPtr<ASTParameter> &param, const std::string &parcelName, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitUtilMethods(StringBuilder &sb, bool isDecl) override;

    void EmitStubStaticMethodDecl(const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitCppLocalVar(const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_SERVICE_STUB_CODE_EMITTER_H