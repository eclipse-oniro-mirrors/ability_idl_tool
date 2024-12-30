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

#ifndef OHOS_IDL_OPTION_H
#define OHOS_IDL_OPTION_H

#include <string>
#include <unordered_map>
#include <set>

#include "util/common.h"

namespace OHOS {
namespace Idl {
class Options {
public:
    using PkgPathMap = std::unordered_map<std::string, std::string>;

    static Options &GetInstance();

    Options(const Options &other) = delete;
    Options operator=(const Options &other) = delete;

    bool Parse(int argc, char *argv[]);

    ~Options() = default;

    struct Attribute {
        Attribute() : doHitrace(false), doLog(false) {}
        std::string hitraceTag;
        std::string logTag;
        std::string domainId;
        bool doHitrace;
        bool doLog;
    };

    inline bool DoShowUsage() const
    {
        return doShowUsage;
    }

    inline bool DoShowVersion() const
    {
        return doShowVersion;
    }

    inline bool DoCompile() const
    {
        return doCompile;
    }

    inline bool DoDumpAST() const
    {
        return doDumpAST;
    }

    inline bool DoDumpMetadata() const
    {
        return doDumpMetadata;
    }

    inline bool DoSaveMetadata() const
    {
        return doSaveMetadata;
    }

    inline bool DoGetHashKey() const
    {
        return doHashKey;
    }

    inline bool DoGenerateCode() const
    {
        return doGenerateCode;
    }

    inline bool DoGenerateKernelCode() const
    {
        return genMode == GenMode::KERNEL;
    }

    inline bool DoPassthrough() const
    {
        return genMode == GenMode::PASSTHROUGH;
    }

    inline std::set<std::string> GetSourceFiles() const
    {
        return sourceFiles;
    }

    inline std::string GetMetadataFile() const
    {
        return metadataFile;
    }

    inline PkgPathMap GetPackagePathMap() const
    {
        return packagePathMap;
    }

    inline std::string GetPackage() const
    {
        return idlPackage;
    }

    inline std::string GetGenerationDirectory() const
    {
        return genDir;
    }

    inline std::string GetOutPutFile() const
    {
        return outPutFile;
    }

    bool HasErrors() const;

    void ShowErrors() const;

    bool HasWarning() const;

    void ShowWarning() const;

    void ShowVersion() const;

    void ShowUsage() const;

    bool DoLogOn() const;

    bool DoLegalLog() const;

    std::string GetRootPackage(const std::string &package) const;

    std::string GetRootPath(const std::string &package) const;

    std::string GetSubPackage(const std::string &package) const;

    std::string GetPackagePath(const std::string &package) const;

    std::string GetImportFilePath(const std::string &import, const std::string &curPath) const;

    inline SystemLevel GetSystemLevel() const
    {
        return systemLevel;
    }

    inline GenMode GetGenMode() const
    {
        return genMode;
    }

    inline Language GetLanguage() const
    {
        return genLanguage;
    }

    inline bool DoHitraceState() const
    {
        return doHitrace;
    }

    inline bool DoSearchKeywords() const
    {
        return doKeywords;
    }

    inline std::string GetGenerateHitraceTag() const
    {
        return hitraceTag;
    }

    inline std::string GetDomainId() const
    {
        return domainId;
    }

    inline std::string GetLogTag() const
    {
        return logTag;
    }

    inline Attribute GetAttribute() const
    {
        return attribute;
    }

    inline InterfaceType GetInterfaceType() const
    {
        return interfaceType;
    }
private:
    Options()
        : program(),
        systemLevel(SystemLevel::INIT),
        genMode(GenMode::INIT),
        genLanguage(Language::CPP),
        idlPackage(),
        sourceFiles(),
        genDir(),
        packagePathMap(),
        outPutFile(),
        attribute(Attribute()),
        doShowUsage(false),
        doShowVersion(false),
        doCompile(false),
        doDumpAST(false),
        doDumpMetadata(false),
        doHashKey(false),
        doGenerateCode(false),
        doOutDir(false),
        doKeywords(false),
        doSaveMetadata(false),
        doHitrace(false),
        interfaceType(InterfaceType::SA)
    {
    }

    void SetLongOption(char op);

    bool SetSystemLevel(const std::string &system);

    bool SetGenerateMode(const std::string &mode);

    bool SetLanguage(const std::string &language);

    void SetPackage(const std::string &package);

    bool AddPackagePath(const std::string &packagePath);

    void AddSources(const std::string &sourceFile);

    void AddSourcesByDir(const std::string &dir);

    void SetOutDir(const std::string &dir);

    void SetCodePart(const std::string &part);

    bool DoLegalParam(const std::string &tag);

    bool SetHiTrace(const std::string &tag);

    bool SetLogDomainId(const std::string &id);

    bool SetLogTag(const std::string &id);

    bool SetInterfaceType(const std::string &type);

    bool DoSupportHdiType();

    bool DoSupportSmType();

    void SetSmDefaultOption();

    bool CheckSmOptions();

    bool DoSupportSaType();

    bool CheckHdiOptions();

    void SetHdiDefaultOption();

    bool CheckSaOptions();

    bool SetMetadataFile(const std::string &metadataFile);

    bool CheckOptions();

    bool ParseSingle(int option, const std::string& optVal);

    bool ParseOptionWithValue(int option, const std::string& optVal);

    static const char *optSupportArgs;

    static constexpr int VERSION_MAJOR  = 1;
    static constexpr int VERSION_MINOR = 0;

    std::string program;
    SystemLevel systemLevel;
    GenMode genMode;
    Language genLanguage;
    std::string idlPackage;
    std::string sourceDir; // '-D <directory>'
    std::set<std::string> sourceFiles; // '-c <*.idl>'
    std::string genDir; // '-d <directory>'
    PkgPathMap packagePathMap;
    std::string outPutFile;
    std::string hitraceTag;
    std::string domainId;
    std::string logTag;
    std::string metadataFile;
    Attribute attribute;
    std::string illegalOptions;

    bool doShowUsage;
    bool doShowVersion; // '-v'
    bool doCompile; // '-c'
    bool doDumpAST;
    bool doDumpMetadata;
    bool doHashKey;
    bool doGenerateCode;
    bool doOutDir; // '-d'
    bool doKeywords; // "-t" "-log-.."
    bool doSaveMetadata;
    bool doHitrace;
    InterfaceType interfaceType;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_OPTION_H