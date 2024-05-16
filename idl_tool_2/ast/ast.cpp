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

#include "ast/ast.h"

#include <cstdlib>

#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
AST::TypeStringMap AST::basicTypes_ = {
    {"void",           new ASTVoidType()     },
    {"boolean",        new ASTBooleanType()     },
    {"byte",           new ASTByteType()        },
    {"short",          new ASTShortType()       },
    {"int",            new ASTIntegerType()     },
    {"long",           new ASTLongType()        },
    {"float",          new ASTFloatType()       },
    {"double",         new ASTDoubleType()      },
    {"String",         new ASTStringType()      },
    {"char",           new ASTCharType()        },
    {"unsigned char",  new ASTUcharType()       },
    {"unsigned short", new ASTUshortType()      },
    {"unsigned int",   new ASTUintType()        },
    {"unsigned long",  new ASTUlongType()       },
    {"FileDescriptor", new ASTFdType()          },
    {"Ashmem",         new ASTAshmemType()      },
    {"NativeBuffer",   new ASTNativeBufferType()},
    {"Pointer",        new ASTPointerType()     },
};

void AST::SetIdlFile(const std::string &idlFile)
{
    idlFilePath_ = idlFile;
#ifdef __MINGW32__
    size_t index = idlFilePath_.rfind('\\');
#else
    size_t index = idlFilePath_.rfind('/');
#endif

    size_t end = idlFilePath_.rfind(".idl");
    if (end == std::string::npos) {
        end = idlFile.size();
    }

    name_ = StringHelper::SubStr(idlFilePath_, (index == std::string::npos) ? 0 : (index + 1), end);
}

void AST::SetFullName(const std::string &fullName)
{
    std::string name = fullName;
    size_t start = fullName.rfind("..");
    if (start != std::string::npos) {
        name = StringHelper::SubStr(fullName, start + strlen(".."));
    }
    size_t index = name.rfind('.');
    if (index != std::string::npos) {
        packageName_ = StringHelper::SubStr(name, 0, index);
        name_ = StringHelper::SubStr(name, index + 1);
    } else {
        packageName_ = "";
        name_ = name;
    }
}

void AST::SetPackageName(const std::string &packageName)
{
    packageName_ = packageName;
    ParseNamespace(packageName_);
}

AutoPtr<ASTNamespace> AST::ParseNamespace(const std::string &nspaceStr)
{
    AutoPtr<ASTNamespace> currNspace;
    size_t begin = 0;
    size_t index = 0;
    while ((index = nspaceStr.find('.', begin)) != std::string::npos) {
        std::string ns = StringHelper::SubStr(nspaceStr, begin, index);
        AutoPtr<ASTNamespace> nspace;
        if (currNspace == nullptr) {
            nspace = NewNameSpace(ns);
        } else {
            nspace = currNspace->FindNamespace(ns);
            if (nspace == nullptr) {
                nspace = new ASTNamespace(ns);
                currNspace->AddNamespace(nspace);
            }
        }
        currNspace = nspace;
        begin = index + 1;
    }
    if (currNspace == nullptr) {
        currNspace = NewNameSpace("");
    }
    return currNspace;
}

AutoPtr<ASTNamespace> AST::NewNameSpace(std::string nameSpace)
{
    AutoPtr<ASTNamespace> currNspace = FindNamespace(nameSpace);
    if (currNspace == nullptr) {
        currNspace = new ASTNamespace(nameSpace);
        AddNamespace(currNspace);
    }
    return currNspace;
}

void AST::AddNamespace(const AutoPtr<ASTNamespace> &nspace)
{
    if (nspace == nullptr) {
        return;
    }
    namespaces_.push_back(nspace);
}

AutoPtr<ASTNamespace> AST::FindNamespace(const std::string &nspaceStr)
{
    for (auto nspace : namespaces_) {
        if (nspace->ToShortString() == nspaceStr) {
            return nspace;
        }
    }
    return nullptr;
}

AutoPtr<ASTNamespace> AST::GetNamespace(size_t index)
{
    if (index >= namespaces_.size()) {
        return nullptr;
    }

    return namespaces_[index];
}

void AST::AddInterfaceDef(const AutoPtr<ASTInterfaceType> &interface)
{
    if (interface == nullptr) {
        return;
    }

    interfaceDefs_.push_back(interface);
    AddType(interface.Get());
}

AutoPtr<ASTInterfaceType> AST::GetInterfaceDef(size_t index)
{
    if (index >= interfaceDefs_.size()) {
        return nullptr;
    }

    return interfaceDefs_[index];
}

