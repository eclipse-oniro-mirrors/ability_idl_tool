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

#include "sa_code_emitter.h"
#include "ast/ast_map_type.h"
#include "type/sa_boolean_type_emitter.h"
#include "type/sa_byte_type_emitter.h"
#include "type/sa_short_type_emitter.h"
#include "type/sa_int_type_emitter.h"
#include "type/sa_long_type_emitter.h"
#include "type/sa_float_type_emitter.h"
#include "type/sa_double_type_emitter.h"
#include "type/sa_char_type_emitter.h"
#include "type/sa_string_type_emitter.h"
#include "type/sa_seq_type_emitter.h"
#include "type/sa_interface_type_emitter.h"
#include "type/sa_map_type_emitter.h"
#include "type/sa_array_type_emitter.h"

#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include "util/file.h"
#include "util/options.h"
#include "util/logger.h"
#include "util/string_helper.h"

namespace OHOS {
namespace Idl {
SACodeEmitter::TypeEmitterMap SACodeEmitter::basicEmitters_ = {
    {TypeKind::TYPE_BOOLEAN,           new SaBooleanTypeEmitter()     },
    {TypeKind::TYPE_BYTE,              new SaByteTypeEmitter()        },
    {TypeKind::TYPE_SHORT,             new SaShortTypeEmitter()       },
    {TypeKind::TYPE_INT,               new SaIntTypeEmitter()         },
    {TypeKind::TYPE_LONG,              new SaLongTypeEmitter()        },
    {TypeKind::TYPE_FLOAT,             new SaFloatTypeEmitter()       },
    {TypeKind::TYPE_DOUBLE,            new SaDoubleTypeEmitter()      },
    {TypeKind::TYPE_CHAR,              new SaCharTypeEmitter()        },
    {TypeKind::TYPE_STRING,            new SaStringTypeEmitter()      },
};

bool SACodeEmitter::OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if (!Reset(ast, targetDirectory, mode)) {
        return false;
    }

    EmitCode();
    return true;
}

bool SACodeEmitter::Reset(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode)
{
    if ((ast == nullptr) || (ast->GetASTFileType() != ASTFileType::AST_IFACE)) {
        return false;
    }

    CleanData();

    ast_ = ast;
    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == false) {
            interface_ = interface;
            break;
        }
    }
    if (interface_ != nullptr) {
        interfaceName_ = interface_->GetName();
        interfaceFullName_ = interface_->GetNamespace()->ToString() + interfaceName_;
        baseName_ = StringHelper::StartWith(interfaceName_, "I") ? interfaceName_.substr(1) : interfaceName_;
        proxyName_ = baseName_ + "Proxy";
        proxyFullName_ = interface_->GetNamespace()->ToString() + proxyName_;

        stubName_ = baseName_ + "Stub";
        stubFullName_ = interface_->GetNamespace()->ToString() + stubName_;
    }

    if (!ResolveDirectory(targetDirectory)) {
        return false;
    }

    return true;
}

void SACodeEmitter::CleanData()
{
    Options &options = Options::GetInstance();

    domainId_ = options.GetDomainId();
    logTag_ = options.GetLogTag();
    hitraceTag_ = options.GetGenerateHitraceTag();
    hitraceOn_ = options.DoHitraceState();
    logOn_ = options.DoLogOn();
    SaTypeEmitter::logOn_ = logOn_;
    ast_ = nullptr;
    interface_ = nullptr;
    directory_ = "";
    interfaceName_ = "";
    interfaceFullName_ = "";
    baseName_ = "";
    proxyName_ = "";
    proxyFullName_ = "";
    stubName_ = "";
    stubFullName_ = "";
}

bool SACodeEmitter::ResolveDirectory(const std::string &targetDirectory)
{
    directory_ = targetDirectory;
#ifdef __MINGW32__
    if (targetDirectory.find(":\\") == std::string::npos) {
        char* cwd = getcwd(nullptr, 0);
        directory_ = StringHelper::Format("%s\\%s", cwd, targetDirectory.c_str());
        free(cwd);
    }
#else
    if (!StringHelper::StartWith(targetDirectory, "/")) {
        char* cwd = getcwd(nullptr, 0);
        directory_ = StringHelper::Format("%s/%s", cwd, targetDirectory.c_str());
        free(cwd);
    }
#endif

    if (!access(directory_.c_str(), R_OK | W_OK)) {
        return true;
    }

#ifdef __MINGW32__
    if (mkdir(directory_.c_str()) != 0) {
#else
    if (mkdir(directory_.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
#endif
        Logger::E(TAG, "Create \"%s\" directory failed.", directory_.c_str());
        return false;
    }

    return true;
}

AutoPtr<SaTypeEmitter> SACodeEmitter::GetTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaTypeEmitter> typeEmitter;
    auto basicTypePair = basicEmitters_.find(astType->GetTypeKind());
    if (basicTypePair != basicEmitters_.end()) {
        typeEmitter = (static_cast<SaTypeEmitter*>(basicTypePair->second.Get()));
    }

    if (typeEmitter == nullptr) {
        typeEmitter = NewTypeEmitter(astType);
    }

    typeEmitter->SetTypeName(astType->ToString());
    return typeEmitter;
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewTypeEmitter(AutoPtr<ASTType> astType) const
{
    switch (astType->GetTypeKind()) {
        case TypeKind::TYPE_MAP:
            return NewMapTypeEmitter(astType);
        case TypeKind::TYPE_ARRAY:
            return NewArrayTypeEmitter(astType);
        case TypeKind::TYPE_LIST:
            return NewListTypeEmitter(astType);
        case TypeKind::TYPE_SEQUENCEABLE:
            return new SaSeqTypeEmitter();
        case TypeKind::TYPE_INTERFACE:
            return new SaInterfaceTypeEmitter();
        default: {
            // type not match, new a empty emitter
            AutoPtr<SaTypeEmitter> typeEmitter = new SaTypeEmitter();
            return typeEmitter;
        }
    }
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewMapTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaMapTypeEmitter> mapTypeEmitter = new SaMapTypeEmitter();
    AutoPtr<ASTType> keyType = (static_cast<ASTMapType*>(astType.Get()))->GetKeyType();
    AutoPtr<ASTType> valueType = (static_cast<ASTMapType*>(astType.Get()))->GetValueType();
    AutoPtr<SaTypeEmitter> keyEmitter = GetTypeEmitter(keyType);
    AutoPtr<SaTypeEmitter> valueEmitter = GetTypeEmitter(valueType);
    mapTypeEmitter->SetKeyEmitter(keyEmitter);
    mapTypeEmitter->SetValueEmitter(valueEmitter);
    return mapTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewArrayTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaArrayTypeEmitter> arrayTypeEmitter = new SaArrayTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTArrayType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    arrayTypeEmitter->SetElementEmitter(elemEmitter);
    return arrayTypeEmitter.Get();
}

AutoPtr<SaTypeEmitter> SACodeEmitter::NewListTypeEmitter(AutoPtr<ASTType> astType) const
{
    AutoPtr<SaListTypeEmitter> listTypeEmitter = new SaListTypeEmitter();
    AutoPtr<ASTType> elemType = (static_cast<ASTListType*>(astType.Get()))->GetElementType();
    AutoPtr<SaTypeEmitter> elemEmitter = GetTypeEmitter(elemType);
    listTypeEmitter->SetElementEmitter(elemEmitter);
    return listTypeEmitter.Get();
}
} // namespace Idl
} // namespace OHOS
