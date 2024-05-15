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

#ifndef OHOS_IDL_CPP_SERVICE_IMPL_CODE_EMITTER_H
#define OHOS_IDL_CPP_SERVICE_IMPL_CODE_EMITTER_H

#include "hdi_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class CppServiceImplCodeEmitter : public HDICppCodeEmitter {
public:
    CppServiceImplCodeEmitter() : HDICppCodeEmitter() {}

    ~CppServiceImplCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitImplHeaderFile();

    void EmitServiceImplInclusions(StringBuilder &sb);

    void EmitServiceImplDecl(StringBuilder &sb);

    void EmitServiceImplBody(StringBuilder &sb, const std::string &prefix);

    void EmitServiceImplConstructor(StringBuilder &sb, const std::string &prefix) const;

    void EmitServiceImplMethodDecls(StringBuilder &sb, const std::string &prefix) const;

    void EmitServiceImplMethodDecl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitImplSourceFile();

    void EmitImplSourceInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitServiceImplMethodImpls(StringBuilder &sb, const std::string &prefix) const;

    void EmitServiceImplMethodImpl(
        const AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitExternalGetMethodImpl(StringBuilder &sb);

    void EmitExternalReleaseMethodImpl(StringBuilder &sb);

    void EmitServiceImplGetMethodImpl(StringBuilder &sb, const std::string &prefix) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_SERVICE_IMPL_CODE_EMITTER_H