AutoPtr<ASTSequenceableType> AST::GetSequenceableDef(size_t index)
{
    if (index >= sequenceableDefs_.size()) {
        return nullptr;
    }

    return sequenceableDefs_[index];
}

void AST::AddSequenceableDef(const AutoPtr<ASTSequenceableType> &sequenceable)
{
    if (sequenceable == nullptr) {
        return;
    }

    sequenceableDefs_.push_back(sequenceable);
    AddType(sequenceable.Get());
}

int AST::IndexOf(ASTInterfaceType* interface)
{
    for (size_t i = 0; i < interfaceDefs_.size(); i++) {
        if (interfaceDefs_[i] == interface) {
            return i;
        }
    }
    return -1;
}

int AST::IndexOf(ASTSequenceableType* sequenceable)
{
    for (size_t i = 0; i < sequenceableDefs_.size(); i++) {
        if (sequenceableDefs_[i] == sequenceable) {
            return i;
        }
    }
    return -1;
}

int AST::IndexOf(ASTType* type)
{
    int i = 0;
    for (auto it = types_.begin(); it != types_.end(); ++it, ++i) {
        if (it->second == type) {
            return i;
        }
    }
    return -1;
}

void AST::AddType(const AutoPtr<ASTType> &type)
{
    if (type == nullptr) {
        return;
    }

    types_[type->ToString()] = type;
}

AutoPtr<ASTType> AST::FindType(const std::string &typeName, bool lookImports)
{
    if (typeName.empty()) {
        return nullptr;
    }

    for (const auto &type : types_) {
        if ((typeName.find('.') == std::string::npos && type.second->GetName() == typeName) ||
            type.first == typeName) {
            return type.second;
        }
    }

    auto basicTypePair = basicTypes_.find(typeName);
    if (basicTypePair != basicTypes_.end()) {
        return basicTypePair->second;
    }

    if (!lookImports) {
        return nullptr;
    }

    AutoPtr<ASTType> type = nullptr;
    for (const auto &importPair : imports_) {
        type = importPair.second->FindType(typeName, false);
        if (type != nullptr) {
            break;
        }
    }
    return type;
}

void AST::AddTypeDefinition(const AutoPtr<ASTType> &type)
{
    if (type == nullptr) {
        return;
    }

    AddType(type);
    typeDefinitions_.push_back(type);
}

AutoPtr<ASTType> AST::GetTypeDefintion(size_t index)
{
    if (index >= typeDefinitions_.size()) {
        return nullptr;
    }
    return typeDefinitions_[index];
}

std::string AST::Dump(const std::string &prefix)
{
    StringBuilder sb;

    sb.Append(prefix);
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        sb.Append("Module[");
    } else {
        sb.Append("AST[");
    }
    sb.Append("name: ").Append(name_).Append(" ");
    sb.Append("file: ").Append(idlFilePath_);
    sb.Append("]\n");

    if (!packageName_.empty()) {
        sb.Append("package ").Append(packageName_).Append(";");
        sb.Append('\n');
        sb.Append('\n');
    }

    if (imports_.size() > 0) {
        std::string Keyword = "import";
        if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
            Keyword = "sequenceable";
        }
        for (const auto &import : imports_) {
            sb.AppendFormat("%s %s;\n", Keyword.c_str(), import.first.c_str());
        }
        sb.Append("\n");
    }

    if (typeDefinitions_.size() > 0) {
        for (auto type : typeDefinitions_) {
            std::string info = type->Dump("");
            sb.Append(info).Append("\n");
        }
    }

    if (interfaceDefs_.size() > 0) {
        for (auto type : interfaceDefs_) {
            if (type->IsExternal()) {
                std::string info = type->Dump("");
                sb.Append(info).Append("\n");
            }
        }

        for (auto type : interfaceDefs_) {
            if (!type->IsExternal()) {
                std::string info = type->Dump("");
                sb.Append(info).Append("\n");
            }
        }
    }

    return sb.ToString();
}

bool AST::AddImport(const AutoPtr<AST> &importAst)
{
    if (imports_.find(importAst->GetFullName()) != imports_.end()) {
        return false;
    }

    imports_[importAst->GetFullName()] = importAst;

    return true;
}

void AST::SetVersion(size_t &majorVer, size_t &minorVer)
{
    majorVersion_ = majorVer;
    minorVersion_ = minorVer;
}

bool AST::IsValid()
{
    if (name_.empty()) {
        return false;
    }

    return interfaceDefs_.size() > 0;
}

} // namespace Idl
} // namespace OHOS