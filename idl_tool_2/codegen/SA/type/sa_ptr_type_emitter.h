/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDL_SA_PTRTYPE_EMITTER_H
#define OHOS_IDL_SA_PTRTYPE_EMITTER_H

#include "codegen/SA/sa_type_emitter.h"

namespace OHOS {
namespace Idl {
class SaPtrTypeEmitter : public SaTypeEmitter {
public:
    explicit SaPtrTypeEmitter(TypeKind kind) : typeKind_(kind)
    {
        if (kind == TypeKind::TYPE_NULL_SHAREDPTR || kind == TypeKind::TYPE_NULL_UNIQUEPTR ||
            kind == TypeKind::TYPE_NULL_SPTR) {
            supportNullPtr_ = true;
        } else {
            supportNullPtr_ = false;
        }
        switch (kind) {
            case TypeKind::TYPE_SHAREDPTR:
            case TypeKind::TYPE_NULL_SHAREDPTR:
                ptrTypeName_ = "std::shared_ptr";
                return;
            case TypeKind::TYPE_UNIQUEPTR:
            case TypeKind::TYPE_NULL_UNIQUEPTR:
                ptrTypeName_ = "std::unique_ptr";
                return;
            case TypeKind::TYPE_SPTR:
            case TypeKind::TYPE_NULL_SPTR:
                ptrTypeName_ = "sptr";
                return;
            default:
                return;
        }
    }

    inline void SetElementEmitter(const AutoPtr<SaTypeEmitter> &elementEmitter)
    {
        elementEmitter_ = elementEmitter;
    }

    TypeKind GetTypeKind() override;

    std::string EmitCppType(TypeMode mode = TypeMode::NO_MODE) const override;

    void EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const override;

    void EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, bool emitType) const override;

protected:
    AutoPtr<SaTypeEmitter> elementEmitter_;

private:
    TypeKind typeKind_;
    std::string ptrTypeName_;
    bool supportNullPtr_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_PTRTYPE_EMITTER_H