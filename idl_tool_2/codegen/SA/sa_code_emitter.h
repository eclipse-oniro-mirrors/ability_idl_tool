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

#ifndef OHOS_IDL_SA_CODE_EMITTER_H
#define OHOS_IDL_SA_CODE_EMITTER_H

#include "codegen/code_emitter.h"
#include "sa_type_emitter.h"

namespace OHOS {
namespace Idl {
class SACodeEmitter : public CodeEmitter {
public:
    using TypeEmitterMap = std::unordered_map<TypeKind, AutoPtr<SaTypeEmitter>>;

    ~SACodeEmitter() override = default;

    bool OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode) override;

protected:
    bool ResolveDirectory(const std::string &targetDirectory) override;

    bool Reset(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode);

    void CleanData();

    void GetOption();

    AutoPtr<SaTypeEmitter> GetTypeEmitter(AutoPtr<ASTType> astType) const;

    std::string domainId_;
    std::string logTag_;
    std::string hitraceTag_;
    std::string macroError_ = "HiLog::Error(LABEL";
    std::string macroWarn_ = "HiLog::Warn(LABEL";
    std::string macroDefine_ = "using OHOS::HiviewDFX::HiLog;\n\n";
    bool hitraceOn_ = false;
    bool logOn_ = false;

private:
    std::string GetNameWithNamespace(AutoPtr<ASTNamespace> space, const std::string name) const;

    AutoPtr<SaTypeEmitter> NewTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewMapTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewOrderedMapTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewArrayTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewListTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewPtrTypeEmitter(AutoPtr<ASTType> astType, TypeKind kind) const;

    AutoPtr<SaTypeEmitter> NewSetTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewEnumTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewStructTypeEmitter(AutoPtr<ASTType> astType) const;

    AutoPtr<SaTypeEmitter> NewUnionTypeEmitter(AutoPtr<ASTType> astType) const;

    static TypeEmitterMap basicEmitters_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_CODE_EMITTER_H
