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

#ifndef OHOS_IDL_SA_ORDEREDMAPTYPE_EMITTER_H
#define OHOS_IDL_SA_ORDEREDMAPTYPE_EMITTER_H

#include "codegen/SA/sa_type_emitter.h"

namespace OHOS {
namespace Idl {
class SaOrderedMapTypeEmitter : public SaTypeEmitter {
public:
    inline void SetKeyEmitter(const AutoPtr<SaTypeEmitter> &keyEmitter)
    {
        keyEmitter_ = keyEmitter;
    }

    inline void SetValueEmitter(const AutoPtr<SaTypeEmitter> &valueEmitter)
    {
        valueEmitter_ = valueEmitter;
    }

    TypeKind GetTypeKind() override;

    std::string EmitCppType(TypeMode mode) const override;

    void EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix) const override;

    void EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, bool emitType) const override;

private:
    AutoPtr<SaTypeEmitter> keyEmitter_;
    AutoPtr<SaTypeEmitter> valueEmitter_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_SA_ORDEREDMAPTYPE_EMITTER_H