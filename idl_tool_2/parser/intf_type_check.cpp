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

#include "ast/ast_parameter.h"
#include "util/logger.h"
#include "util/string_builder.h"
#include "parser/intf_type_check.h"

namespace OHOS {
namespace Idl {
bool IntfTypeChecker::CheckIntegrity()
{
    if (ast_ == nullptr) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:ast is nullptr.", __func__, __LINE__).c_str());
        return false;
        return false;
    }

    if (ast_->GetName().empty()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:ast's name is empty.", __func__, __LINE__).c_str());
        return false;
    }

    InterfaceType interfaceType = Options::GetInstance().GetInterfaceType();
    if (interfaceType == InterfaceType::SA) {
        return CheckIntfSaAst();
    } else if (interfaceType == InterfaceType::HDI) {
        return CheckIntfHdiAst();
    } else if (interfaceType == InterfaceType::SM) {
        return CheckIntfSmAst();
    }

    Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf type is invalid.", __func__, __LINE__).c_str());
    return false;
}

bool IntfTypeChecker::CheckIntfSaAst()
{
    if (!ast_->GetPackageName().empty()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: package not support", __func__, __LINE__).c_str());
        return false;
    }

    const auto &importMap = ast_->GetImports();
    if (std::any_of(importMap.begin(), importMap.end(), [] (const std::pair<std::string, AutoPtr<AST>> &importPair) {
        return importPair.second->GetASTFileType() != ASTFileType::AST_SEQUENCEABLE;
    })) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: import not support", __func__, __LINE__).c_str());
        return false;
    }

    bool definedIntf = false;
    for (size_t i = 0; i < ast_->GetInterfaceDefNumber(); i++) {
        if (!ast_->GetInterfaceDef(i)->IsExternal()) {
            definedIntf = true;
            break;
        }
    }

    if (!definedIntf) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: interface is not defined.", __func__,
            __LINE__).c_str());
        return false;
    }

    ASTAttr::Attribute attr = ast_->GetInterfaceDef()->GetAttribute()->GetValue();
    if ((attr != ASTAttr::NONE) && (attr != ASTAttr::ONEWAY)) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: interface attr only support [oneway]", __func__,
            __LINE__).c_str());
        return false;
    }

    if (!CheckIntfSaAstTypes() || !CheckIntfSaAstMethods()) {
        return false;
    }

    return true;
}

bool IntfTypeChecker::CheckIntfSaAstTypes()
{
    for (const auto &pair : ast_->GetTypes()) {
        AutoPtr<ASTType> type = pair.second;
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_FILEDESCRIPTOR:
            case TypeKind::TYPE_ASHMEM:
            case TypeKind::TYPE_NATIVE_BUFFER:
            case TypeKind::TYPE_POINTER:
            case TypeKind::TYPE_SMQ:
            case TypeKind::TYPE_ENUM:
            case TypeKind::TYPE_STRUCT:
            case TypeKind::TYPE_UNION:
            case TypeKind::TYPE_UCHAR:
            case TypeKind::TYPE_USHORT:
            case TypeKind::TYPE_UINT:
            case TypeKind::TYPE_ULONG:
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: type '%s' not support", __func__, __LINE__,
                pair.first.c_str()).c_str());
                return false;
            default:
                break;
        }
    }
    return true;
}

bool IntfTypeChecker::CheckIntfSaAstMethods()
{
    AutoPtr<ASTInterfaceType> interfaceType = ast_->GetInterfaceDef();
    bool onewayInterface = (interfaceType->GetAttribute()->GetValue() == ASTAttr::ONEWAY);

    for (size_t i = 0; i < interfaceType->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interfaceType->GetMethod(i);
        if (((method->GetAttribute()->GetValue()) & (~(ASTAttr::ONEWAY | ASTAttr::CACHEABLE))) != 0) {
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: method attr support oneway or cacheable",
                __func__, __LINE__).c_str());
            return false;
        }
        if (method->GetAttribute()->HasValue(ASTAttr::CACHEABLE) &&
            !method->GetAttribute()->HasValue(ASTAttr::ONEWAY)) {
            auto ret = method->SetCacheableTime();
            if (ret) {
                ast_->SetHasCacheableProxyMethods(true);
            } else {
                Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sa: method attr cacheable time invalid",
                    __func__, __LINE__).c_str());
                return false;
            }
        }
        if ((onewayInterface || method->GetAttribute()->GetValue() == ASTAttr::ONEWAY) &&
            !method->GetReturnType()->IsVoidType()) {
            Logger::E(TAG, StringHelper::Format(
                "[%s:%d] error:intf sa: method return type must be void in [oneway] interface or method", __func__,
                __LINE__).c_str());
            return false;
        }
    }
    return true;
}

bool IntfTypeChecker::CheckIntfHdiAst()
{
    if (ast_->GetPackageName().empty()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi ast's package name is empty.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (!CheckIntfHdiAstFileType() || !CheckIntfHdiAstTypes()) {
        return false;
    }

    AutoPtr<ASTInterfaceType> interfaceType = ast_->GetInterfaceDef();
    if (interfaceType == nullptr) {
        return true;
    }

    if (interfaceType->IsExternal()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: interface not support external", __func__,
            __LINE__).c_str());
        return false;
    }

    for (size_t i = 0; i < interfaceType->GetMethodNumber(); i++) {
        AutoPtr<ASTMethod> method = interfaceType->GetMethod(i);
        for (size_t j = 0; j < method->GetParameterNumber(); j++) {
            if (!CheckIntfHdiAstParam(method->GetParameter(j), i, j)) {
                return false;
            }
        }
    }

    return true;
}

