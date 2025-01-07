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

#include "sa_rust_code_emitter.h"

namespace OHOS {
namespace Idl {
void RustCodeEmitter::EmitHeadMacro(StringBuilder &sb) const
{
    sb.Append("#![allow(missing_docs)]\n");
    sb.Append("#![allow(unused_variables)]\n");
    sb.Append("#![allow(unused_mut)]\n");
    sb.Append('\n');
}

void RustCodeEmitter::EmitCommonHeaders(StringBuilder &sb) const
{
    bool useMap = false;
    const AST::TypeStringMap &types = ast_->GetTypes();
    for (const auto &pair : types) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_MAP: {
                if (!useMap) {
                    sb.Append("use std::collections::HashMap;\n");
                    useMap = true;
                }
                break;
            }
            default:
                break;
        }
    }
    if (useMap) {
        sb.Append('\n');
    }
}

} // namespace Idl
} // namespace OHOS