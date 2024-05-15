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

#include "hdi_java_code_emitter.h"

namespace OHOS {
namespace Idl {
void HDIJavaCodeEmitter::EmitPackage(StringBuilder &sb)
{
    sb.AppendFormat("package %s;\n", ast_->GetPackageName().c_str());
}

void HDIJavaCodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    auto methods = interface_->GetMethodsBySystem(Options::GetInstance().GetSystemLevel());
    for (size_t i = 0; i < methods.size(); i++) {
        sb.Append(prefix).AppendFormat("private static final int COMMAND_%s = IRemoteObject.MIN_TRANSACTION_ID + %d;\n",
            ConstantName(methods[i]->GetName()).c_str(), i);
    }
}

std::string HDIJavaCodeEmitter::MethodName(const std::string &name) const
{
    if (name.empty() || islower(name[0])) {
        return name;
    }
    return StringHelper::Format("%c%s", tolower(name[0]), name.substr(1).c_str());
}

std::string HDIJavaCodeEmitter::SpecificationParam(StringBuilder &paramSb, const std::string &prefix) const
{
    size_t maxLineLen = 120;
    size_t replaceLen = 2;
    std::string paramStr = paramSb.ToString();
    size_t preIndex = 0;
    size_t curIndex = 0;

    std::string insertStr = StringHelper::Format("\n%s", prefix.c_str());
    for (; curIndex < paramStr.size(); curIndex++) {
        if (curIndex == maxLineLen && preIndex > 0) {
            StringHelper::Replace(paramStr, preIndex, replaceLen, ",");
            paramStr.insert(preIndex + 1, insertStr);
        } else {
            if (paramStr[curIndex] == ',') {
                preIndex = curIndex;
            }
        }
    }
    return paramStr;
}

void HDIJavaCodeEmitter::EmitInterfaceMethodParameter(
    const AutoPtr<ASTParameter> &param, StringBuilder &sb, const std::string &prefix) const
{
    std::string name = param->GetName();
    AutoPtr<ASTType> type = param->GetType();
    AutoPtr<HdiTypeEmitter> typeEmitter = GetTypeEmitter(type);
    switch (type->GetTypeKind()) {
        case TypeKind::TYPE_BOOLEAN:
        case TypeKind::TYPE_BYTE:
        case TypeKind::TYPE_SHORT:
        case TypeKind::TYPE_INT:
        case TypeKind::TYPE_LONG:
        case TypeKind::TYPE_UCHAR:
        case TypeKind::TYPE_USHORT:
        case TypeKind::TYPE_UINT:
        case TypeKind::TYPE_ULONG:
        case TypeKind::TYPE_FLOAT:
        case TypeKind::TYPE_DOUBLE:
        case TypeKind::TYPE_ENUM:
        case TypeKind::TYPE_FILEDESCRIPTOR:
        case TypeKind::TYPE_STRING:
        case TypeKind::TYPE_SEQUENCEABLE:
        case TypeKind::TYPE_INTERFACE:
        case TypeKind::TYPE_STRUCT:
        case TypeKind::TYPE_UNION:
        case TypeKind::TYPE_POINTER:
        case TypeKind::TYPE_ARRAY:
        case TypeKind::TYPE_LIST:
        case TypeKind::TYPE_MAP: {
            sb.Append(prefix).AppendFormat("%s %s", typeEmitter->EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str());
            break;
        }
        default:
            sb.Append(prefix).AppendFormat("unknow type %s", name.c_str());
            break;
    }
}
} // namespace Idl
} // namespace OHOS