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

#include "metadata/metadata_builder.h"
#include <cstring>
#include "securec.h"
#include "ast/ast_array_type.h"
#include "ast/ast_map_type.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
std::string MetadataBuilder::tag_ = "MetadataBuilder";
std::shared_ptr<MetaComponent> MetadataBuilder::Build()
{
    if (!module_->IsValid()) {
        Logger::E(tag_.c_str(), "The module is not validate.");
        return nullptr;
    }

    size_ = CalculateMetadataSize();
    if (size_ > 0) {
        void* metadata =  calloc(size_, 1);
        if (metadata == nullptr) {
            Logger::E(tag_.c_str(), "Out of memory.");
            return nullptr;
        }
        metaComponent_.reset(
            new(metadata) MetaComponent, [](MetaComponent* p) { free(p); });

        WriteMetadata(reinterpret_cast<uintptr_t>(metadata));
    } else {
        return nullptr;
    }

    return metaComponent_;
}

size_t MetadataBuilder::CalculateMetadataSize()
{
    baseAddr_ = 0;
    CalculateMetaComponent(module_);
    return baseAddr_;
}

void MetadataBuilder::CalculateMetaComponent(AST* module)
{
    size_t namespaceNumber = module->GetNamespaceNumber();
    size_t sequenceableNumber = module->GetSequenceableDefNumber();
    size_t interfaceNumber = module->GetInterfaceDefNumber();
    size_t typeNumber = module->GetTypeNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(module_->GetName());
    // namespaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaComponent));
    // sequenceables_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaNamespace*) * namespaceNumber);
    // interfaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaSequenceable*) * sequenceableNumber);
    // types_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaInterface*) * interfaceNumber);
    // stringPool_'s address
    baseAddr_ = baseAddr_ + sizeof(MetaType*) * typeNumber;

    for (size_t i = 0; i < namespaceNumber; i++) {
        CalculateMetaNamespace(module->GetNamespace(i));
    }

    for (size_t i = 0; i < sequenceableNumber; i++) {
        CalculateMetaSequenceable(module->GetSequenceableDef(i));
    }

    for (size_t i = 0; i < interfaceNumber; i++) {
        CalculateMetaInterface(module->GetInterfaceDef(i));
    }

    const AST::TypeStringMap& types = module_->GetTypes();
    for (const auto& pair : types) {
        CalculateMetaType(pair.second);
    }

    // end address
    CalculateStringPool();
}

void MetadataBuilder::CalculateMetaNamespace(ASTNamespace* nspace)
{
    size_t sequenceableNumber = nspace->GetSequenceableNumber();
    size_t interfaceNumber = nspace->GetInterfaceNumber();
    size_t namespaceNumber = nspace->GetNamespaceNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(nspace->GetName());
    // sequenceables_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaNamespace));
    // interfaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(int) * sequenceableNumber);
    // namespaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(int) * interfaceNumber);
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaNamespace*) * namespaceNumber;

    for (size_t i = 0; i < namespaceNumber; i++) {
        CalculateMetaNamespace(nspace->GetNamespace(i));
    }
}

void MetadataBuilder::CalculateMetaSequenceable(ASTSequenceableType* sequenceable)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(sequenceable->GetName());
    stringPool_.Add(sequenceable->GetNamespace()->ToString());
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaSequenceable);
}

void MetadataBuilder::CalculateMetaInterface(ASTInterfaceType* interface)
{
    size_t methodNumber = interface->GetMethodNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(interface->GetLicense());
    stringPool_.Add(interface->GetName());
    stringPool_.Add(interface->GetNamespace()->ToString());
    // methods_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaInterface));
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaMethod*) * methodNumber;

    for (size_t i = 0; i < methodNumber; i++) {
        CalculateMetaMethod(interface->GetMethod(i));
    }
}

void MetadataBuilder::CalculateMetaMethod(ASTMethod* method)
{
    size_t parameterNumber = method->GetParameterNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(method->GetName());
    stringPool_.Add(method->GetSignature());
    // parameters_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaMethod));
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaParameter*) * parameterNumber;

    for (size_t i = 0; i < parameterNumber; i++) {
        CalculateMetaParameter(method->GetParameter(i));
    }
}

void MetadataBuilder::CalculateMetaParameter(ASTParameter* parameter)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    stringPool_.Add(parameter->GetName());
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaParameter);
}

