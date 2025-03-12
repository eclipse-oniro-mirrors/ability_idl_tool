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

#include "metadata/metadata_dumper.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
std::string MetadataDumper::tab_ = "    ";
std::string MetadataDumper::Dump(const std::string& prefix)
{
    if (metaComponent_ == nullptr) {
        return "";
    }

    return DumpMetaComponent(metaComponent_, prefix);
}

std::string MetadataDumper::DumpMetaComponent(MetaComponent* mc, const std::string& prefix)
{
    StringBuilder sb;

    sb.Append(prefix).Append("MetaComponent\n");
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"magic_\" : \"0x%x\",\n", mc->magic_);
    sb.Append(prefix + tab_).AppendFormat("\"size_\" : \"%d\",\n", mc->size_);
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mc->name_);
    sb.Append(prefix + tab_).AppendFormat("\"namespaceNumber_\" : \"%d\",\n", mc->namespaceNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"sequenceableNumber_\" : \"%d\",\n", mc->sequenceableNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"rawdataNumber_\" : \"%d\",\n", mc->rawdataNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"interfaceNumber_\" : \"%d\",\n", mc->interfaceNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"typeNumber_\" : \"%d\",\n", mc->typeNumber_);

    DumpMetaNamespaces(sb, mc, prefix);
    DumpMetaSequenceables(sb, mc, prefix);
    DumpMetaRawDatas(sb, mc, prefix);
    DumpMetaInterfaces(sb, mc, prefix);

    sb.Append(prefix + tab_).AppendFormat("\"stringPoolSize_\" : \"%d\"\n", mc->stringPoolSize_);

    sb.Append(prefix).Append("}\n");

    return sb.ToString();
}

