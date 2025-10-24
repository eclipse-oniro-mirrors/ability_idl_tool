/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "util/logger.h"
#include "../../../codegen/SA/ts/sa_ts_code_emitter.h"

namespace OHOS {
namespace Idl {
bool SATsCodeEmitter::CheckInterfaceType() const
{
    if (interface_ == nullptr) {
        return false;
    }
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        if (method == nullptr) {
            return false;
        }
        AutoPtr<ASTType> type = method->GetReturnType();
        AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(type);
        if ((type->GetTypeKind() != TypeKind::TYPE_VOID) &&
            (typeEmitter != nullptr && typeEmitter->EmitTsType() == "unknown type")) {
            Logger::E("SATsCodeEmitter", "unsupported type in .idl file for method '%s'", method->GetName().c_str());
            return false;
        }
        int paramNumber = static_cast<int>(method->GetParameterNumber());
        for (int j = 0; j < paramNumber; j++) {
            AutoPtr<ASTParameter> param = method->GetParameter(j);
            type = param->GetType();
            typeEmitter = GetTypeEmitter(type);
            if ((type->GetTypeKind() != TypeKind::TYPE_VOID) &&
                (typeEmitter != nullptr && typeEmitter->EmitTsType() == "unknown type")) {
                Logger::E("SATsCodeEmitter", "unsupported type in .idl file for method '%s'",
                    method->GetName().c_str());
                return false;
            }
        }
    }
    return true;
}

std::string SATsCodeEmitter::FileName(const std::string &name) const
{
    if (name.empty()) {
        return name;
    }

    StringBuilder sb;
    for (size_t i = 0; i < name.size(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            if (i > 0) {
                sb.Append('_');
            }
            sb.Append(tolower(c));
        } else {
            sb.Append(c);
        }
    }

    return StringHelper::Replace(sb.ToString(), '.', '/');
}

std::string SATsCodeEmitter::MethodName(const std::string &name) const
{
    if (name.empty() || islower(name[0])) {
        return name;
    }
    return StringHelper::Format("%c%s", tolower(name[0]), name.substr(1).c_str());
}

const std::string SATsCodeEmitter::SuffixAdded(const std::string &originName) const
{
    std::string varSuffix("Var");
    return originName + varSuffix;
}

void SATsCodeEmitter::EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix)
{
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.Append(prefix).AppendFormat("static readonly COMMAND_%s = %d;\n",
            ConstantName(method->GetName()).c_str(), i + 1);
    }
}

void SATsCodeEmitter::EmitInterfaceImports(StringBuilder &sb) const
{
    int methodNumber = static_cast<int>(interface_->GetMethodNumber());
    for (int i = 0; i < methodNumber; i++) {
        AutoPtr<ASTMethod> method = interface_->GetMethod(i);
        sb.AppendFormat("import {%sCallback} from \"./%s\";\n", MethodName(method->GetName()).c_str(),
            FileName(interfaceName_).c_str());
    }
    sb.AppendFormat("import %s from \"./%s\";\n", interface_->GetName().c_str(), FileName(interfaceName_).c_str());
    sb.Append("import rpc from \"@ohos.rpc\";\n");
    EmitInterfaceSelfDefinedTypeImports(sb);

    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == false) {
            continue;
        }
        std::string dependInterface = interface->GetName();
        std::string dependStubName = StringHelper::StartWith(dependInterface, 'I') ?
            dependInterface.substr(1) + "Stub" :
            dependInterface + "Stub";
        sb.AppendFormat("import %s from \"./%s\";\n", dependStubName.c_str(), FileName(dependStubName).c_str());
    }
}

void SATsCodeEmitter::EmitInterfaceSelfDefinedTypeImports(StringBuilder &sb) const
{
    int sequenceableNumber = static_cast<int>(ast_->GetSequenceableDefNumber());
    for (int i = 0; i < sequenceableNumber; i++) {
        AutoPtr<ASTSequenceableType> seqType = ast_->GetSequenceableDef(i);
        sb.AppendFormat("import %s from \"./%s\";\n", seqType->GetName().c_str(),
            FileName(seqType->GetName()).c_str());
    }

    for (auto interface : ast_->GetInterfaceDefs()) {
        if (interface->IsExternal() == true) {
            sb.AppendFormat("import %s from \"./%s\";\n", interface->GetName().c_str(),
                FileName(interface->GetName()).c_str());
        }
    }
}

void SATsCodeEmitter::EmitInterfaceMethodHead(AutoPtr<ASTMethod> &method, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s(", MethodName(method->GetName()).c_str());
    int paramNumber = static_cast<int>(method->GetParameterNumber());
    for (int i = 0; i < paramNumber; i++) {
        AutoPtr<ASTParameter> param = method->GetParameter(i);
        if (param->GetAttribute() & ASTParamAttr::PARAM_IN) {
            AutoPtr<SaTypeEmitter> typeEmitter = GetTypeEmitter(param->GetType());
            if (typeEmitter != nullptr) {
                sb.AppendFormat("%s: %s, ", param->GetName().c_str(), typeEmitter->EmitTsType().c_str());
            }
        }
    }
    sb.AppendFormat("callback: %sCallback", MethodName(method->GetName()).c_str());
}
} // namespace Idl
} // namespace OHOS