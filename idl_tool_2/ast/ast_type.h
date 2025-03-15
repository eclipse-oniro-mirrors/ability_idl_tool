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

#ifndef OHOS_IDL_ASTTYPE_H
#define OHOS_IDL_ASTTYPE_H

#include <functional>
#include <regex>
#include <unordered_map>

#include "ast/ast_namespace.h"
#include "ast/ast_node.h"
#include "util/autoptr.h"
#include "util/options.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
enum class TypeKind {
    TYPE_UNKNOWN = 0,
    TYPE_VOID,
    TYPE_BOOLEAN,
    TYPE_BYTE,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_UCHAR,
    TYPE_USHORT,
    TYPE_UINT,
    TYPE_ULONG,
    TYPE_STRING,
    TYPE_STRING16,
    TYPE_U16STRING,
    TYPE_FILEDESCRIPTOR,
    TYPE_SEQUENCEABLE,
    TYPE_RAWDATA,
    TYPE_INTERFACE,
    TYPE_LIST,
    TYPE_MAP,
    TYPE_ORDEREDMAP,
    TYPE_ARRAY,
    TYPE_ENUM,
    TYPE_STRUCT,
    TYPE_UNION,
    TYPE_SMQ,
    TYPE_ASHMEM,
    TYPE_NATIVE_BUFFER,
    TYPE_POINTER,
};

enum class TypeMode {
    NO_MODE,   // only type
    PARAM_IN,  // type of the in attribute parameter
    PARAM_OUT, // type of the out attribute parameter
    PARAM_INOUT, // type of the inout attribute parameter
    LOCAL_VAR, // type of the local variable
};

enum class SerMode {
    PROXY_SER,  // the flag of proxy serialized
    STUB_SER,   // the flag of stub serialized
    CUSTOM_SER, // the flag of custom types serialized
};

class ASTType : public ASTNode {
public:
    explicit ASTType(bool isPod = true, TypeKind typekind = TypeKind::TYPE_UNKNOWN)
        :isPod_(isPod), typeKind_(typekind), name_(), namespace_()
    {
    }

    virtual void SetName(const std::string &name);

    virtual std::string GetName();

    virtual void SetNamespace(const AutoPtr<ASTNamespace> &nspace);

    virtual AutoPtr<ASTNamespace> GetNamespace();

    virtual std::string GetSignature() = 0;

    virtual bool IsVoidType();

    virtual bool IsBooleanType();

    virtual bool IsByteType();

    virtual bool IsShortType();

    virtual bool IsIntegerType();

    virtual bool IsLongType();

    virtual bool IsCharType();

    virtual bool IsUcharType();

    virtual bool IsUshortType();

    virtual bool IsUintType();

    virtual bool IsUlongType();

    virtual bool IsFloatType();

    virtual bool IsDoubleType();

    virtual bool IsStringType();

    virtual bool IsU16stringType();

    virtual bool IsListType();

    virtual bool IsMapType();

    virtual bool IsOrderedMapType();

    virtual bool IsEnumType();

    virtual bool IsStructType();

    virtual bool IsUnionType();

    virtual bool IsInterfaceType();

    virtual bool IsSequenceableType();

    virtual bool IsRawDataType();

    virtual bool IsArrayType();

    virtual bool IsFdType();

    virtual bool IsSmqType();

    virtual bool IsAshmemType();

    virtual bool IsNativeBufferType();

    virtual bool IsPointerType();

    bool IsPod() const;

    bool IsTypeKind(TypeKind typekind) const;

    virtual bool HasInnerType(TypeKind innerType) const;

    virtual std::string ToShortString();

    std::string ToString() const override;

    virtual TypeKind GetTypeKind();
protected:
    bool isPod_;
    TypeKind typeKind_;
    std::string name_;
    AutoPtr<ASTNamespace> namespace_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_ASTTYPE_H