void MetadataBuilder::CalculateMetaType(ASTType* type)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    // nestedTypeIndexes_'s address
    baseAddr_ = baseAddr_ + sizeof(MetaType);
    if (type->IsListType()) {
        baseAddr_ = Align8(baseAddr_);
        // end address
        baseAddr_ = baseAddr_ + sizeof(int*);
    } else if (type->IsMapType()) {
        // end address
        size_t typeNumber = 2;
        baseAddr_ = baseAddr_ + sizeof(int*) * typeNumber;
    } else if (type->IsArrayType()) {
        baseAddr_ = baseAddr_ + sizeof(int*);
    }
}

void MetadataBuilder::CalculateStringPool()
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    // end address
    baseAddr_ = baseAddr_ + stringPool_.GetSize();
}

void MetadataBuilder::WriteMetadata(uintptr_t base)
{
    baseAddr_ = base;
    WriteMetaComponent(module_);
}

void MetadataBuilder::WriteMetaComponent(AST* module)
{
    size_t namespaceNumber = module->GetNamespaceNumber();
    size_t sequenceableNumber = module->GetSequenceableDefNumber();
    size_t interfaceNumber = module->GetInterfaceDefNumber();
    size_t typeNumber = module->GetTypeNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaComponent* mc = reinterpret_cast<MetaComponent*>(baseAddr_);
    mc->magic_ = METADATA_MAGIC_NUMBER;
    mc->size_ = static_cast<int>(size_);
    mc->namespaceNumber_ = static_cast<int>(namespaceNumber);
    mc->sequenceableNumber_ = static_cast<int>(sequenceableNumber);
    mc->interfaceNumber_ = static_cast<int>(interfaceNumber);
    mc->typeNumber_ = static_cast<int>(typeNumber);
    mc->stringPoolSize_ = stringPool_.GetSize();
    // namespaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaComponent));
    mc->namespaces_ = reinterpret_cast<MetaNamespace**>(baseAddr_);
    // sequenceables_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaNamespace*) * namespaceNumber);
    mc->sequenceables_ = reinterpret_cast<MetaSequenceable**>(baseAddr_);
    // interfaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaSequenceable*) * sequenceableNumber);
    mc->interfaces_ = reinterpret_cast<MetaInterface**>(baseAddr_);
    // types_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaInterface*) * interfaceNumber);
    mc->types_ = reinterpret_cast<MetaType**>(baseAddr_);
    // stringPool_'s address
    baseAddr_ = baseAddr_ + sizeof(MetaType*) * typeNumber;
    mc->stringPool_ = reinterpret_cast<char*>(baseAddr_);
    // end address
    baseAddr_ = baseAddr_ + stringPool_.GetSize();
    (void)memcpy_s(mc->stringPool_, stringPool_.GetSize(), stringPool_.GetData(), stringPool_.GetSize());

    mc->name_ = WriteString(module->GetName());

    for (size_t i = 0; i < namespaceNumber; i++) {
        mc->namespaces_[i] = WriteMetaNamespace(module->GetNamespace(i));
    }

    for (size_t i = 0; i < sequenceableNumber; i++) {
        mc->sequenceables_[i] = WriteMetaSequenceable(module->GetSequenceableDef(i));
    }

    for (size_t i = 0; i < interfaceNumber; i++) {
        mc->interfaces_[i] = WriteMetaInterface(module->GetInterfaceDef(i));
    }

    const AST::TypeStringMap& types = module->GetTypes();
    int i = 0;
    for (const auto& pair : types) {
        mc->types_[i++] = WriteMetaType(pair.second);
    }
}

