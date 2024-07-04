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

#ifndef OHOS_IDL_ASTSTRING16TYPE_H
#define OHOS_IDL_ASTSTRING16TYPE_H

#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
class ASTString16Type : public ASTType {
public:
    ASTString16Type() : ASTType(false, TypeKind::TYPE_STRING16) {}

    std::string GetSignature() override;

    bool IsStringType() override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTSTRING16TYPE_H