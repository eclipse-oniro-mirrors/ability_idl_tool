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

#ifndef OHOS_IDL_CODE_EMITTER_H
#define OHOS_IDL_CODE_EMITTER_H

#include <set>

#include "ast/ast.h"
#include "util/autoptr.h"
#include "util/light_refcount_base.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
enum class HeaderFileType {
    OWN_HEADER_FILE,
    SYSTEM_HEADER_FILE,
    C_STD_HEADER_FILE,
    CPP_STD_HEADER_FILE,
    OTHER_MODULES_HEADER_FILE,
    OWN_MODULE_HEADER_FILE,
};

struct HeaderFile {
    HeaderFile(HeaderFileType type, std::string fileName) : type_(type), fileName_(fileName) {}

    struct Compare {
        bool operator()(const HeaderFile &lhs, const HeaderFile &rhs) const
        {
            int compareRet = lhs.fileName_.compare(rhs.fileName_);
            if (compareRet == 0) {
                return false;
            }

            if (lhs.type_ != rhs.type_) {
                return lhs.type_ < rhs.type_;
            }

            return compareRet < 0;
        }
    };

    std::string ToString() const
    {
        switch (type_) {
            case HeaderFileType::OWN_HEADER_FILE:
            case HeaderFileType::OWN_MODULE_HEADER_FILE:
                return StringHelper::Format("#include \"%s.h\"", fileName_.c_str());
            case HeaderFileType::SYSTEM_HEADER_FILE:
            case HeaderFileType::C_STD_HEADER_FILE:
            case HeaderFileType::OTHER_MODULES_HEADER_FILE:
                return StringHelper::Format("#include <%s.h>", fileName_.c_str());
            case HeaderFileType::CPP_STD_HEADER_FILE:
                return StringHelper::Format("#include <%s>", fileName_.c_str());
            default:
                return StringHelper::Format("//");
        }
    }

    using HeaderFileSet = std::set<HeaderFile, HeaderFile::Compare>;

    HeaderFileType type_;
    std::string fileName_;
};

class CodeEmitter : public LightRefCountBase {
public:
    ~CodeEmitter() override = default;

    virtual bool OutPut(const AutoPtr<AST> &ast, const std::string &targetDirectory, GenMode mode) = 0;

protected:
    virtual bool ResolveDirectory(const std::string &targetDirectory) = 0;

    virtual void EmitCode() = 0;

    void EmitLicense(StringBuilder &sb);

    bool NeedFlag(const AutoPtr<ASTMethod> &method) const;

    std::string GetFileParentPath(const std::string &outDir) const;

    std::string PackageToFilePath(const std::string &packageName) const;

    std::string InterfaceToFilePath(const std::string &interfaceName) const;

    std::string EmitMethodCmdID(const AutoPtr<ASTMethod> &method);

    virtual void EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix);

    /* add version prefix
     * MajorVersion: 1
     * MinorVersion: 0
     * name: IFoo
     * result: v1_0/ifoo.h
     */
    std::string EmitVersionHeaderName(const std::string &name) const;

    // log tag macro of hdf
    void EmitLogTagMacro(StringBuilder &sb, const std::string &name) const;

    // file_name -> FILE_NAME
    std::string ConstantName(const std::string &name) const;

    // file_name -> FileName
    std::string PascalName(const std::string &name) const;

    // FileName -> file_name
    std::string FileName(const std::string &name) const;

    std::string GetNamespace(const std::string &fpnp) const;

    void EmitHeadMacro(StringBuilder &sb, const std::string &fullName) const;

    void EmitTailMacro(StringBuilder &sb, const std::string &fullName) const;

    void EmitHeadExternC(StringBuilder &sb) const;

    void EmitTailExternC(StringBuilder &sb) const;

    std::string MacroName(const std::string &name) const;

protected:
    AutoPtr<AST> ast_ = nullptr;
    AutoPtr<ASTInterfaceType> interface_ = nullptr;
    std::string directory_;

    std::string interfaceName_;
    std::string interfaceFullName_;
    std::string baseName_;
    std::string proxyName_;
    std::string proxyFullName_;
    std::string stubName_;
    std::string stubFullName_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CODE_EMITTER_H
