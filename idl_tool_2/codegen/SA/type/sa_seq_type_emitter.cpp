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

#include "sa_seq_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaSeqTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_SEQUENCEABLE;
}

std::string SaSeqTypeEmitter::EmitCppType(TypeMode mode) const
{
    if (typeName_ == "IRemoteObject") {
        switch (mode) {
            case TypeMode::PARAM_IN:
                return StringHelper::Format("const sptr<%s>&", typeName_.c_str());
            case TypeMode::PARAM_INOUT:
                return StringHelper::Format("sptr<%s>&", typeName_.c_str());
            case TypeMode::PARAM_OUT:
                return StringHelper::Format("sptr<%s>&", typeName_.c_str());
            case TypeMode::NO_MODE:
            case TypeMode::LOCAL_VAR:
                return StringHelper::Format("sptr<%s>", typeName_.c_str());
            default:
                return "unknown type";
        }
    } else {
        switch (mode) {
            case TypeMode::PARAM_IN:
                return StringHelper::Format("const %s&", typeName_.c_str());
            case TypeMode::PARAM_INOUT:
                return StringHelper::Format("%s*", typeName_.c_str());
            case TypeMode::NO_MODE:
            case TypeMode::PARAM_OUT:
                return StringHelper::Format("%s&", typeName_.c_str());
            case TypeMode::LOCAL_VAR:
                return StringHelper::Format("%s", typeName_.c_str());
            default:
                return "unknown type";
        }
    }
}

std::string SaSeqTypeEmitter::EmitRustType(bool pt, TypeMode mode) const
{
    return typeName_;
}

std::string SaSeqTypeEmitter::EmitTsType(TypeMode mode) const
{
    return typeName_;
}

void SaSeqTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    if (typeName_ == "IRemoteObject") {
        sb.Append(prefix).AppendFormat("if (!%sWriteRemoteObject(%s)) {\n", parcelName.c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("if (!%sWriteParcelable(&%s)) {\n", parcelName.c_str(), name.c_str());
    }
    if (logOn_) {
        sb.Append(prefix).Append(TAB).AppendFormat("HiLog::Error(LABEL, \"Write [%s] failed!\");\n", name.c_str());
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaSeqTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (typeName_ == "IRemoteObject") {
        if (emitType) {
            sb.Append(prefix).AppendFormat("sptr<%s> %s = %sReadRemoteObject();\n",
                typeName_.c_str(), name.c_str(), parcelName.c_str(), typeName_.c_str());
            sb.Append(prefix).AppendFormat("if (!%s) {\n", name.c_str());
            if (logOn_) {
                sb.Append(prefix).Append(TAB).AppendFormat(
                    "HiLog::Error(LABEL, \"Read [%s] failed!\");\n", typeName_.c_str());
            }
            sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
            sb.Append(prefix).Append("}\n\n");
        } else {
            sb.Append(prefix).AppendFormat("%s = %sReadRemoteObject();\n\n",
                name.c_str(), parcelName.c_str());
        }
    } else {
        if (emitType) {
            sb.Append(prefix).AppendFormat("std::unique_ptr<%s> %s(%sReadParcelable<%s>());\n",
                typeName_.c_str(), name.c_str(), parcelName.c_str(), typeName_.c_str());
            sb.Append(prefix).AppendFormat("if (!%s) {\n", name.c_str());
            if (logOn_) {
                sb.Append(prefix).Append(TAB).AppendFormat(
                    "HiLog::Error(LABEL, \"Read [%s] failed!\");\n", typeName_.c_str());
            }
            sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
            sb.Append(prefix).Append("}\n\n");
        } else {
            sb.Append(prefix).AppendFormat("std::unique_ptr<%s> %sInfo(%sReadParcelable<%s>());\n",
                typeName_.c_str(), name.c_str(), parcelName.c_str(), typeName_.c_str());
            sb.Append(prefix).AppendFormat("if (%sInfo != nullptr) {\n", name.c_str());
            sb.Append(prefix).Append(TAB).AppendFormat("%s = *%sInfo;\n", name.c_str(), name.c_str());
            sb.Append(prefix).Append("}\n\n");
        }
    }
}

void SaSeqTypeEmitter::EmitTsWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    if (typeName_ == "IRemoteObject") {
        sb.Append(prefix).AppendFormat("%s.writeRemoteObject(%s);\n", parcelName.c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s.writeParcelable(%s);\n", parcelName.c_str(), name.c_str());
    }
}

void SaSeqTypeEmitter::EmitTsReadVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    if (typeName_ == "IRemoteObject") {
        if (mode == TypeMode::PARAM_IN) {
            sb.Append(prefix).AppendFormat(
                "IRemoteObject %s = %s.readRemoteObject();\n", name.c_str(), parcelName.c_str());
        } else {
            sb.Append(prefix).AppendFormat("%s = %s.readRemoteObject();\n", name.c_str(), parcelName.c_str());
        }
        return;
    }

    if (mode == TypeMode::PARAM_IN) {
        sb.Append(prefix).AppendFormat("let %s = new %s();\n", name.c_str(), typeName_.c_str());
        sb.Append(prefix).AppendFormat("%s.readParcelable(%s);\n", parcelName.c_str(), name.c_str());
    } else {
        sb.Append(prefix).AppendFormat("let %s = new %s();\n", name.c_str(), typeName_.c_str());
        sb.Append(prefix).AppendFormat("%s.readSequenceable(%s);\n", parcelName.c_str(), name.c_str());
    }
}
} // namespace Idl
} // namespace OHOS