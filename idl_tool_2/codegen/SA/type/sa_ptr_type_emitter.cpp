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

#include "sa_ptr_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaPtrTypeEmitter::GetTypeKind()
{
    return typeKind_;
}

std::string SaPtrTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("%s<%s>", ptrTypeName_.c_str(),
                elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const %s<%s>&", ptrTypeName_.c_str(),
                elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        case TypeMode::PARAM_OUT:
        case TypeMode::PARAM_INOUT:
            return StringHelper::Format("%s<%s>&", ptrTypeName_.c_str(),
                elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        default:
            return "unknown type";
    }
}

void SaPtrTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    EmitCppPtrVar emitCppPtrVar = { parcelName, name, prefix, ptrTypeName_, supportNullPtr_ };
    elementEmitter_->EmitCppPtrWriteVar(emitCppPtrVar, sb);
}

void SaPtrTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    EmitCppPtrVar emitCppPtrVar = { parcelName, name, prefix, ptrTypeName_, supportNullPtr_ };
    elementEmitter_->EmitCppPtrReadVar(emitCppPtrVar, sb, emitType);
}
} // namespace Idl
} // namespace OHOS