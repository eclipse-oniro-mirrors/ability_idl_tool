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

#ifndef OHOS_IDL_SA_TYPE_EMITTER_H
#define OHOS_IDL_SA_TYPE_EMITTER_H

#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
class SaTypeEmitter : public LightRefCountBase {
public:
    void SetTypeName(const std::string &name);

    std::string GetTypeName();

    virtual TypeKind GetTypeKind();

    virtual std::string EmitCppType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual std::string EmitRustType(bool pt = false, TypeMode mode = TypeMode::NO_MODE) const;

    virtual std::string EmitTsType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual std::string EmitCppTypeDecl() const;

    virtual void EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, bool emitType = true) const;

    virtual void EmitRustReadVar(const std::string &result, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitRustWriteVar(const std::string &result, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitTsReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, TypeMode mode = TypeMode::NO_MODE) const;

    static bool logOn_;
    static int circleCount_;
    bool isParamInout = false;
    bool isProxy = false;

protected:
    std::string typeName_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_TYPE_EMITTER_H