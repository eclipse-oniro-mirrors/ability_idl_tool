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

#include "hdi_type_emitter.h"

namespace OHOS {
namespace Idl {
std::string HdiTypeEmitter::dataParcelName_;
std::string HdiTypeEmitter::replyParcelName_;
std::string HdiTypeEmitter::errorCodeName_;
bool HdiTypeEmitter::IsPod() const
{
    return isPod_;
}

void HdiTypeEmitter::SetPod(const bool &isPod)
{
    isPod_ = isPod;
}

TypeKind HdiTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_UNKNOWN;
}

void HdiTypeEmitter::SetTypeName(const std::string &name)
{
    typeName_ = name;
}

std::string HdiTypeEmitter::GetTypeName()
{
    return typeName_;
}

void HdiTypeEmitter::SetName(const std::string &name)
{
    name_ = name;
}

std::string HdiTypeEmitter::GetName()
{
    return name_;
}

std::string HdiTypeEmitter::EmitCType(TypeMode mode) const
{
    return "unknow";
}

std::string HdiTypeEmitter::EmitCppType(TypeMode mode) const
{
    return "unknow";
}

std::string HdiTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return "unknow";
}

std::string HdiTypeEmitter::EmitCTypeDecl() const
{
    StringBuilder sb;
    return sb.ToString();
}

std::string HdiTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    return sb.ToString();
}

std::string HdiTypeEmitter::EmitJavaTypeDecl() const
{
    StringBuilder sb;

    return sb.ToString();
}

void HdiTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCProxyWriteOutVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
}

void HdiTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCStubReadOutVar(const std::string &memFlagName, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix, std::vector<std::string> &freeObjStatements) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitFreeStatements(
    const std::vector<std::string> &freeObjStatements, StringBuilder &sb, const std::string &prefix) const
{
    for (auto it = freeObjStatements.rbegin(); it != freeObjStatements.rend(); it++) {
        sb.Append(prefix).Append(*it);
    }
}

void HdiTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
}

void HdiTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
}

void HdiTypeEmitter::EmitMemoryRecycle(
    const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const
{
    // only std::string, Array, List, struct and union type need recycle memory
    (void)name;
    (void)ownership;
    (void)sb;
    (void)prefix;
}

void HdiTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("//Writing \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("//Reading \"%s\" type of data is not supported\n", name_.c_str());
}

void HdiTypeEmitter::EmitCWriteMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
}

void HdiTypeEmitter::EmitCReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
}

void HdiTypeEmitter::EmitCStubReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
}

void HdiTypeEmitter::EmitCppWriteMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
}

void HdiTypeEmitter::EmitCppReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
}
} // namespace Idl
} // namespace OHOS