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
#include <cstdint>

#include "metadata/metadata_reader.h"
#include "metadata/metadata_serializer.h"
#include "util/file.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {
std::string MetadataReader::tag_ = "MetadataReader";

std::shared_ptr<MetaComponent> MetadataReader::ReadMetadataFromFile(const std::string& filePath)
{
    File file(filePath, File::READ);
    if (!file.IsValid()) {
        Logger::E(tag_.c_str(), "Open \"%s\" file failed.", filePath.c_str());
        return nullptr;
    }

    if (!file.Reset()) {
        Logger::E(tag_.c_str(), "Reset \"%s\" file failed.", filePath.c_str());
        return nullptr;
    }

    MetaComponent header;

    if (!file.ReadData((void*)&header, sizeof(MetaComponent))) {
        Logger::E(tag_.c_str(), "Read \"%s\" file failed.", filePath.c_str());
        return nullptr;
    }

    if (header.magic_ != METADATA_MAGIC_NUMBER || header.size_ < 0 || header.size_ > UINT16_MAX) {
        Logger::E(tag_.c_str(), "The metadata in \"%s\" file is bad.", filePath.c_str());
        return nullptr;
    }

    if (!file.Reset()) {
        Logger::E(tag_.c_str(), "Reset \"%s\" file failed.", filePath.c_str());
        return nullptr;
    }

    void* data = malloc(header.size_);
    if (data == nullptr) {
        Logger::E(tag_.c_str(), "Malloc metadata failed.");
        return nullptr;
    }

    if (!file.ReadData(data, header.size_)) {
        Logger::E(tag_.c_str(), "Read \"%s\" file failed.", filePath.c_str());
        free(data);
        return nullptr;
    }

    std::shared_ptr<MetaComponent> metadata(reinterpret_cast<MetaComponent *>(data), [](MetaComponent* p) { free(p); });

    MetadataSerializer serializer((uintptr_t)data);
    serializer.Deserialize();

    return metadata;
}

std::unordered_map<std::string, AutoPtr<AST>> MetadataReader::ReadMetadataToAst()
{
    std::unordered_map<std::string, AutoPtr<AST>> allAsts;

    ast_ = new AST();
    for (int i = 0; i < metaComponent_->sequenceableNumber_; i++) {
        ReadMetaSequenceable(metaComponent_->sequenceables_[i]);
    }

    for (int i = 0; i < metaComponent_->rawdataNumber_; i++) {
        ReadMetaRawData(metaComponent_->rawdatas_[i]);
    }

    for (int i = 0; i < metaComponent_->interfaceNumber_; i++) {
        ReadMetaInterface(metaComponent_->interfaces_[i]);
    }

    ast_->SetFullName(std::string(reinterpret_cast<char*>(metaComponent_->name_)));
    ast_->SetAStFileType(ASTFileType::AST_IFACE);
    allAsts[std::string(reinterpret_cast<char*>(metaComponent_->name_))] = ast_;

    return allAsts;
}

void MetadataReader::ReadMetaSequenceable(MetaSequenceable* mp)
{
    AutoPtr<ASTSequenceableType> seqType = new ASTSequenceableType();

    seqType->SetName(std::string(reinterpret_cast<char*>(mp->name_)));
    seqType->SetNamespace(ast_->ParseNamespace(std::string(reinterpret_cast<char*>(mp->namespace_))));
    AutoPtr<AST> seqAst = new AST();
    seqAst->SetFullName(seqType->GetFullName());
    seqAst->AddSequenceableDef(seqType);
    seqAst->SetAStFileType(ASTFileType::AST_SEQUENCEABLE);
    ast_->AddImport(seqAst);
    ast_->AddSequenceableDef(seqType);
}

void MetadataReader::ReadMetaRawData(MetaRawData* mp)
{
    AutoPtr<ASTRawDataType> rawdataType = new ASTRawDataType();

    rawdataType->SetName(std::string(reinterpret_cast<char*>(mp->name_)));
    rawdataType->SetNamespace(ast_->ParseNamespace(std::string(reinterpret_cast<char*>(mp->namespace_))));
    AutoPtr<AST> rawdataAst = new AST();
    rawdataAst->SetFullName(rawdataType->GetFullName());
    rawdataAst->AddRawDataDef(rawdataType);
    rawdataAst->SetAStFileType(ASTFileType::AST_RAWDATA);
    ast_->AddImport(rawdataAst);
    ast_->AddRawDataDef(rawdataType);
}

void MetadataReader::ReadMetaInterface(MetaInterface* mi)
{
    AutoPtr<ASTInterfaceType> interface = new ASTInterfaceType();
    AutoPtr<ASTAttr> infAttr = new ASTAttr();
    if (mi->properties_ == INTERFACE_PROPERTY_ONEWAY) {
        infAttr->SetValue(ASTAttr::ONEWAY);
    }
    interface->SetAttribute(infAttr);
    if (!mi->external_) {
        interface->SetLicense(std::string(reinterpret_cast<char*>(mi->license_)));
        ast_->SetLicense(std::string(reinterpret_cast<char*>(mi->license_)));
    }
    interface->SetName(std::string(reinterpret_cast<char*>(mi->name_)));

    interface->SetNamespace(ast_->ParseNamespace(std::string(reinterpret_cast<char*>(mi->namespace_))));
    interface->SetExternal(mi->external_);
    ast_->AddInterfaceDef(interface);
    for (int i = 0; i < mi->methodNumber_; i++) {
        ReadMetaMethod(interface, mi->methods_[i]);
    }
}

