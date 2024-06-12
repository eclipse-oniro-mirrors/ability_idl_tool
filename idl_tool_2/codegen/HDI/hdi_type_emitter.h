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

#ifndef OHOS_IDL_HDI_TYPE_EMITTER_H
#define OHOS_IDL_HDI_TYPE_EMITTER_H

#include <map>
#include "ast/ast_type.h"

namespace OHOS {
namespace Idl {
constexpr const char *MAX_BUFF_SIZE_MACRO = "HDI_BUFF_MAX_SIZE";
constexpr const char *MAX_BUFF_SIZE_VALUE = "1024 * 200";    // 200KB
constexpr const char *CHECK_VALUE_RETURN_MACRO = "HDI_CHECK_VALUE_RETURN";
constexpr const char *CHECK_VALUE_RET_GOTO_MACRO = "HDI_CHECK_VALUE_RET_GOTO";

using UtilMethodMap = std::unordered_map<std::string, std::string>;
class HdiTypeEmitter : public LightRefCountBase {
public:
    bool IsPod() const;

    void SetPod(const bool &isPod);

    virtual TypeKind GetTypeKind();

    void SetTypeName(const std::string &name);

    std::string GetTypeName();

    void SetName(const std::string &name);

    std::string GetName();

    virtual std::string EmitCppType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual std::string EmitCType(TypeMode mode = TypeMode::NO_MODE) const;

    virtual std::string EmitJavaType(TypeMode mode, bool isInnerType = false) const;

    virtual std::string EmitCTypeDecl() const;

    virtual std::string EmitCppTypeDecl() const;

    virtual std::string EmitJavaTypeDecl() const;

    virtual void EmitCWriteVar(TypeMode mode, const std::string &name, const std::string &gotoLabel,
        StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitCProxyWriteOutVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
        StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix) const;

    virtual void EmitCStubReadOutVar(const std::string &memFlagName, const std::string &name,
        const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitCppWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const;

    virtual void EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
        TypeMode mode, unsigned int innerLevel = 0) const;

    virtual void EmitCMarshalling(const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitCUnMarshalling(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
        const std::string &prefix, std::vector<std::string> &freeObjStatements) const;

    virtual void EmitCWriteMethods(UtilMethodMap &methods, const std::string &prefix,
        const std::string &methodPrefix, bool isDecl) const;

    virtual void EmitCppWriteMethods(UtilMethodMap &methods, const std::string &prefix,
        const std::string &methodPrefix, bool isDecl) const;

    virtual void EmitCReadMethods(UtilMethodMap &methods, const std::string &prefix,
        const std::string &methodPrefix, bool isDecl) const;

    virtual void EmitCppReadMethods(UtilMethodMap &methods, const std::string &prefix,
        const std::string &methodPrefix, bool isDecl) const;

    virtual void EmitCStubReadMethods(UtilMethodMap &methods, const std::string &prefix,
        const std::string &methodPrefix, bool isDecl) const;

    void EmitFreeStatements(
        const std::vector<std::string> &freeObjStatements, StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitCppMarshalling(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const;

    virtual void EmitCppUnMarshalling(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, unsigned int innerLevel = 0) const;

    virtual void EmitMemoryRecycle(
        const std::string &name, bool ownership, StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitJavaWriteVar(const std::string &parcelName, const std::string &name, StringBuilder &sb,
        const std::string &prefix, TypeMode mode = TypeMode::NO_MODE) const;

    virtual void EmitJavaReadVar(
        const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const;

    virtual void EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
        StringBuilder &sb, const std::string &prefix) const;

    static std::string dataParcelName_;
    static std::string replyParcelName_;
    static std::string errorCodeName_;

protected:
    bool isPod_ = false;
    std::string typeName_;
    std::string name_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_HDI_TYPE_EMITTER_H