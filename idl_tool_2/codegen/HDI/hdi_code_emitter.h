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

#ifndef OHOS_IDL_HDI_CODE_EMITTER_H
#define OHOS_IDL_HDI_CODE_EMITTER_H

#include "codegen/code_emitter.h"
#include "hdi_type_emitter.h"

namespace OHOS {
namespace Idl {
class HDICodeEmitter : public CodeEmitter {
public:
    using TypeEmitterMap = std::unordered_map<TypeKind, AutoPtr<HdiTypeEmitter>>;

    ~HDICodeEmitter() override = default;

    bool OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode) override;

    virtual void EmitUtilMethods(StringBuilder &sb, bool isDecl);

protected:
    bool Reset(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode);

    void CleanData();

    AutoPtr<HdiTypeEmitter> GetTypeEmitter(AutoPtr<ASTType> astType) const;

    void EmitInterfaceBuffSizeMacro(StringBuilder &sb) const;

    void EmitUtilMethodMap(StringBuilder &sb, const UtilMethodMap &methods);

    GenMode mode_ = GenMode::IPC;
    std::string implName_;
    std::string implFullName_;
    std::string majorVerName_;
    std::string minorVerName_;

    std::string flagOfSetMemName_;
    std::string optionName_;

private:
    std::string GetNameWithNamespace(AutoPtr<ASTNamespace> space, std::string name) const;

    AutoPtr<HdiTypeEmitter> NewTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewMapTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewArrayTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewListTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewEnumTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewStructTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewUnionTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<HdiTypeEmitter> NewSmqTypeEmitter(AutoPtr<ASTType> astType) const;

    static TypeEmitterMap basicEmitters_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CODE_EMITTER_H
