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

#include "sa_fd_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind SaFdTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_FILEDESCRIPTOR;
}

std::string SaFdTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
        case TypeMode::PARAM_IN:
        case TypeMode::LOCAL_VAR:
            return "int";
        case TypeMode::PARAM_INOUT:
        case TypeMode::PARAM_OUT:
            return "int&";
        default:
            return "unknown type";
    }
}

void SaFdTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("if (!%sWriteFileDescriptor(%s)) {\n", parcelName.c_str(), name.c_str());
    if (logOn_) {
        sb.Append(prefix).Append(TAB).AppendFormat("HiLog::Error(LABEL, \"Write [%s] failed!\");\n", name.c_str());
    }
    if (!isProxy) {
        sb.Append(prefix).Append(TAB).AppendFormat("if (%s >= 0) {\n", name.c_str());
        sb.Append(prefix).Append(TAB).Append(TAB).AppendFormat("close(%s);\n", name.c_str());
        sb.Append(prefix).Append(TAB).Append("}\n");
    }
    sb.Append(prefix).Append(TAB).Append("return ERR_INVALID_DATA;\n");
    sb.Append(prefix).Append("}\n");
}

void SaFdTypeEmitter::EmitCppReadVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
    const std::string &prefix, bool emitType) const
{
    if (emitType) {
        sb.Append(prefix).AppendFormat("%s %s = %sReadFileDescriptor();\n",
            EmitCppType(TypeMode::LOCAL_VAR).c_str(), name.c_str(), parcelName.c_str());
    } else {
        sb.Append(prefix).AppendFormat("%s = %sReadFileDescriptor();\n", name.c_str(), parcelName.c_str());
    }
}
} // namespace Idl
} // namespace OHOS