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

#ifndef OHOS_IDL_HDI_C_CUSTOM_TYPES_CODE_EMITTER_H
#define OHOS_IDL_HDI_C_CUSTOM_TYPES_CODE_EMITTER_H

#include <vector>

#include "hdi_c_code_emitter.h"

namespace OHOS {
namespace Idl {
class CCustomTypesCodeEmitter : public HDICCodeEmitter {
public:
    CCustomTypesCodeEmitter() : HDICCodeEmitter() {}

    ~CCustomTypesCodeEmitter() override = default;

private:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    void EmitCode() override;

    void EmitPassthroughCustomTypesHeaderFile();

    void EmitPassthroughHeaderInclusions(StringBuilder &sb);

    void EmitCustomTypesHeaderFile();

    void EmitHeaderInclusions(StringBuilder &sb);

    void EmitForwardDeclaration(StringBuilder &sb) const;

    void EmitCustomTypeDecls(StringBuilder &sb) const;

    void EmitCustomTypeFuncDecl(StringBuilder &sb) const;

    void EmitCustomTypeMarshallFuncDecl(StringBuilder &sb, const AutoPtr<ASTType> &type) const;

    void EmitCustomTypesSourceFile();

    void EmitSoucreInclusions(StringBuilder &sb);

    void GetSourceOtherLibInclusions(HeaderFile::HeaderFileSet &headerFiles) const;

    void EmitCustomTypeDataProcess(StringBuilder &sb);

    void EmitCustomTypeMarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitCustomTypeUnmarshallingImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type);

    void EmitMarshallingVarDecl(const AutoPtr<ASTStructType> &type,
        const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    void EmitUnmarshallingVarDecl(const AutoPtr<ASTStructType> &type,
        const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    void EmitParamCheck(const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    void EmitPodTypeUnmarshalling(const std::string &typeName,
        const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    void EmitMemberUnmarshalling(const AutoPtr<ASTType> &type, const std::string &name, const std::string &memberName,
        StringBuilder &sb, const std::string &prefix);

    void EmitStringMemberUnmarshalling(const AutoPtr<HdiTypeEmitter> &type, const std::string &memberName,
        const std::string &varName, StringBuilder &sb, const std::string &prefix);

    void EmitArrayMemberUnmarshalling(const AutoPtr<ASTType> &type, const std::string &memberName,
        const std::string &varName, StringBuilder &sb, const std::string &prefix);

    void EmitCustomTypeFreeImpl(StringBuilder &sb, const AutoPtr<ASTStructType> &type) const;

    bool NeedEmitInitVar(const AutoPtr<ASTType> &type, bool needFree);

    void EmitCustomTypeMemoryRecycle(const AutoPtr<ASTStructType> &type,
        const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    void EmitUtilMethods(StringBuilder &sb, bool isDecl) override;

    std::vector<std::string> freeObjStatements_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_C_CUSTOM_TYPES_CODE_EMITTER_H