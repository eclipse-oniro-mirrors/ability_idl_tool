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

#include "ast/ast_namespace.h"

#include <algorithm>

#include "ast/ast_interface_type.h"
#include "ast/ast_rawdata_type.h"
#include "ast/ast_sequenceable_type.h"

namespace OHOS {
namespace Idl {
ASTNamespace::ASTNamespace(const std::string &nspaceStr) : name_(nspaceStr), outerNamespace_(nullptr) {}

void ASTNamespace::AddNamespace(const AutoPtr<ASTNamespace> &innerNspace)
{
    if (innerNspace == nullptr) {
        return;
    }

    innerNamespaces_.push_back(innerNspace);
    innerNspace->outerNamespace_ = this;
}

AutoPtr<ASTNamespace> ASTNamespace::FindNamespace(const std::string &nspaceStr)
{
    if (nspaceStr.empty()) {
        return nullptr;
    }

    auto resIter = std::find_if(
        innerNamespaces_.begin(), innerNamespaces_.end(), [nspaceStr](const AutoPtr<ASTNamespace> &element) {
            return element->name_ == nspaceStr;
        });
    return resIter != innerNamespaces_.end() ? *resIter : nullptr;
}

AutoPtr<ASTNamespace> ASTNamespace::GetNamespace(size_t index)
{
    if (index >= innerNamespaces_.size()) {
        return nullptr;
    }

    return innerNamespaces_[index];
}

void ASTNamespace::AddInterface(const AutoPtr<ASTInterfaceType> &interface)
{
    if (interface == nullptr) {
        return;
    }

    interfaces_.push_back(interface);
}

AutoPtr<ASTInterfaceType> ASTNamespace::GetInterface(size_t index)
{
    if (index >= interfaces_.size()) {
        return nullptr;
    }

    return interfaces_[index];
}

void ASTNamespace::AddSequenceable(const AutoPtr<ASTSequenceableType> &sequenceable)
{
    if (sequenceable == nullptr) {
        return;
    }

    sequenceables_.push_back(sequenceable);
}

AutoPtr<ASTSequenceableType> ASTNamespace::GetSequenceable(size_t index)
{
    if (index >= sequenceables_.size()) {
        return nullptr;
    }

    return sequenceables_[index];
}

void ASTNamespace::AddRawData(const AutoPtr<ASTRawDataType> &rawdata)
{
    if (rawdata == nullptr) {
        return;
    }

    rawdatas_.push_back(rawdata);
}

AutoPtr<ASTRawDataType> ASTNamespace::GetRawData(size_t index)
{
    if (index >= rawdatas_.size()) {
        return nullptr;
    }

    return rawdatas_[index];
}

std::string ASTNamespace::ToString() const
{
    std::string nspaceStr;
    const ASTNamespace *nspace = this;
    while (nspace != nullptr) {
        nspaceStr = nspace->name_ + "." + nspaceStr;
        nspace = nspace->outerNamespace_;
    }
    return nspaceStr;
}
} // namespace Idl
} // namespace OHOS