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

#ifndef OHOS_IDL_ASTU16STRINGTYPE_H
#define OHOS_IDL_ASTU16STRINGTYPE_H

#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
class ASTU16stringType : public ASTType {
public:
    ASTU16stringType() : ASTType(false, TypeKind::TYPE_U16STRING) {}

    std::string GetSignature() override;

    bool IsU16stringType() override;

    std::string ToString() const override;

    TypeKind GetTypeKind() override;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTU16STRINGTYPE_H