MetaNamespace* MetadataBuilder::WriteMetaNamespace(ASTNamespace* nspace)
{
    size_t sequenceableNumber = nspace->GetSequenceableNumber();
    size_t interfaceNumber = nspace->GetInterfaceNumber();
    size_t namespaceNumber = nspace->GetNamespaceNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaNamespace* mn = reinterpret_cast<MetaNamespace*>(baseAddr_);
    mn->name_ = WriteString(nspace->GetName());
    mn->sequenceableNumber_ = static_cast<int>(sequenceableNumber);
    mn->interfaceNumber_ = static_cast<int>(interfaceNumber);
    mn->namespaceNumber_ = static_cast<int>(namespaceNumber);
    // sequenceables_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaNamespace));
    mn->sequenceableIndexes_ = reinterpret_cast<int*>(baseAddr_);
    // interfaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(int) * sequenceableNumber);
    mn->interfaceIndexes_ = reinterpret_cast<int*>(baseAddr_);
    // namespaces_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(int) * interfaceNumber);
    mn->namespaces_ = reinterpret_cast<MetaNamespace**>(baseAddr_);
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaNamespace*) * namespaceNumber;

    for (size_t i = 0; i < sequenceableNumber; i++) {
        AutoPtr<ASTSequenceableType> sequenceable = nspace->GetSequenceable(i);
        mn->sequenceableIndexes_[i] = module_->IndexOf(sequenceable.Get());
    }

    for (size_t i = 0; i < interfaceNumber; i++) {
        AutoPtr<ASTInterfaceType> interface = nspace->GetInterface(i);
        mn->interfaceIndexes_[i] = module_->IndexOf(interface.Get());
    }

    for (size_t i = 0; i < namespaceNumber; i++) {
        AutoPtr<ASTNamespace> inner = nspace->GetNamespace(i);
        mn->namespaces_[i] = WriteMetaNamespace(inner);
    }

    return mn;
}

MetaSequenceable* MetadataBuilder::WriteMetaSequenceable(ASTSequenceableType* parcelabe)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaSequenceable* mp = reinterpret_cast<MetaSequenceable*>(baseAddr_);
    mp->name_ = WriteString(parcelabe->GetName());
    mp->namespace_ = WriteString(parcelabe->GetNamespace()->ToString());
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaSequenceable);

    return mp;
}

MetaInterface* MetadataBuilder::WriteMetaInterface(ASTInterfaceType* interface)
{
    size_t methodNumber = interface->GetMethodNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaInterface* mi = reinterpret_cast<MetaInterface*>(baseAddr_);
    mi->license_ = WriteString(interface->GetLicense());
    mi->name_ = WriteString(interface->GetName());
    mi->namespace_ = WriteString(interface->GetNamespace()->ToString());
    mi->properties_ = interface->GetAttribute()->GetValue() == ASTAttr::ONEWAY ? INTERFACE_PROPERTY_ONEWAY : 0;
    mi->methodNumber_ = static_cast<int>(methodNumber);
    mi->external_ = interface->IsExternal();
    // methods_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaInterface));
    mi->methods_ = reinterpret_cast<MetaMethod**>(baseAddr_);
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaMethod*) * methodNumber;

    for (size_t i = 0; i < methodNumber; i++) {
        mi->methods_[i] = WriteMetaMethod(interface->GetMethod(i));
    }

    return mi;
}

MetaMethod* MetadataBuilder::WriteMetaMethod(ASTMethod* method)
{
    size_t parameterNumber = method->GetParameterNumber();

    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaMethod* mm = reinterpret_cast<MetaMethod*>(baseAddr_);
    mm->name_ = WriteString(method->GetName());
    mm->signature_ = WriteString(method->GetSignature());
    mm->properties_ = method->GetAttribute()->GetValue() == ASTAttr::ONEWAY ? METHOD_PROPERTY_ONEWAY : 0;
    mm->returnTypeIndex_ = module_->IndexOf(method->GetReturnType());
    mm->parameterNumber_ = static_cast<int>(parameterNumber);
    // parameters_'s address
    baseAddr_ = Align8(baseAddr_ + sizeof(MetaMethod));
    mm->parameters_ = reinterpret_cast<MetaParameter**>(baseAddr_);
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaParameter*) * parameterNumber;

    for (size_t i = 0; i < parameterNumber; i++) {
        mm->parameters_[i] = WriteMetaParameter(method->GetParameter(i));
    }

    return mm;
}

MetaParameter* MetadataBuilder::WriteMetaParameter(ASTParameter* parameter)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaParameter* mp = reinterpret_cast<MetaParameter*>(baseAddr_);
    mp->name_ = WriteString(parameter->GetName());
    if (parameter->GetAttribute() & ASTParamAttr::PARAM_IN) {
        mp->attributes_ |= ATTR_IN;
    }
    if (parameter->GetAttribute() & ASTParamAttr::PARAM_OUT) {
        mp->attributes_ |= ATTR_OUT;
    }
    mp->typeIndex_ = module_->IndexOf(parameter->GetType());
    // end address
    baseAddr_ = baseAddr_ + sizeof(MetaParameter);

    return mp;
}