void MetadataReader::ReadMetaMethod(AutoPtr<ASTInterfaceType>& interface, MetaMethod* mm)
{
    AutoPtr<ASTMethod> method = new ASTMethod();
    AutoPtr<ASTAttr> methodAttr = new ASTAttr();
    if (mm->properties_ == METHOD_PROPERTY_ONEWAY) {
        methodAttr->SetValue(ASTAttr::ONEWAY);
    }
    method->SetAttribute(methodAttr);

    MetaType* type = metaComponent_->types_[mm->returnTypeIndex_];
    method->SetReturnType(ReadMetaType(type));
    method->SetName(std::string(reinterpret_cast<char*>((mm->name_))));
    for (int i = 0; i < mm->parameterNumber_; i++) {
        ReadMetaParam(method, mm->parameters_[i]);
    }
    interface->AddMethod(method);
}

void MetadataReader::ReadMetaParam(AutoPtr<ASTMethod>& method, MetaParameter* mp)
{
    AutoPtr<ASTParamAttr> attr = new ASTParamAttr(ASTParamAttr::PARAM_NONE);

    if ((mp->attributes_ & ATTR_IN) == ATTR_IN) {
        attr->value_ |= ASTParamAttr::PARAM_IN;
    }

    if ((mp->attributes_ & ATTR_OUT) == ATTR_OUT) {
        attr->value_ |= ASTParamAttr::PARAM_OUT;
    }

    MetaType* type = metaComponent_->types_[mp->typeIndex_];
    AutoPtr<ASTParameter> param = new ASTParameter(std::string(reinterpret_cast<char*>((mp->name_))),
        attr, ReadMetaType(type));
    method->AddParameter(param);
}

AutoPtr<ASTType> MetadataReader::ReadMetaType(MetaType* type)
{
    std::string typeName = MetaTypeName(type);
    AutoPtr<ASTType> astType = ast_->FindType(typeName);
    switch (type->kind_) {
        case MetaTypeKind::List:
            if (astType == nullptr) {
                MetaType* elementMt = metaComponent_->types_[type->nestedTypeIndexes_[0]];
                AutoPtr<ASTListType> listType = new ASTListType();
                listType->SetElementType(ReadMetaType(elementMt));
                astType = listType.Get();
            }
            break;
        case MetaTypeKind::Map:
            if (astType == nullptr) {
                MetaType* keyMt = metaComponent_->types_[type->nestedTypeIndexes_[0]];
                MetaType* valueMt = metaComponent_->types_[type->nestedTypeIndexes_[1]];
                AutoPtr<ASTMapType> mapType = new ASTMapType();
                mapType->SetKeyType(ReadMetaType(keyMt));
                mapType->SetValueType(ReadMetaType(valueMt));
                astType = mapType.Get();
            }
            break;
        case MetaTypeKind::Array:
            if (astType == nullptr) {
                MetaType* elementMt = metaComponent_->types_[type->nestedTypeIndexes_[0]];
                AutoPtr<ASTArrayType> arrayType = new ASTArrayType();
                arrayType->SetElementType(ReadMetaType(elementMt));
                astType = arrayType.Get();
            }
            break;
        default:
            break;
    }
    ast_->AddType(astType);
    return astType;
}

std::string MetadataReader::MetaTypeName(MetaType* mt)
{
    switch (mt->kind_) {
        case MetaTypeKind::Char:
            return "char";
        case MetaTypeKind::Boolean:
            return "boolean";
        case MetaTypeKind::Byte:
            return "byte";
        case MetaTypeKind::Short:
            return "short";
        case MetaTypeKind::Integer:
            return "int";
        case MetaTypeKind::Long:
            return "long";
        case MetaTypeKind::Float:
            return "float";
        case MetaTypeKind::Double:
            return "double";
        case MetaTypeKind::String:
            return "String";
        case MetaTypeKind::Void:
            return "void";
        case MetaTypeKind::Sequenceable:
        case MetaTypeKind::RawData:
        case MetaTypeKind::Interface:
            return GetMetaTypeName(mt);
        case MetaTypeKind::List: {
            MetaType* elementMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return "List<" + MetaTypeName(elementMt) + ">";
        }
        case MetaTypeKind::Map: {
            MetaType* keyMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            MetaType* valueMt = metaComponent_->types_[mt->nestedTypeIndexes_[1]];
            return "Map<" + MetaTypeName(keyMt) + ", " + MetaTypeName(valueMt) + ">";
        }
        case MetaTypeKind::Array: {
            MetaType* elementMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return MetaTypeName(elementMt) + "[]";
        }
        case MetaTypeKind::Unknown:
        default:
            printf("Unknown %d\n", mt->index_);
            return "unknown";
    }
}

std::string MetadataReader::GetMetaTypeName(MetaType* mt)
{
    switch (mt->kind_) {
        case MetaTypeKind::Sequenceable: {
            MetaSequenceable* mp = metaComponent_->sequenceables_[mt->index_];
            return reinterpret_cast<char*>(mp->name_);
        }
        case MetaTypeKind::RawData: {
            MetaRawData* mr = metaComponent_->rawdatas_[mt->index_];
            return reinterpret_cast<char*>(mr->name_);
        }
        case MetaTypeKind::Interface: {
            MetaInterface* mi = metaComponent_->interfaces_[mt->index_];
            return reinterpret_cast<char*>(mi->name_);
        }
        default:
            return "unknown";
    }
}
} // namespace Idl
} // namespace OHOS
