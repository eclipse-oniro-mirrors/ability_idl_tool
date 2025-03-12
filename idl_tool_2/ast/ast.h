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

#ifndef OHOS_IDL_AST_H
#define OHOS_IDL_AST_H

#include <unordered_map>
#include <vector>

#include "ast/ast_void_type.h"
#include "ast/ast_array_type.h"
#include "ast/base/ast_boolean_type.h"
#include "ast/ast_native_buffer_type.h"
#include "ast/base/ast_byte_type.h"
#include "ast/base/ast_double_type.h"
#include "ast/ast_enum_type.h"
#include "ast/ast_fd_type.h"
#include "ast/base/ast_float_type.h"
#include "ast/base/ast_integer_type.h"
#include "ast/ast_interface_type.h"
#include "ast/base/ast_long_type.h"
#include "ast/ast_map_type.h"
#include "ast/ast_namespace.h"
#include "ast/ast_node.h"
#include "ast/ast_pointer_type.h"
#include "ast/ast_rawdata_type.h"
#include "ast/ast_sequenceable_type.h"
#include "ast/base/ast_short_type.h"
#include "ast/ast_smq_type.h"
#include "ast/base/ast_string_type.h"
#include "ast/base/ast_string16_type.h"
#include "ast/ast_struct_type.h"
#include "ast/base/ast_char_type.h"
#include "ast/base/ast_uchar_type.h"
#include "ast/base/ast_uint_type.h"
#include "ast/base/ast_ulong_type.h"
#include "ast/ast_union_type.h"
#include "ast/base/ast_ushort_type.h"
#include "util/autoptr.h"

namespace OHOS {
namespace Idl {
enum class ASTFileType {
    AST_IFACE,        // this idl file contains class of normal interface
    AST_CALL_IFACE,   // this idl file contains class of interface that as parameter
    AST_ICALLBACK,    // this idl file contains class of callback interface
    AST_TYPES,        // this idl file contains custom types
    AST_SEQUENCEABLE, // this is not an idl file, but a c++/java file
    AST_RAWDATA, // this is not an idl file, but a c++/java file
};

class AST : public ASTNode {
public:
    using StrASTMap = std::unordered_map<std::string, AutoPtr<AST>>;
    using TypeStringMap = std::unordered_map<std::string, AutoPtr<ASTType>>;

    ~AST() override = default;

    void SetAStFileType(ASTFileType fileType)
    {
        astFileType_ = fileType;
    }

    ASTFileType GetASTFileType() const
    {
        return astFileType_;
    }

    void SetIdlFile(const std::string &idlFile);

    inline std::string GetName()
    {
        return name_;
    }

    void SetFullName(const std::string &fullName);

    inline std::string GetFullName()
    {
        return packageName_ + "." + name_;
    }

    inline std::string GetPackageName() const
    {
        return packageName_;
    }

    inline void SetLicense(const std::string &license)
    {
        license_ = license;
    }

    inline std::string GetLicense()
    {
        return license_;
    }

    inline std::string GetIdlFile()
    {
        return idlFilePath_;
    }

    void SetPackageName(const std::string &packageName);

    AutoPtr<ASTNamespace> ParseNamespace(const std::string &nspaceStr);

    void AddNamespace(const AutoPtr<ASTNamespace> &nspace);

    AutoPtr<ASTNamespace> FindNamespace(const std::string &nspaceStr);

    AutoPtr<ASTNamespace> GetNamespace(size_t index);

    inline std::vector<AutoPtr<ASTNamespace>> GetNamespace()
    {
        return namespaces_;
    }

    inline size_t GetNamespaceNumber()
    {
        return namespaces_.size();
    }

    void AddInterfaceDef(const AutoPtr<ASTInterfaceType> &interface);

    AutoPtr<ASTInterfaceType> GetInterfaceDef(size_t index = 0);

    inline size_t GetInterfaceDefNumber() const
    {
        return interfaceDefs_.size();
    }

    void AddSequenceableDef(const AutoPtr<ASTSequenceableType> &sequenceable);

    AutoPtr<ASTSequenceableType> GetSequenceableDef(size_t index = 0);

    inline size_t GetSequenceableDefNumber() const
    {
        return sequenceableDefs_.size();
    }

    void AddRawDataDef(const AutoPtr<ASTRawDataType> &rawdata);

    AutoPtr<ASTRawDataType> GetRawDataDef(size_t index = 0);

