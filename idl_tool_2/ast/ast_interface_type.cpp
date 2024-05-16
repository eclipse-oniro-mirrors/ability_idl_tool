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

#include "ast/ast_interface_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void ASTInterfaceType::SetNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    ASTType::SetNamespace(nspace);
    if (namespace_ != nullptr) {
        namespace_->AddInterface(this);
    }
}

void ASTInterfaceType::AddMethod(const AutoPtr<ASTMethod> &method)
{
    if (method == nullptr) {
        return;
    }
    methods_.push_back(method);
}

AutoPtr<ASTMethod> ASTInterfaceType::GetMethod(size_t index)
{
    if (index >= methods_.size()) {
        return nullptr;
    }

    return methods_[index];
}

bool ASTInterfaceType::AddExtendsInterface(AutoPtr<ASTInterfaceType> interface)
{
    if (extendsInterface_ != nullptr) {
        return false;
    }
    extendsInterface_ = interface;
    return true;
}

void ASTInterfaceType::SetVersion(size_t majorVer, size_t minorVer)
{
    majorVersion_ = majorVer;
    minorVersion_ = minorVer;
}

std::vector<AutoPtr<ASTMethod>> ASTInterfaceType::GetMethodsBySystem(SystemLevel system) const
{
    std::vector<AutoPtr<ASTMethod>> methods;
    for (const auto &method : methods_) {
        if (method->GetAttribute()->Match(system)) {
            methods.push_back(method);
        }
    }
    return methods;
}

std::string ASTInterfaceType::GetSignature()
{
    std::string fullName = namespace_ != nullptr ?
            namespace_->ToString() + name_ : name_;
    return "L" + StringHelper::Replace(fullName, '.', '/') + ";";
}

bool ASTInterfaceType::IsInterfaceType()
{
    return true;
}

std::string ASTInterfaceType::Dump(const std::string &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        sb.AppendFormat("interface %s", (namespace_->ToString() + name_).c_str());
    } else {
        sb.Append(attr_->Dump(prefix));
        sb.AppendFormat("interface %s", name_.c_str());
    }
    if (isExternal_) {
        sb.Append(";\n");
    } else {
        sb.Append(" {\n");
        for (auto method : methods_) {
            sb.Append(method->Dump(prefix + "  "));
            if (method != methods_[methods_.size() - 1]) {
                sb.Append('\n');
            }
        }
        sb.Append(prefix).Append("}\n");
    }

    return sb.ToString();
}

TypeKind ASTInterfaceType::GetTypeKind()
{
    return TypeKind::TYPE_INTERFACE;
}

std::string ASTInterfaceType::GetFullName() const
{
    return namespace_->ToString() + name_;
}
} // namespace Idl
} // namespace OHOS
