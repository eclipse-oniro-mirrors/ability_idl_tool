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

#ifndef OHOS_IDL_HDI_ENUMTYPE_EMITTER_H
#define OHOS_IDL_HDI_ENUMTYPE_EMITTER_H

#include "codegen/HDI/hdi_type_emitter.h"

namespace OHOS {
namespace Idl {
class HdiEnumValueEmitter : public HdiTypeEmitter {
public:
    explicit HdiEnumValueEmitter(const std::string &name, const std::string &value) : mName_(name), value_(value) {}

    inline ~HdiEnumValueEmitter() override {}

    inline std::string GetValue()
    {
        return value_;
    }

    inline std::string GetValueName()
    {
        return mName_;
    }

private:
    std::string mName_;
    std::string value_;
};

class HdiEnumTypeEmitter : public HdiTypeEmitter {
public:
    inline void AddMember(const AutoPtr<HdiEnumValueEmitter> &member)
    {
        members_.push_back(member);
    }

    void SetBaseTypeName(const std::string &name);

    TypeKind GetTypeKind() override;

    std::string EmitCType(TypeMode mode = TypeMode::NO_MODE) const override;

    std::string EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    std::string EmitJavaType(TypeMode mode, bool isInnerType = false) const override;

    std::string EmitCTypeDecl() const override;

    std::string EmitCppTypeDecl() const override;

    void EmitCWriteVar(TypeMode mode, const std::string &name, const std::string &gotoLabel,
        StringBuilder &sb, const std::string &prefix) const override;

    void EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix) const override;

    void EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix) const override;

    void EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const override;

    void EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
        TypeMode mode, unsigned int innerLevel = 0) const override;

    void EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const override;

    void EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix, std::vector<std::string> &freeObjStatements) const override;

    void EmitCppMarshalling(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const override;

    void EmitCppUnMarshalling(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const override;
private:
    std::vector<AutoPtr<HdiEnumValueEmitter>> members_;
    std::string baseTypeName_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_ENUMTYPE_EMITTER_H