MetaType* MetadataBuilder::WriteMetaType(ASTType* type)
{
    // begin address
    baseAddr_ = Align8(baseAddr_);
    MetaType* mt = reinterpret_cast<MetaType*>(baseAddr_);
    mt->kind_ = Type2Kind(type);
    if (type->IsSequenceableType()) {
        mt->index_ = module_->IndexOf(static_cast<ASTSequenceableType*>(type));
    } else if (type->IsInterfaceType()) {
        mt->index_ = module_->IndexOf(static_cast<ASTInterfaceType*>(type));
    } else {
        mt->index_ = module_->IndexOf(type);
    }
    baseAddr_ = baseAddr_ + sizeof(MetaType);
    if (type->IsListType()) {
        mt->nestedTypeNumber_ = 1;
        // nestedTypeIndexes_'s address
        baseAddr_ = Align8(baseAddr_);
        mt->nestedTypeIndexes_ = reinterpret_cast<int*>(baseAddr_);
        AutoPtr<ASTType> elementType = (static_cast<ASTListType*>(type))->GetElementType();
        mt->nestedTypeIndexes_[0] = module_->IndexOf(elementType);
        // end address
        baseAddr_ = baseAddr_ + sizeof(int*);
    } else if (type->IsMapType()) {
        size_t typeNumber = 2;
        mt->nestedTypeNumber_ = static_cast<int>(typeNumber);
        // nestedTypeIndexes_'s address
        baseAddr_ = Align8(baseAddr_);
        mt->nestedTypeIndexes_ = reinterpret_cast<int*>(baseAddr_);
        AutoPtr<ASTType> keyType = (static_cast<ASTMapType*>(type))->GetKeyType();
        AutoPtr<ASTType> valueType = (static_cast<ASTMapType*>(type))->GetValueType();
        mt->nestedTypeIndexes_[0] = module_->IndexOf(keyType);
        mt->nestedTypeIndexes_[1] = module_->IndexOf(valueType);
        // end address
        baseAddr_ = baseAddr_ + sizeof(int*) * typeNumber;
    } else if (type->IsArrayType()) {
        mt->nestedTypeNumber_ = 1;
        // nestedTypeIndexes_'s address
        baseAddr_ = Align8(baseAddr_);
        mt->nestedTypeIndexes_ = reinterpret_cast<int*>(baseAddr_);
        AutoPtr<ASTType> elementType = (static_cast<ASTArrayType*>(type))->GetElementType();
        mt->nestedTypeIndexes_[0] = module_->IndexOf(elementType);
        // end address
        baseAddr_ = baseAddr_ + sizeof(int*);
    }

    return mt;
}

char* MetadataBuilder::WriteString(const std::string& string)
{
    return string.empty() ? nullptr : metaComponent_->stringPool_ + stringPool_.GetOffset(string);
}

MetaTypeKind MetadataBuilder::Type2Kind(ASTType* type)
{
    if (type->IsCharType()) {
        return MetaTypeKind::Char;
    } else if (type->IsBooleanType()) {
        return MetaTypeKind::Boolean;
    } else if (type->IsByteType()) {
        return MetaTypeKind::Byte;
    } else if (type->IsShortType()) {
        return MetaTypeKind::Short;
    } else if (type->IsIntegerType()) {
        return MetaTypeKind::Integer;
    } else if (type->IsLongType()) {
        return MetaTypeKind::Long;
    } else if (type->IsFloatType()) {
        return MetaTypeKind::Float;
    } else if (type->IsDoubleType()) {
        return MetaTypeKind::Double;
    } else if (type->IsStringType()) {
        return MetaTypeKind::String;
    } else if (type->IsVoidType()) {
        return MetaTypeKind::Void;
    } else if (type->IsSequenceableType()) {
        return MetaTypeKind::Sequenceable;
    } else if (type->IsInterfaceType()) {
        return MetaTypeKind::Interface;
    } else if (type->IsListType()) {
        return MetaTypeKind::List;
    } else if (type->IsMapType()) {
        return MetaTypeKind::Map;
    } else if (type->IsArrayType()) {
        return MetaTypeKind::Array;
    }
    return MetaTypeKind::Unknown;
}
} // namespace Idl
} // namespace OHOS
