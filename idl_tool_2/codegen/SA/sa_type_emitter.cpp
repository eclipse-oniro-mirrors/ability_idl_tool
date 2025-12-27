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

#include "sa_type_emitter.h"

namespace OHOS {
namespace Idl {
std::string SaTypeEmitter::domainId_ = "";
std::string SaTypeEmitter::macroHilog_ = "HiLog::Error(LABEL";
bool SaTypeEmitter::logOn_ = false;
bool SaTypeEmitter::usingOn_ = false;
int SaTypeEmitter::circleCount_ = 0;

TypeKind SaTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_UNKNOWN;
}

void SaTypeEmitter::SetTypeName(const std::string &name)
{
    typeName_ = name;
}

std::string SaTypeEmitter::GetTypeName()
{
    return typeName_;
}

std::string SaTypeEmitter::EmitCppType(TypeMode mode) const
{
    return "unknow";
}

std::string SaTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return "()";
}

std::string SaTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "unknown type";
}

std::string SaTypeEmitter::EmitCppTypeDecl() const
{
    StringBuilder sb;
    return sb.ToString();
}

void SaTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    (void)parcelName;
    (void)name;
    (void)sb;
    (void)prefix;
}

void SaTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    (void)parcelName;
    (void)name;
    (void)sb;
    (void)prefix;
    (void)emitType;
}

void SaTypeEmitter::EmitCppPtrWriteVar(const EmitCppPtrVar &emitCppPtrVar, StringBuilder &sb) const
{
    (void)emitCppPtrVar;
    (void)sb;
}

void SaTypeEmitter::EmitCppPtrReadVar(const EmitCppPtrVar &emitCppPtrVar, StringBuilder &sb, bool emitType) const
{
    (void)emitCppPtrVar;
    (void)sb;
    (void)emitType;
}

void SaTypeEmitter::EmitRustReadVar(const std::string &result, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("let %s : %s = %s.read()?;\n", name.c_str(), EmitRustType().c_str(), result.c_str());
}

void SaTypeEmitter::EmitRustWriteVar(const std::string &result, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s.write(&%s)?;\n", result.c_str(), name.c_str());
}

void SaTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    (void)parcelName;
    (void)name;
    (void)sb;
    (void)prefix;
}

void SaTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    (void)parcelName;
    (void)name;
    (void)sb;
    (void)prefix;
}
} // namespace Idl
} // namespace OHOS