void MetadataDumper::DumpMetaNamespaces(StringBuilder& sb, MetaComponent* mc, const std::string& prefix)
{
    if (mc->namespaceNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"namespaces_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"namespaces_\" : [\n");
        for (int i = 0; i < mc->namespaceNumber_; i++) {
            DumpMetaNamespace(sb, mc->namespaces_[i], prefix + tab_ + tab_);
            if (i != mc->namespaceNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }
}

void MetadataDumper::DumpMetaNamespace(StringBuilder& sb, MetaNamespace* mn, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mn->name_);
    sb.Append(prefix + tab_).AppendFormat("\"sequenceableNumber_\" : \"%d\",\n", mn->sequenceableNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"rawdataNumber_\" : \"%d\",\n", mn->rawdataNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"interfaceNumber_\" : \"%d\",\n", mn->interfaceNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"namespaceNumber_\" : \"%d\",\n", mn->namespaceNumber_);

    GetDumpMetaNamespace(sb, mn, prefix);

    if (mn->interfaceNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"interfaceIndexes_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"interfaceIndexes_\" : [\n");
        for (int i = 0; i < mn->interfaceNumber_; i++) {
            MetaInterface* mi = metaComponent_->interfaces_[mn->interfaceIndexes_[i]];
            sb.Append(prefix + tab_ + tab_).AppendFormat("{ \"name\" : \"%s\" }", mi->name_);
            if (i != mn->interfaceNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }

    if (mn->namespaceNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"namespaces_\" : []\n");
    } else {
        sb.Append(prefix + tab_).Append("\"namespaces_\" : [\n");
        for (int i = 0; i < mn->namespaceNumber_; i++) {
            MetaNamespace* innermn = mn->namespaces_[i];
            DumpMetaNamespace(sb, innermn, prefix + tab_ + tab_);
            if (i != mn->namespaceNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("]\n");
    }

    sb.Append(prefix).Append("}");
}

void MetadataDumper::GetDumpMetaNamespace(StringBuilder& sb, MetaNamespace* mn, const std::string& prefix)
{
    if (mn->sequenceableNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"sequenceableIndexes_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"sequenceableIndexes_\" : [\n");
        for (int i = 0; i < mn->sequenceableNumber_; i++) {
            MetaSequenceable* mp = metaComponent_->sequenceables_[i];
            sb.Append(prefix + tab_ + tab_).AppendFormat("{ \"name\" : \"%s\" }", mp->name_);
            if (i != mn->sequenceableNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }

    if (mn->rawdataNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"rawdataIndexes_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"rawdataIndexes_\" : [\n");
        for (int i = 0; i < mn->rawdataNumber_; i++) {
            MetaRawData* mp = metaComponent_->rawdatas_[i];
            sb.Append(prefix + tab_ + tab_).AppendFormat("{ \"name\" : \"%s\" }", mp->name_);
            if (i != mn->rawdataNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }
}

void MetadataDumper::DumpMetaSequenceables(StringBuilder& sb, MetaComponent* mc, const std::string& prefix)
{
    if (mc->sequenceableNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"sequenceables_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"sequenceables_\" : [\n");
        for (int i = 0; i < mc->sequenceableNumber_; i++) {
            DumpMetaSequenceable(sb, mc->sequenceables_[i], prefix + tab_ + tab_);
            if (i != mc->sequenceableNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }
}

void MetadataDumper::DumpMetaSequenceable(StringBuilder& sb, MetaSequenceable* mp, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mp->name_);
    sb.Append(prefix + tab_).AppendFormat("\"namespace_\" : \"%s\"\n", mp->namespace_);
    sb.Append(prefix).Append("}");
}

void MetadataDumper::DumpMetaRawDatas(StringBuilder& sb, MetaComponent* mc, const std::string& prefix)
{
    if (mc->rawdataNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"rawdatas_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"rawdatas_\" : [\n");
        for (int i = 0; i < mc->rawdataNumber_; i++) {
            DumpMetaRawData(sb, mc->rawdatas_[i], prefix + tab_ + tab_);
            if (i != mc->rawdataNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }
}

void MetadataDumper::DumpMetaRawData(StringBuilder& sb, MetaRawData* mp, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mp->name_);
    sb.Append(prefix + tab_).AppendFormat("\"namespace_\" : \"%s\"\n", mp->namespace_);
    sb.Append(prefix).Append("}");
}

void MetadataDumper::DumpMetaInterfaces(StringBuilder& sb, MetaComponent* mc, const std::string& prefix)
{
    if (mc->interfaceNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"interfaces_\" : [],\n");
    } else {
        sb.Append(prefix + tab_).Append("\"interfaces_\" : [\n");
        for (int i = 0; i < mc->interfaceNumber_; i++) {
            DumpMetaInterface(sb, mc->interfaces_[i], prefix + tab_ + tab_);
            if (i != mc->interfaceNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("],\n");
    }
}

void MetadataDumper::DumpMetaInterface(StringBuilder& sb, MetaInterface* mi, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mi->name_);
    sb.Append(prefix + tab_).AppendFormat("\"namespace_\" : \"%s\",\n", mi->namespace_);
    sb.Append(prefix + tab_).AppendFormat("\"properties_\" : \"%s\",\n",
        (mi->properties_ & INTERFACE_PROPERTY_ONEWAY) != 0 ? "oneway" : "");
    sb.Append(prefix + tab_).AppendFormat("\"methodNumber_\" : \"%d\",\n", mi->methodNumber_);
    sb.Append(prefix + tab_).AppendFormat("\"external_\" : \"%d\",\n", mi->external_);

    if (mi->methodNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"methods_\" : []\n");
    } else {
        sb.Append(prefix + tab_).Append("\"methods_\" : [\n");
        for (int i = 0; i < mi->methodNumber_; i++) {
            DumpMetaMethod(sb, mi->methods_[i], prefix + tab_ + tab_);
            if (i != mi->methodNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("]\n");
    }

    sb.Append(prefix).Append("}");
}

void MetadataDumper::DumpMetaMethod(StringBuilder& sb, MetaMethod* mm, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mm->name_);
    sb.Append(prefix + tab_).AppendFormat("\"signature_\" : \"%s\",\n", mm->signature_);
    sb.Append(prefix + tab_).AppendFormat("\"properties_\" : \"%s\",\n",
        (mm->properties_ & METHOD_PROPERTY_ONEWAY) != 0 ? "oneway" : "");
    MetaType* type = metaComponent_->types_[mm->returnTypeIndex_];
    sb.Append(prefix + tab_).AppendFormat("\"returnType_\" : \"%s\",\n", DumpMetaType(type).c_str());
    sb.Append(prefix + tab_).AppendFormat("\"parameterNumber_\" : \"%d\",\n", mm->parameterNumber_);

    if (mm->parameterNumber_ == 0) {
        sb.Append(prefix + tab_).Append("\"parameters_\" : []\n");
    } else {
        sb.Append(prefix + tab_).Append("\"parameters_\" : [\n");
        for (int i = 0; i < mm->parameterNumber_; i++) {
            DumpMetaParameter(sb, mm->parameters_[i], prefix + tab_ + tab_);
            if (i != mm->parameterNumber_ - 1) {
                sb.Append(",\n");
            }
        }
        sb.Append("\n" + prefix + tab_).Append("]\n");
    }

    sb.Append(prefix).Append("}");
}

void MetadataDumper::DumpMetaParameter(StringBuilder& sb, MetaParameter* mp, const std::string& prefix)
{
    sb.Append(prefix).Append("{\n");
    sb.Append(prefix + tab_).AppendFormat("\"name_\" : \"%s\",\n", mp->name_);
    sb.Append(prefix + tab_).Append("\"attributes_\" : \"");
    bool addComma = false;
    if ((mp->attributes_ & ATTR_IN) == ATTR_IN) {
        sb.Append("in");
        addComma = true;
    }
    if ((mp->attributes_ & ATTR_OUT) == ATTR_OUT) {
        sb.Append(addComma ? ", out" : "out");
    }
    sb.Append("\",\n");
    MetaType* type = metaComponent_->types_[mp->typeIndex_];
    sb.Append(prefix + tab_).AppendFormat("\"type_\" : \"%s\"\n", DumpMetaType(type).c_str());

    sb.Append(prefix).Append("}");
}

std::string MetadataDumper::DumpMetaType(MetaType* mt)
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
            return GetDumpMetaType(mt);
        case MetaTypeKind::List: {
            MetaType* elementMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return "List<" + DumpMetaType(elementMt) + ">";
        }
        case MetaTypeKind::Map: {
            MetaType* keyMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            MetaType* valueMt = metaComponent_->types_[mt->nestedTypeIndexes_[1]];
            return "Map<" + DumpMetaType(keyMt) + ", " + DumpMetaType(valueMt) + ">";
        }
        case MetaTypeKind::Array: {
            MetaType* elementMt = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return DumpMetaType(elementMt) + "[]";
        }
        case MetaTypeKind::Unknown:
        default:
            printf("Unknown %d\n", mt->index_);
            return "unknown";
    }
}

std::string MetadataDumper::GetDumpMetaType(MetaType* mt)
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