bool IntfTypeChecker::CheckIntfHdiAstFileType()
{
    switch (ast_->GetASTFileType()) {
        case ASTFileType::AST_IFACE:
            return CheckInterfaceAst();
        case ASTFileType::AST_ICALLBACK:
            return CheckCallbackAst();
        case ASTFileType::AST_SEQUENCEABLE:
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:it's impossible that ast is sequenceable.", __func__,
                __LINE__).c_str());
            return false;
        case ASTFileType::AST_TYPES:
            if (ast_->GetInterfaceDef() != nullptr) {
                Logger::E(TAG, StringHelper::Format("[%s:%d] error:custom ast cannot has interface.", __func__,
                    __LINE__).c_str());
                return false;
            }
            return true;
        default:
            return true;
    }
}

bool IntfTypeChecker::CheckIntfHdiAstTypes()
{
    for (const auto &pair : ast_->GetTypes()) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_CHAR) {
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: type '%s' not support", __func__, __LINE__,
                pair.first.c_str()).c_str());
            return false;
        }
    }
    return true;
}

bool IntfTypeChecker::CheckIntfHdiAstParam(AutoPtr<ASTParameter> param, size_t methodIdx, size_t paramIdx)
{
    ASTParamAttr::ParamAttr paramAttr = param->GetAttribute();
    if (paramAttr == ASTParamAttr::PARAM_INOUT) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: method[%d] param[%d] attr not support "
            "[inout] or [in, out]", __func__, __LINE__, methodIdx, paramIdx).c_str());
        return false;
    }

    AutoPtr<ASTType> paramType = param->GetType();
    if (paramType != nullptr && paramType->IsInterfaceType()) {
        AutoPtr<ASTInterfaceType> ifaceType = dynamic_cast<ASTInterfaceType *>(paramType.Get());
        if (ifaceType->IsCallback() && paramAttr != ASTParamAttr::PARAM_IN) {
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: '%s' "
                "param of callback interface type must be 'in' attr", __func__, __LINE__,
                param->GetName().c_str()).c_str());
            return false;
        } else if (!ifaceType->IsCallback() && paramAttr != ASTParamAttr::PARAM_OUT) {
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: '%s' param of interface "
                "type must be 'out' attr", __func__, __LINE__, param->GetName().c_str()).c_str());
            return false;
        }
    }

    return true;
}
bool IntfTypeChecker::CheckInterfaceAst()
{
    AutoPtr<ASTInterfaceType> interface = ast_->GetInterfaceDef();
    if (interface == nullptr) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: ast's interface is empty.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (ast_->GetTypeDefinitionNumber() > 0) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: interface ast cannot has custom types.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (interface->GetMethodNumber() == 0) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf hdi: interface ast has no method.", __func__,
            __LINE__).c_str());
        return false;
    }
    return true;
}

bool IntfTypeChecker::CheckCallbackAst()
{
    AutoPtr<ASTInterfaceType> interface = ast_->GetInterfaceDef();
    if (interface == nullptr) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:ast's interface is empty.", __func__, __LINE__).c_str());
        return false;
    }

    if (!interface->IsCallback()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:ast is callback, but ast's interface is not callback.",
            __func__, __LINE__).c_str());
        return false;
    }
    return true;
}

bool IntfTypeChecker::CheckIntfSmAst()
{
    if (ast_->GetPackageName().empty()) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm ast's package name is empty.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (!CheckIntfSmAstFileType()) {
        return false;
    }

    for (const auto &pair : ast_->GetTypes()) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_CHAR) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm: type '%s' not support", __func__, __LINE__,
            pair.first.c_str()).c_str());
            return false;
        }
    }

    return true;
}

bool IntfTypeChecker::CheckIntfSmAstFileType()
{
    switch (ast_->GetASTFileType()) {
        case ASTFileType::AST_IFACE:
            return CheckSmInterfaceAst();
        case ASTFileType::AST_ICALLBACK:
            return CheckCallbackAst();
        case ASTFileType::AST_SEQUENCEABLE:
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:it's impossible that ast is sequenceable.",
                __func__, __LINE__).c_str());
            return false;
        case ASTFileType::AST_TYPES:
            if (ast_->GetInterfaceDef() != nullptr) {
                Logger::E(TAG, StringHelper::Format("[%s:%d] error:custom ast cannot has interface.",
                    __func__, __LINE__).c_str());
                return false;
            }
            return true;
        default:
            return true;
    }
}

bool IntfTypeChecker::CheckIntfSmAstTypes()
{
    for (const auto &pair : ast_->GetTypes()) {
        AutoPtr<ASTType> type = pair.second;
        if (type->GetTypeKind() == TypeKind::TYPE_CHAR) {
            Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm: type '%s' not support", __func__, __LINE__,
                pair.first.c_str()).c_str());
            return false;
        }
    }
    return true;
}

bool IntfTypeChecker::CheckSmInterfaceAst()
{
    size_t index = 0;
    for (size_t i = 0; i < ast_->GetInterfaceDefNumber(); i++) {
        if (!ast_->GetInterfaceDef(i)->IsExternal()) {
            index = i;
            break;
        }
    }
    AutoPtr<ASTInterfaceType> interface = ast_->GetInterfaceDef(index);
    if (interface == nullptr) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm: ast's interface is empty.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (ast_->GetTypeDefinitionNumber() > 0) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm: interface ast cannot has custom types.", __func__,
            __LINE__).c_str());
        return false;
    }

    if (interface->GetMethodNumber() == 0) {
        Logger::E(TAG, StringHelper::Format("[%s:%d] error:intf sm: interface ast has no method.", __func__,
            __LINE__).c_str());
        return false;
    }
    return true;
}
} // namespace Idl
} // namespace OHOS
