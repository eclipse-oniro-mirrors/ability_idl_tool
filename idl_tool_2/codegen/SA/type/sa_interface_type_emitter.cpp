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

#include "sa_interface_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaInterfaceTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_INTERFACE;
}

std::string SaInterfaceTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("sptr<%s>", typeName_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const sptr<%s>&", typeName_.c_str());
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("sptr<%s>&", typeName_.c_str());
        default:
            return "unknown type";
    }
}

std::string SaInterfaceTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return typeName_;
}

std::string SaInterfaceTypeEmitter::EmitTsType(TypeMode mode) const
{
    return typeName_;
}

void SaInterfaceTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
            AppendFormat(", \"%s is nullptr!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat(
        "if (!%sWriteRemoteObject(%s->AsObject())) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).AppendFormat(", \"Write [%s] failed!\");\n",
            name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaInterfaceTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = iface_cast<%s>(%sReadRemoteObject());\n",
            EmitCppType(TypeMode::NO_MODE).c_str(), name.c_str(), typeName_.c_str(), parcelName.c_str());
        sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
        if (logOn_) {
            sb.Append(prefix).Append(TAB).Append(macroHilog_.c_str()).
                AppendFormat(", \"Read [%s] failed!\");\n", name.c_str());
        }
        sb.Append(prefix).Append(TAB).AppendFormat("return ERR_INVALID_DATA;\n");
        sb.Append(prefix).Append("}\n\n");
    } else {
        sb.Append(prefix).AppendFormat("%s = iface_cast<%s>(%sReadRemoteObject());\n",
            name.c_str(), typeName_.c_str(), parcelName.c_str());
    }
}

std::string SaInterfaceTypeEmitter::StubName(const std::string &name) const
{
    return StringHelper::StartWith(name, 'I') ? (name.substr(1) + "Stub") : (name + "Stub");
}

void SaInterfaceTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat(
        "%s.writeRemoteObject(%s as %s);\n", parcelName.c_str(), name.c_str(), StubName(EmitTsType()).c_str());
}

void SaInterfaceTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("let %s = %s.readRemoteObject();\n", name.c_str(), parcelName.c_str());
}
} // namespace Idl
} // namespace OHOS
