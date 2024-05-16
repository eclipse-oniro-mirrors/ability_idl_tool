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

#ifndef OHOS_IDL_ASTINTERFACETYPE_H
#define OHOS_IDL_ASTINTERFACETYPE_H

#include "ast/ast_attribute.h"
#include "ast/ast_method.h"

#include <vector>

#include "ast/ast_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
class ASTInterfaceType : public ASTType {
public:
    ASTInterfaceType()
        : ASTType(false),
        license_(),
        attr_(new ASTAttr()),
        isSerializable_(false),
        methods_(),
        getVerMethod_(),
        extendsInterface_(nullptr),
        majorVersion_(1),
        minorVersion_(0),
        isExternal_(false)
    {
    }

    void SetNamespace(const AutoPtr<ASTNamespace> &nspace) override;

    inline void SetLicense(const std::string &license)
    {
        license_ = license;
    }

    inline std::string GetLicense() const
    {
        return license_;
    }

    void SetAttribute(const AutoPtr<ASTAttr> &attr)
    {
        if (attr != nullptr) {
            attr_ = attr;
            if (attr_->HasValue(ASTAttr::CALLBACK)) {
                isSerializable_ = true;
            }
        }
    }

    inline AutoPtr<ASTAttr> GetAttribute() const
    {
        return attr_;
    }

    inline bool IsAttributeNone() const
    {
        return attr_->IsNone();
    }

    inline bool IsOneWay() const
    {
        return attr_->HasValue(ASTAttr::ONEWAY);
    }

    inline bool IsCallback() const
    {
        return attr_->HasValue(ASTAttr::CALLBACK);
    }

    inline void SetSerializable(bool isSerializable)
    {
        isSerializable_ = isSerializable;
    }

    inline bool IsSerializable() const
    {
        return isSerializable_;
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

    void AddMethod(const AutoPtr<ASTMethod> &method);

    AutoPtr<ASTMethod> GetMethod(size_t index);

    std::vector<AutoPtr<ASTMethod>> GetMethodsBySystem(SystemLevel system) const;

    inline size_t GetMethodNumber() const
    {
        return methods_.size();
    }

    void AddVersionMethod(const AutoPtr<ASTMethod> &method)
    {
        getVerMethod_ = method;
    }

    AutoPtr<ASTMethod> GetVersionMethod()
    {
        return getVerMethod_;
    }

    bool AddExtendsInterface(AutoPtr<ASTInterfaceType> interface);

    AutoPtr<ASTInterfaceType> GetExtendsInterface()
    {
        return extendsInterface_;
    }

    inline size_t GetMajorVersion()
    {
        return majorVersion_;
    }

    inline size_t GetMinorVersion()
    {
        return minorVersion_;
    }

    inline void SetExternal(bool external)
    {
        isExternal_ = external;
    }

    inline bool IsExternal() const
    {
        return isExternal_;
    }

    void SetVersion(size_t majorVer, size_t minorVer);

    std::string GetSignature() override;

    bool IsInterfaceType() override;

    std::string Dump(const std::string &prefix) override;

    TypeKind GetTypeKind() override;

    std::string GetFullName() const;

private:
    std::string license_;

    AutoPtr<ASTAttr> attr_;
    bool isSerializable_;
    std::vector<AutoPtr<ASTMethod>> methods_;
    AutoPtr<ASTMethod> getVerMethod_;
    AutoPtr<ASTInterfaceType> extendsInterface_;
    size_t majorVersion_;
    size_t minorVersion_;
    bool isExternal_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTINTERFACETYPE_H