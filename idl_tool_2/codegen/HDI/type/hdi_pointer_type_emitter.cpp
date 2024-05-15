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

#include "hdi_pointer_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiPointerTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_POINTER;
}

std::string HdiPointerTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "void*";
        case TypeMode::PARAM_IN:
            return "const void*";
        case TypeMode::PARAM_OUT:
            return "void**";
        case TypeMode::LOCAL_VAR:
            return "void*";
        default:
            return "unknow type";
    }
}

std::string HdiPointerTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return "void*";
        case TypeMode::PARAM_IN:
            return "const void*";
        case TypeMode::PARAM_OUT:
            return "void*&";
        case TypeMode::LOCAL_VAR:
            return "void*";
        default:
            return "unknow type";
    }
}
} // namespace Idl
} // namespace OHOS