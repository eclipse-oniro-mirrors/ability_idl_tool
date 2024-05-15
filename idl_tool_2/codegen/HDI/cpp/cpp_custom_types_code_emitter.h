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

#ifndef OHOS_IDL_CPP_CUSTOM_TYPES_CODE_EMITTER_H
#define OHOS_IDL_CPP_CUSTOM_TYPES_CODE_EMITTER_H

#include "hdi_cpp_code_emitter.h"

namespace OHOS {
namespace Idl {
class CppCustomTypesCodeEmitter : public HDICppCodeEmitter {
public:
    CppCustomTypesCodeEmitter() : HDICppCodeEmitter() {}

    ~CppCustomTypesCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitPassthroughCustomTypesHeaderFile();

    void EmitPassthroughHeaderFileInclusions(StringBuilder& sb);

    void EmitCustomTypesHeaderFile();

    void EmitHeaderFileInclusions(StringBuilder &sb);

    void EmitForwardDeclaration(StringBuilder &sb) const;

    void EmitUsingNamespace(StringBuilder &sb) override;

    void EmitCustomTypeDecls(StringBuilder &sb) const;

    void EmitCustomTypeFuncDecl(StringBuilder &sb) const;

    void EmitCustomTypeMarshallFuncDecl(StringBuilder &sb, const AutoPtr<ASTType> &type) const;

    void EmitCustomTypesSourceFile();

    void EmitSourceFileInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitCustomTypeDataProcess(StringBuilder &sb) const;

    void EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type) const;

    void EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type) const;

    void EmitBeginNamespace(StringBuilder &sb) override;

    void EmitEndNamespace(StringBuilder &sb) override;

    void EmitUtilMethods(StringBuilder &sb, bool isDecl) override;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_CUSTOM_TYPES_CODE_EMITTER_H