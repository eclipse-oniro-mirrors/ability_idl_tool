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

#include "ast/ast_method.h"

#include "ast/ast_interface_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void ASTMethod::CheckOverload(AutoPtr<ASTInterfaceType> interface)
{
    if (interface == nullptr) {
        return;
    }
    interface = interface->GetExtendsInterface();
    while (interface != nullptr) {
        const std::vector<AutoPtr<ASTMethod>> methodMap =
            interface->GetMethodsBySystem(Options::GetInstance().GetSystemLevel());
        if (std::any_of(methodMap.begin(), methodMap.end(),
            [this] (const AutoPtr<ASTMethod> &method) { return name_ == method->GetName();})) {
            isOverload_ = true;
            return;
        }
        interface = interface->GetExtendsInterface();
    }
    isOverload_ = false;
}

std::string ASTMethod::GetSignature()
{
    if (signature_.empty()) {
        BuildSignature();
    }
    return signature_;
}

void ASTMethod::BuildSignature()
{
    StringBuilder sb;

    sb.AppendFormat("(%s)", returnType_->GetSignature().c_str());
    for (size_t i = 0; i < parameters_.size(); i++) {
        sb.Append(parameters_[i]->GetType()->GetSignature());
    }
    signature_ = sb.ToString();
}

void ASTMethod::AddParameter(const AutoPtr<ASTParameter> &parameter)
{
    if (parameter == nullptr) {
        return;
    }
    parameters_.push_back(parameter);
}

AutoPtr<ASTParameter> ASTMethod::GetParameter(size_t index)
{
    if (index >= parameters_.size()) {
        return nullptr;
    }

    return parameters_[index];
}

std::string ASTMethod::Dump(const std::string &prefix)
{
    StringBuilder sb;

    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        if (returnType_ != nullptr) {
            sb.Append(prefix).Append(returnType_->ToString()).Append(' ');
        }
    } else {
        sb.Append(prefix).Append(attr_->Dump(prefix));
    }
    sb.Append(name_).Append('(');
    if (!parameters_.empty()) {
        sb.Append('\n');
        for (const auto& parameter : parameters_) {
            std::string info = parameter->Dump(prefix + TAB);
            sb.Append(info);
            if (parameter != parameters_[parameters_.size() - 1]) {
                sb.Append(",\n");
            }
        }
    }
    sb.Append(");\n");

    return sb.ToString();
}
} // namespace Idl
} // namespace OHOS