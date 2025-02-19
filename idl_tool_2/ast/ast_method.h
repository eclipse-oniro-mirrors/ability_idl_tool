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

#ifndef OHOS_IDL_ASTMETHOD_H
#define OHOS_IDL_ASTMETHOD_H

#include <vector>

#include "ast/ast_node.h"
#include "ast/ast_parameter.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTMethod : public ASTNode {
public:
    inline void SetName(const std::string &name)
    {
        name_ = name;
    }

    inline std::string GetName()
    {
        return name_;
    }

    std::string GetSignature();

    inline void SetAttribute(const AutoPtr<ASTAttr>& attr)
    {
        if (attr_ != nullptr && attr != nullptr) {
            attr_->SetValue(attr->GetValue());
        }
    }

    inline AutoPtr<ASTAttr> GetAttribute() const
    {
        return attr_;
    }

    inline void SetReturnType(AutoPtr<ASTType> type)
    {
        returnType_ = type;
    }

    inline AutoPtr<ASTType> GetReturnType()
    {
        return returnType_;
    }

    inline bool IsOneWay() const
    {
        return attr_->HasValue(ASTAttr::ONEWAY);
    }

    inline bool IsFull() const
    {
        return attr_->HasValue(ASTAttr::FULL);
    }

    inline bool IsLite() const
    {
        return attr_->HasValue(ASTAttr::LITE);
    }

    inline bool IsMini() const
    {
        return attr_->HasValue(ASTAttr::MINI);
    }

    inline bool IsOverload() const
    {
        return isOverload_;
    }

    void CheckOverload(AutoPtr<ASTInterfaceType> interface);

    void AddParameter(const AutoPtr<ASTParameter> &parameter);

    AutoPtr<ASTParameter> GetParameter(size_t index);

    inline size_t GetParameterNumber()
    {
        return parameters_.size();
    }

    inline void SetCmdId(size_t cmdId)
    {
        cmdId_ = cmdId;
    }

    inline size_t GetCmdId() const
    {
        return cmdId_;
    }

    inline std::string GetMethodIdentifier() const
    {
        return isOverload_ ? "_" + std::to_string(cmdId_) : "";
    }

    std::string Dump(const std::string &prefix) override;

    void SetCacheable(const AutoPtr<ASTAttr>& attr)
    {
        if (attr->HasValue(ASTAttr::CACHEABLE)) {
            attr_->SetValue(ASTAttr::CACHEABLE);
            attr_->SetCacheableTimeString(attr->GetCacheableTimeString());
        }
    }

    bool SetCacheableTime()
    {
        return attr_->CacheableStrToInt();
    }

    int32_t GetCacheableTime()
    {
        return attr_->GetCacheableTime();
    }

    bool GetCacheable()
    {
        return attr_->HasValue(ASTAttr::CACHEABLE);
    }

    inline void SetFreezeControlReason(const std::string &reason)
    {
        freezeControlReason = reason;
    }

    inline std::string GetFreezeControlReason()
    {
        return freezeControlReason;
    }

    inline bool IsFreezeControl() const
    {
        return attr_->HasValue(ASTAttr::FREEZECONTROL);
    }

    inline void SetMessageOption(const std::string &messageOption)
    {
        messageOption_ = messageOption;
        if (StringHelper::Split(messageOption, "TF_ASYNC").size() > 1) {
            attr_->SetValue(ASTAttr::ONEWAY);
        }
    }

    inline std::string GetMessageOption()
    {
        return messageOption_;
    }

    inline bool IsMessageOption() const
    {
        return attr_->HasValue(ASTAttr::CUSTOM_MSG_OPTION);
    }

    bool HasIpcCode()
    {
        return attr_->HasValue(ASTAttr::IPCCODE);
    }

    std::string& GetIpcCodeStr()
    {
        return attr_->GetIpcCodeStr();
    }

    void SetIpcCode(const AutoPtr<ASTAttr> &attr)
    {
        if (attr->HasValue(ASTAttr::IPCCODE)) {
            attr_->SetIpcCodeStr(attr->GetIpcCodeStr());
        }
    }

    bool HasIpcInCapacity()
    {
        return attr_->HasValue(ASTAttr::IPC_IN_CAPACITY);
    }

    std::string &GetIpcInCapacity()
    {
        return attr_->GetIpcInCapacity();
    }

    void SetIpcInCapacity(const AutoPtr<ASTAttr> &attr)
    {
        if (attr->HasValue(ASTAttr::IPC_IN_CAPACITY)) {
            attr_->SetIpcInCapacity(attr->GetIpcInCapacity());
        }
    }

    bool HasIpcOutCapacity()
    {
        return attr_->HasValue(ASTAttr::IPC_OUT_CAPACITY);
    }

    std::string &GetIpcOutCapacity()
    {
        return attr_->GetIpcOutCapacity();
    }

    void SetIpcOutCapacity(const AutoPtr<ASTAttr> &attr)
    {
        if (attr->HasValue(ASTAttr::IPC_OUT_CAPACITY)) {
            attr_->SetIpcOutCapacity(attr->GetIpcOutCapacity());
        }
    }

    void SetIpcCode(int32_t ipc_code)
    {
        this->ipcCode_ = ipc_code;
    }

    int32_t GetIpcCode() const
    {
        return this->ipcCode_;
    }

private:
    void BuildSignature();

    std::string name_;
    std::string signature_;
    AutoPtr<ASTAttr> attr_ = new ASTAttr();
    AutoPtr<ASTType> returnType_;
    std::vector<AutoPtr<ASTParameter>> parameters_;
    bool isOverload_ = false; // used to identify if method is overload
    size_t cmdId_ = 0;        // used to identify same name method
    int32_t ipcCode_ = 0;
    std::string freezeControlReason;
    std::string messageOption_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTMETHOD_H