    inline size_t GetRawDataDefNumber() const
    {
        return rawdataDefs_.size();
    }

    int IndexOf(ASTInterfaceType* interface);

    int IndexOf(ASTSequenceableType* sequenceable);

    int IndexOf(ASTRawDataType* rawdata);

    int IndexOf(ASTType* type);

    void AddType(const AutoPtr<ASTType> &type);

    AutoPtr<ASTType> FindType(const std::string &typeName, bool lookImports = true);

    inline const TypeStringMap &GetTypes() const
    {
        return types_;
    }

    inline size_t GetTypeNumber() const
    {
        return types_.size();
    }

    void AddTypeDefinition(const AutoPtr<ASTType> &type);

    inline size_t GetTypeDefinitionNumber() const
    {
        return typeDefinitions_.size();
    }

    AutoPtr<ASTType> GetTypeDefintion(size_t index);

    std::string Dump(const std::string &prefix) override;

    bool AddImport(const AutoPtr<AST> &importAst);

    void AddImportName(const std::string &importName);

    void ClearImport()
    {
        return imports_.clear();
    }

    inline const StrASTMap &GetImports() const
    {
        return imports_;
    }

    inline const std::vector<std::string> &GetImportNames() const
    {
        return importNames_;
    }

    void SetVersion(size_t &majorVer, size_t &minorVer);

    inline size_t GetMajorVer() const
    {
        return majorVersion_;
    }

    inline size_t GetMinorVer() const
    {
        return minorVersion_;
    }

    std::string GetVersion() const
    {
        return StringHelper::Format("%u.%u", majorVersion_, minorVersion_);
    }

    inline std::string GetIdlFilePath()
    {
        return idlFilePath_;
    }

    bool IsValid();
    std::vector<AutoPtr<ASTInterfaceType>> GetInterfaceDefs() const
    {
        return interfaceDefs_;
    }

    bool GetHasCacheableProxyMethods() const
    {
        return hasCacheableProxyMethods_;
    }

    void SetHasCacheableProxyMethods(bool cacheable)
    {
        hasCacheableProxyMethods_ = cacheable;
    }

    inline void SetInterfaceToken(std::string &interfaceToken)
    {
        interfaceToken_ = interfaceToken;
    }

    inline std::string GetInterfaceToken() const
    {
        return interfaceToken_;
    }

    inline void SetSupportDelegator(std::string &supportDelegator)
    {
        if (supportDelegator == "on") {
            supportDelegatorOn_ = true;
        } else {
            supportDelegatorOn_ = false;
        }
    }

    inline bool GetSupportDelegatorOn() const
    {
        return supportDelegatorOn_;
    }

    inline void SetOptionStubHooks(std::string &optionStubHooks)
    {
        if (optionStubHooks == "on") {
            optionStubHooksOn_ = true;
        } else {
            optionStubHooksOn_ = false;
        }
    }

    inline bool GetOptionStubHooksOn() const
    {
        return optionStubHooksOn_;
    }

    inline void SetOptionParcelHooks(std::string &optionParcelHooks)
    {
        if (optionParcelHooks == "on") {
            optionParcelHooksOn_ = true;
        } else {
            optionParcelHooksOn_ = false;
        }
    }

    inline bool GetOptionParcelHooksOn() const
    {
        return optionParcelHooksOn_;
    }

private:
    AutoPtr<ASTNamespace> NewNameSpace(std::string nameSpace);

    ASTFileType astFileType_ = ASTFileType::AST_IFACE;
    std::string name_;
    std::string license_;
    std::string packageName_;
    size_t majorVersion_;
    size_t minorVersion_;
    std::vector<AutoPtr<ASTNamespace>> namespaces_;
    std::vector<AutoPtr<ASTType>> typeDefinitions_; // enum, struct, union
    std::vector<AutoPtr<ASTSequenceableType>> sequenceableDefs_;
    std::vector<AutoPtr<ASTRawDataType>> rawdataDefs_;
    std::vector<AutoPtr<ASTInterfaceType>> interfaceDefs_;
    std::vector<std::string> importNames_;

    StrASTMap imports_;
    TypeStringMap types_;

    static TypeStringMap basicTypes_;

    std::string idlFilePath_;
    bool hasCacheableProxyMethods_ = false;
    std::string interfaceToken_;
    bool supportDelegatorOn_ = false;
    bool optionStubHooksOn_ = false;
    bool optionParcelHooksOn_ = false;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_AST_H