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

#include "sa_array_type_emitter.h"

#include <sstream>

namespace OHOS {
namespace Idl {
TypeKind SaArrayTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_ARRAY;
}

std::string SaArrayTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("std::vector<%s>", elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("const std::vector<%s>&",
                elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        case TypeMode::PARAM_OUT:
        case TypeMode::PARAM_INOUT:
            return StringHelper::Format("std::vector<%s>&", elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("std::vector<%s>", elementEmitter_->EmitCppType(TypeMode::LOCAL_VAR).c_str());
        default:
            return "unknow type";
    }
}

std::string SaArrayTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return StringHelper::Format((pt ? "[%s]" : "Vec<%s>"), elementEmitter_->EmitRustType(false, mode).c_str());
}

std::string SaArrayTypeEmitter::EmitTsType(TypeMode mode) const
{
    if (elementEmitter_->EmitTsType(mode) == "unknown type") {
        return "unknown type";
    }
    return StringHelper::Format("%s[]", elementEmitter_->EmitTsType(mode).c_str());
}

void SaArrayTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(
            "HiLog::Error(LABEL, \"The vector/array size exceeds the security limit!\");\n");
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("%sWriteInt32(%s.size());\n", parcelName.c_str(), name.c_str());

    circleCount_++;
    std::stringstream circleCountStr;
    circleCountStr << circleCount_;
    std::string itStr = "it" + circleCountStr.str();
    sb.Append(prefix).AppendFormat("for (auto %s = %s.begin(); %s != %s.end(); ++%s) {\n", itStr.c_str(),
        name.c_str(), itStr.c_str(), name.c_str(), itStr.c_str());
    elementEmitter_->EmitCppWriteVar(parcelName, "(*" + itStr +")", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void SaArrayTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("int32_t %sSize = %sReadInt32();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%sSize > static_cast<int32_t>(VECTOR_MAX_SIZE)) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix + TAB).Append("HiLog::Error(LABEL, \"The vector/array size exceeds the security limit!\");\n");
    }
    sb.Append(prefix + TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");

    circleCount_++;
    std::stringstream circleCountStr;
    circleCountStr << circleCount_;
    std::string iStr = "i" + circleCountStr.str();
    std::string valueStr = "value" + circleCountStr.str();
    sb.Append(prefix).AppendFormat("for (int32_t %s = 0; %s < %sSize; ++%s) {\n",
        iStr.c_str(), iStr.c_str(), name.c_str(), iStr.c_str());
    elementEmitter_->EmitCppReadVar(parcelName, valueStr.c_str(), sb, prefix + TAB);
    if (elementEmitter_->GetTypeKind() == TypeKind::TYPE_SEQUENCEABLE) {
        sb.Append(prefix + TAB).AppendFormat("%s.push_back(*%s);\n", name.c_str(), valueStr.c_str());
    } else {
        sb.Append(prefix + TAB).AppendFormat("%s.push_back(%s);\n", name.c_str(), valueStr.c_str());
    }
    sb.Append(prefix).Append("}\n");
}

void SaArrayTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    switch (elementEmitter_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_CHAR:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
            sb.Append(prefix).AppendFormat("%s.write%sArray(%s);\n", parcelName.c_str(),
                StringHelper::FirstToUpper(elementEmitter_->GetTypeName()).c_str(), name.c_str());
            break;
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("%s.writeStringArray(%s);\n", parcelName.c_str(), name.c_str());
            break;
        case TypeKind::TYPE_SEQUENCEABLE: {
            sb.Append(prefix).AppendFormat(
                "let %sArray:Array<%s> = %s;\n", name.c_str(), elementEmitter_->GetTypeName().c_str(), name.c_str());
            sb.Append(prefix).AppendFormat("%s.writeInt(%sArray.length);\n", parcelName.c_str(), name.c_str());
            sb.Append(prefix).AppendFormat("for (let index = 0; index < %sArray.length; index++) {\n", name.c_str());
            sb.Append(prefix).Append(TAB).AppendFormat(
                "%s.writeSequenceable(%s[index]);\n", parcelName.c_str(), name.c_str());
            sb.Append(prefix).AppendFormat("}\n");
        }
            break;
        default:
            break;
    }
}

void SaArrayTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    switch (elementEmitter_->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_CHAR:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
            sb.Append(prefix).AppendFormat("let %s = %s.read%sArray();\n", name.c_str(), parcelName.c_str(),
                StringHelper::FirstToUpper(elementEmitter_->GetTypeName()).c_str());
            break;
        case TypeKind::TYPE_STRING:
            sb.Append(prefix).AppendFormat("let %s = %s.readStringArray();\n", name.c_str(), parcelName.c_str());
            break;
        case TypeKind::TYPE_SEQUENCEABLE:
            sb.Append(prefix).AppendFormat("let %sSize = %s.readInt();\n", name.c_str(), parcelName.c_str());
            sb.Append(prefix).AppendFormat("let %s:Array<%s> = [];\n", name.c_str(),
                elementEmitter_->GetTypeName().c_str());
            sb.Append(prefix).AppendFormat("for (let index = 0; index < %sSize; index++) {\n", name.c_str());
            sb.Append(prefix).Append(TAB).AppendFormat("let %sValue = new %s();\n", name.c_str(),
                elementEmitter_->GetTypeName().c_str());
            sb.Append(prefix).Append(TAB).AppendFormat(
                "%s.readSequenceable(%sValue);\n", parcelName.c_str(), name.c_str());
            sb.Append(prefix).Append(TAB).AppendFormat("%s.push(%sValue);\n", name.c_str(), name.c_str());
            sb.Append(prefix).AppendFormat("}\n");
            break;
        default:
            break;
    }
}

TypeKind SaListTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_LIST;
}

std::string SaListTypeEmitter::EmitTsType(TypeMode mode) const
{
    return "unknown type";
}

void SaListTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (%s.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {\n", name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).Append(
            "HiLog::Error(LABEL, \"The list size exceeds the security limit!\");\n");
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append(prefix).AppendFormat("%sWriteInt32(%s.size());\n", parcelName.c_str(), name.c_str());

    circleCount_++;
    std::stringstream circleCountStr;
    circleCountStr << circleCount_;
    std::string itStr = "it" + circleCountStr.str();
    sb.Append(prefix).AppendFormat("for (auto %s = %s.begin(); %s != %s.end(); ++%s) {\n", itStr.c_str(),
        name.c_str(), itStr.c_str(), name.c_str(), itStr.c_str());
    elementEmitter_->EmitCppWriteVar(parcelName, "(*" + itStr +")", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}
} // namespace Idl
} // namespace OHOS