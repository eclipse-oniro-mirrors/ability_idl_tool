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

#ifndef OHOS_IDL_AST_ATTRIBUTE_H
#define OHOS_IDL_AST_ATTRIBUTE_H

#include "ast/ast_node.h"

namespace OHOS {
namespace Idl {
class ASTAttr : public ASTNode {
public:
    using Attribute = uint32_t;
    static constexpr Attribute NONE = 0U;
    static constexpr Attribute MINI = 0x1U;
    static constexpr Attribute LITE = 0x1U << 1;
    static constexpr Attribute FULL = 0x1U << 2;
    static constexpr Attribute ONEWAY = 0x1U << 3;
    static constexpr Attribute CALLBACK = 0x1U << 4;
    static constexpr Attribute CACHEABLE = 0x1U << 5;
    static constexpr Attribute FREEZECONTROL = 0x1U << 6;
    static constexpr Attribute IPCCODE = 0x1U << 7;
    static constexpr Attribute IPC_IN_CAPACITY = 0x1U << 8;
    static constexpr Attribute IPC_OUT_CAPACITY = 0x1U << 9;
    static constexpr Attribute CUSTOM_MSG_OPTION = 0x1U << 10;

    explicit ASTAttr(Attribute value = ASTAttr::NONE) : value_(value) {}

    std::string ToString() const override;

    std::string Dump(const std::string &prefix) override;

    inline void SetValue(Attribute value)
    {
        value_ |= value;
    }

    inline Attribute GetValue() const
    {
        return value_;
    }

    bool IsNone() const
    {
        return value_  == NONE;
    }

    bool HasValue(Attribute attr) const
    {
        return (value_ & attr) != 0;
    }

    bool Match(SystemLevel level) const;

    int32_t GetCacheableTime()
    {
        return cacheableTime_;
    }

    std::string& GetCacheableTimeString()
    {
        return cacheableTimeString_;
    }

    void SetCacheableTimeString(const std::string &timeStr)
    {
        cacheableTimeString_ = timeStr;
    }

    bool CacheableStrToInt();

    std::string& GetIpcCodeStr()
    {
        return ipcCodeStr_;
    }

    void SetIpcCodeStr(const std::string &ipcCodeStr)
    {
        ipcCodeStr_ = ipcCodeStr;
    }

    std::string &GetIpcInCapacity()
    {
        return ipcInCapacity_;
    }

    void SetIpcInCapacity(const std::string &capacity)
    {
        ipcInCapacity_ = capacity;
    }

    std::string &GetIpcOutCapacity()
    {
        return ipcOutCapacity_;
    }

    void SetIpcOutCapacity(const std::string &capacity)
    {
        ipcOutCapacity_ = capacity;
    }

private:
    Attribute value_;
    int32_t cacheableTime_ = 0;
    std::string cacheableTimeString_;
    std::string ipcCodeStr_;
    std::string ipcInCapacity_;
    std::string ipcOutCapacity_;
};

class ASTParamAttr : public ASTNode {
public:
    using ParamAttr = uint32_t;
    static constexpr ParamAttr PARAM_NONE = 0U;
    static constexpr ParamAttr PARAM_IN = 0x1U;
    static constexpr ParamAttr PARAM_OUT = 0x1U << 1;
    static constexpr ParamAttr PARAM_INOUT = (PARAM_IN | PARAM_OUT);

    explicit ASTParamAttr(ParamAttr value) : ASTNode(), value_(value) {}

    std::string ToString() const override;

    std::string Dump(const std::string &prefix) override;

public:
    ParamAttr value_;
};

} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_AST_ATTRIBUTE_H