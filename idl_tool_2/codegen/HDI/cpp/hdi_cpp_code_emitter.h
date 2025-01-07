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

#ifndef OHOS_IDL_HDI_CPP_CODE_EMITTER_H
#define OHOS_IDL_HDI_CPP_CODE_EMITTER_H

#include "codegen/HDI/hdi_code_emitter.h"

namespace OHOS {
namespace Idl {
class HDICppCodeEmitter : public HDICodeEmitter {
public:
    ~HDICppCodeEmitter() override = default;

protected:

    void GetImportInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitInterfaceMethodParameter(
        const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const;

    bool IsVersion(const std::string &name) const;

    void GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    std::vector<std::string> EmitCppNameSpaceVec(const std::string &namespaceStr) const;

    std::string EmitPackageToNameSpace(const std::string &packageName) const;

    virtual void EmitBeginNamespace(StringBuilder &sb);

    virtual void EmitEndNamespace(StringBuilder &sb);

    virtual void EmitUsingNamespace(StringBuilder &sb);

    std::string EmitNamespace(const std::string &packageName) const;

    void EmitImportUsingNamespace(StringBuilder &sb);

    void EmitWriteMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName, StringBuilder &sb,
        const std::string &prefix) const;

    void EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, TypeMode mode, StringBuilder &sb,
        const std::string &prefix) const;

    std::string SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const;

    std::string EmitHeaderNameByInterface(AutoPtr<ASTInterfaceType> interface, const std::string &name);

    std::string EmitDefinitionByInterface(AutoPtr<ASTInterfaceType> interface, const std::string &name) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_CPP_CODE_EMITTER_H