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

#include "util/options.h"

#include <climits>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#include <map>

#include "util/common.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/string_helper.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
const char *Options::optSupportArgs = "hvcs:m:p:d:r:o:D:t:";
static struct option g_longOpts[] = {
    {"help",          no_argument,       nullptr, 'h'},
    {"version",       no_argument,       nullptr, 'v'},
    {"client-enable", no_argument,       nullptr, 'C'},
    {"system",        required_argument, nullptr, 'S'},
    {"mode",          required_argument, nullptr, 'm'},
    {"gen-c",         no_argument,       nullptr, '1'},
    {"gen-cpp",       no_argument,       nullptr, '2'},
    {"gen-java",      no_argument,       nullptr, '3'},
    {"gen-rust",      no_argument,       nullptr, '4'},
    {"gen-ts",        no_argument,       nullptr, '5'},
    {"package",       required_argument, nullptr, 'p'},
    {"dump-ast",      no_argument,       nullptr, 'a'},
    {"dump-metadata", no_argument,       nullptr, 'e'},
    {"hash",          no_argument,       nullptr, 'H'},
    {"log-domainid",  required_argument, nullptr, 'i'},
    {"log-tag",       required_argument, nullptr, 'g'},
    {"intf-type",     required_argument, nullptr, 'T'},
    {nullptr,         0,                 nullptr,  0 }
};

Options &Options::GetInstance()
{
    static Options option;
    return option;
}

bool Options::Parse(int argc, char *argv[])
{
    int ret = true;
    program = argv[0];
    opterr = 0;
    int op = 0;
    int optIndex = 0;

    if (argc < INT_MIN || argc > INT_MAX) {
        Logger::E(TAG, "argc out of range");
        return false;
    }

    while ((op = getopt_long_only(argc, argv, optSupportArgs, g_longOpts, &optIndex)) != -1) {
        if (optind > 0 && optind <= argc) {
            ret = ParseSingle(op, std::string(argv[optind - 1]));
        }
    }

    if (doCompile) {
        int i = 0;
        while (++i < argc) {
            if (argv[i][0] != '-' && StringHelper::EndWith(argv[i], ".idl")) {
                AddSources(argv[i]);
            }
        }
    }

    return ret ? CheckOptions() : ret;
}

bool Options::ParseSingle(int option, std::string optVal)
{
    bool ret = true;
    switch (option) {
        case 'h':
            doShowUsage = true;
            break;
        case 'v':
            doShowVersion = true;
            break;
        case 'a':
            doDumpAST = true;
            break;
        case 'e':
            doDumpMetadata = true;
            break;
        case 'H':
            doHashKey = true;
            break;
        case 'c':
            doCompile = true;
            break;
        case 'C':
            doClient = true;
            break;
        case '1':
            SetLanguage("c");
            break;
        case '2':
            SetLanguage("cpp");
            break;
        case '3':
            SetLanguage("java");
            break;
        case '4':
            SetLanguage("rust");
            break;
        case '5':
            SetLanguage("ts");
            break;
        default:
            ret = ParseOptionWithValue(option, optVal);
            break;
    }
    return ret;
}

bool Options::ParseOptionWithValue(int option, std::string optVal)
{
    bool ret = true;
    StringBuilder errors;

    switch (option) {
        case 'o':
            outPutFile = optarg;
            break;
        case 'S':
            ret = SetSystemLevel(optarg);
            break;
        case 'm':
            ret = SetGenerateMode(optarg);
            break;
        case 's':
            SetMetadataFile(optarg);
            break;
        case 'p':
            SetPackage(optarg);
            break;
        case 'D':
            AddSourcesByDir(optarg);
            break;
        case 'd':
            SetOutDir(optarg);
            break;
        case 'r':
            ret = AddPackagePath(optarg);
            break;
        case 't':
            ret = SetHiTrace(optarg);
            break;
        case 'i':
            ret = SetLogDomainId(optarg);
            break;
        case 'g':
            ret = SetLogTag(optarg);
            break;
        case 'T':
            ret = SetInterfaceType(optarg);
            break;
        default:
            errors.Append(optVal.c_str());
            break;
    }
    illegalOptions = errors.ToString();
    return ret;
}

bool Options::SetSystemLevel(const std::string &system)
{
    static std::map<std::string, SystemLevel> systemLevelMap = {
        {"mini", SystemLevel::MINI},
        {"lite", SystemLevel::LITE},
        {"full", SystemLevel::FULL},
    };

    auto levelIter = systemLevelMap.find(system);
    if (levelIter == systemLevelMap.end()) {
        Logger::E(TAG, "invalid system level set: '%s', please input mini/lite/full", system.c_str());
        return false;
    }
    systemLevel = levelIter->second;
    return true;
}

bool Options::SetGenerateMode(const std::string &mode)
{
    static std::map<std::string, GenMode> codeGenMap = {
        {"low", GenMode::LOW},
        {"passthrough", GenMode::PASSTHROUGH},
        {"ipc", GenMode::IPC},
        {"kernel", GenMode::KERNEL},
    };

    auto codeGenIter = codeGenMap.find(mode);
    if (codeGenIter == codeGenMap.end()) {
        Logger::E(TAG, "invalid generate mode set: '%s', please input low/passthrough/ipc/kernel/sa.", mode.c_str());
        return false;
    }
    genMode = codeGenIter->second;
    return true;
}

bool Options::SetLanguage(const std::string &language)
{
    static const std::map<std::string, Language> languageMap = {
        {"c", Language::C},
        {"cpp", Language::CPP},
        {"java", Language::JAVA},
        {"rust", Language::RUST},
        {"ts", Language::TS},
    };

    const auto kindIter = languageMap.find(language);
    if (kindIter == languageMap.end()) {
        Logger::E(TAG, "invalid language '%s', please input c, cpp, java, rust or ts", language.c_str());
        return false;
    }

    doGenerateCode = true;
    genLanguage = kindIter->second;
    return true;
}

bool Options::SetMetadataFile(const std::string &file)
{
    doSaveMetadata = true;
    metadataFile = file;
    return true;
}

void Options::SetPackage(const std::string &infPackage)
{
    idlPackage = infPackage;
}

void Options::AddSources(const std::string &sourceFile)
{
    std::string realPath = File::AdapterRealPath(sourceFile);
    if (realPath.empty()) {
        Logger::E(TAG, "invalid idl file path:%s", sourceFile.c_str());
        return;
    }

    if (sourceFiles.insert(realPath).second == false) {
        Logger::E(TAG, "this idl file has been add:%s", sourceFile.c_str());
        return;
    }
}

void Options::AddSourcesByDir(const std::string &dir)
{
    sourceDir = dir;
    std::set<std::string> files = File::FindFiles(sourceDir);
    if (!files.empty()) {
        doCompile = true;
        sourceFiles.insert(files.begin(), files.end());
    }
}

bool Options::AddPackagePath(const std::string &packagePath)
{
    size_t index = packagePath.find(":");
    if (index == std::string::npos || index == packagePath.size() - 1) {
        Logger::E(TAG, "invalid option parameters '%s'.", packagePath.c_str());
        return false;
    }

    std::string package = packagePath.substr(0, index);
    std::string path = File::AdapterRealPath(packagePath.substr(index + 1));
    if (path.empty()) {
        Logger::E(TAG, "invalid path '%s'.", packagePath.substr(index + 1).c_str());
        return false;
    }

    auto it = packagePathMap.find(package);
    if (it != packagePathMap.end()) {
        Logger::E(TAG, "The '%s:%s' has been set.", package.c_str(), path.c_str());
        return false;
    }

    packagePathMap[package] = path;
    return true;
}

void Options::SetOutDir(const std::string &dir)
{
    doOutDir = true;
    genDir = dir;
}

bool Options::CheckOptions()
{
    if (doShowUsage || doShowVersion) {
        return true;
    }
    if (HasErrors()) {
        ShowErrors();
        return false;
    }
    if (interfaceType == InterfaceType::SA) {
        return CheckSaOptions();
    } else if (interfaceType == InterfaceType::HDI) {
        return CheckHdiOptions();
    } else if (interfaceType == InterfaceType::SM ||
                interfaceType == InterfaceType::SAM ||
                interfaceType == InterfaceType::SAM_SM ||
                interfaceType == InterfaceType::SAM_UDS ||
                interfaceType == InterfaceType::SM_UDS) {
        return CheckSmOptions();
    } else {
        Logger::E(TAG, "Interface type 'intf-type' value '%d' invalid, please input 'hdi' or 'sa'.", interfaceType);
        return false;
    }
}

bool Options::CheckSaOptions()
{
    if (!DoSupportSaType()) {
        return false;
    }

    if (sourceFiles.empty() && !DoSaveMetadata()) {
        Logger::E(TAG, "Option 'intf-type sa' must set idl file.");
        return false;
    }

    if (!DoLegalLog()) {
        Logger::E(TAG, "Option 'log-domainid' and 'log-tag' must be used together.");
        return false;
    }

    attribute.doHitrace = doHitrace;
    attribute.hitraceTag = hitraceTag;
    attribute.logTag = logTag;
    attribute.domainId = domainId;
    attribute.doLog = DoLogOn();

    return true;
}

bool Options::DoSupportSaType()
{
    bool ret = true;
    if (genLanguage != Language::CPP && genLanguage != Language::RUST && genLanguage != Language::TS) {
        Logger::E(TAG, "Option 'intf-type sa' only support language option 'gen-cpp', 'gen-rust' or 'gen-ts'.");
        ret = false;
    }

    if (systemLevel != SystemLevel::INIT) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'system'.");
        ret = false;
    }

    if (genMode != GenMode::INIT) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'm' or 'mode'.");
        ret = false;
    }

    if (!idlPackage.empty()) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'p' or 'package'.");
        ret = false;
    }

    if (doHashKey) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'hash'.");
        ret = false;
    }

    if (!packagePathMap.empty()) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'r'.");
        ret = false;
    }

    if (!sourceDir.empty()) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'D'.");
        ret = false;
    }

    if (!outPutFile.empty()) {
        Logger::E(TAG, "Option 'intf-type sa' not support option 'o'.");
        ret = false;
    }

    if (!ret) {
        printf("Use \"-h, --help\" to show usage.\n");
    }
    return ret;
}

bool Options::CheckHdiOptions()
{
    SetHdiDefaultOption();
    if (!DoSupportHdiType()) {
        return false;
    }

    if (doCompile) {
        if (!DoGetHashKey() && !doDumpAST && !doGenerateCode && !doOutDir) {
            Logger::E(TAG, "nothing to do.");
            return false;
        }

        if (!doGenerateCode && doOutDir) {
            Logger::E(TAG, "no target language.");
            return false;
        }

        if (doGenerateCode && !doOutDir) {
            Logger::E(TAG, "no out directory.");
            return false;
        }
    } else {
        if (DoGetHashKey() || doDumpAST || doGenerateCode || doOutDir) {
            Logger::E(TAG, "no idl files.");
            return false;
        }
    }

    return true;
}

void Options::SetHdiDefaultOption()
{
    if (systemLevel == SystemLevel::INIT) {
        systemLevel = SystemLevel::FULL;
    }
    if (genMode == GenMode::INIT) {
        genMode = GenMode::IPC;
    }
    return;
}

bool Options::DoSupportHdiType()
{
    bool ret = true;

    if (genLanguage != Language::C && genLanguage != Language::CPP && genLanguage != Language::JAVA) {
        Logger::E(TAG, "Option 'intf-type hdi' only support language option 'gen-c', 'gen-cpp' or 'gen-java'.");
        ret = false;
    }

    if (doDumpMetadata) {
        Logger::E(TAG, "Option 'intf-type hdi' not support option 'dump-metadata'.");
        ret = false;
    }

    if (doKeywords) {
        Logger::E(TAG, "Option 'intf-type hdi' not support option '-t', '-log-domainid' or '-log-tag'.");
        ret = false;
    }

    if (doSaveMetadata) {
        Logger::E(TAG, "Option 'intf-type hdi' not support option '-s'.");
        ret = false;
    }

    if (!ret) {
        printf("Use \"-h, --help\" to show usage.\n");
    }
    return ret;
}

bool Options::DoSupportSmType()
{
    bool ret = true;

    if (genLanguage != Language::CPP && genLanguage != Language::JAVA) {
        Logger::E(TAG, "Option 'intf-type sm' only support language option 'gen-cpp' or 'gen-java'.");
        ret = false;
    }

    if (doDumpMetadata) {
        Logger::E(TAG, "Option 'intf-type sm' not support option 'dump-metadata'.");
        ret = false;
    }

    if (doKeywords) {
        Logger::E(TAG, "Option 'intf-type sm' not support option '-t', '-log-domainid' or '-log-tag'.");
        ret = false;
    }

    if (doSaveMetadata) {
        Logger::E(TAG, "Option 'intf-type sm' not support option '-s'.");
        ret = false;
    }

    if (!ret) {
        printf("Use \"-h, --help\" to show usage.\n");
    }
    return ret;
}

void Options::SetSmDefaultOption()
{
    systemLevel = SystemLevel::INIT;
    genMode = GenMode::INIT;
    return;
}

bool Options::CheckSmOptions()
{
    SetSmDefaultOption();
    if (!DoSupportSmType()) {
        return false;
    }

    if (doCompile) {
        if (!DoGetHashKey() && !doDumpAST && !doGenerateCode && !doOutDir) {
            Logger::E(TAG, "nothing to do.");
            return false;
        }

        if (!doGenerateCode && doOutDir) {
            Logger::E(TAG, "no target language.");
            return false;
        }

        if (doGenerateCode && !doOutDir) {
            Logger::E(TAG, "no out directory.");
            return false;
        }
    } else {
        if (DoGetHashKey() || doDumpAST || doGenerateCode || doOutDir) {
            Logger::E(TAG, "no idl files.");
            return false;
        }
    }

    return true;
}

bool Options::SetHiTrace(const std::string &tag)
{
    doKeywords = true;
    hitraceTag = tag;
    doHitrace = true;
    return true;
}

bool Options::SetLogDomainId(const std::string &id)
{
    doKeywords = true;
    domainId = id;
    return true;
}

bool Options::SetLogTag(const std::string &tag)
{
    doKeywords = true;
    logTag = tag;
    return true;
}

bool Options::SetInterfaceType(const std::string &type)
{
    static std::map<std::string, InterfaceType> Type = {
        {"hdi", InterfaceType::HDI},
        {"sa", InterfaceType::SA},
        {"sm", InterfaceType::SM},
        {"sam", InterfaceType::SAM},
        {"sam_sm", InterfaceType::SAM_SM},
        {"sam_uds", InterfaceType::SAM_UDS},
        {"sm_uds", InterfaceType::SM_UDS},
    };

    auto codeGenIter = Type.find(type);
    if (codeGenIter == Type.end()) {
        Logger::E(TAG, "invalid interface type set: '%s', please input hdi/sa.", type.c_str());
        return false;
    }
    interfaceType = codeGenIter->second;
    return true;
}

bool Options::DoLogOn() const
{
    if (!domainId.empty() && !logTag.empty()) {
        return true;
    }
    return false;
}

bool Options::DoLegalLog() const
{
    if (genLanguage == Language::CPP) {
        if (!domainId.empty() && !logTag.empty()) {
            return true;
        } else if (domainId.empty() && logTag.empty()) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}

bool Options::HasErrors() const
{
    return !illegalOptions.empty();
}

void Options::ShowErrors() const
{
    std::vector<std::string> illegalOptionsVec = StringHelper::Split(illegalOptions, " ");
    for (size_t i = 0; i < illegalOptionsVec.size(); i++) {
        Logger::E(TAG, "The Option \"%s\" is illegal.", illegalOptionsVec[i].c_str());
    }
    printf("Use \"-h, --help\" to show usage.\n");
    return;
}

bool Options::HasWarning() const
{
    if (interfaceType == InterfaceType::SA) {
        if (genLanguage == Language::RUST || genLanguage == Language::TS) {
            if (DoSearchKeywords()) {
                return true;
            }
        }
    }
    return false;
}

void Options::ShowWarning() const
{
    printf("Warning : Executed successfully, but contains invalid commands !\n");
}

void Options::ShowVersion() const
{
    printf("idl %d.%d\n"
          "Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.\n\n",
           VERSION_MAJOR, VERSION_MINOR);
}

void Options::ShowUsage() const
{
    printf("Compile a .idl file and generate C/C++/Ts/Rust and Java codes.\n"
           "Usage: idl [options] file\n"
           "Options:\n"
           "  -h, --help                      Display command line options\n"
           "  -v, --version                   Display toolchain version information\n"
           "      --system <value>            Set system level 'mini','lite' or 'full', the default value is 'full', "
           "only support 'intf-type hdi'\n"
           "  -m, --mode <value>              Set generate code mode 'low', 'passthrough', 'ipc' or 'kernel',"
           " the default value is 'ipc', only support 'intf-type hdi'\n"
           "  -p, --package <package name>    Set package of idl files, only support 'intf-type hdi'\n"
           "      --dump-ast                  Display the AST of the compiled file\n"
           "      --dump-metadata             Display the metadata generated from the compiled file, "
           "only support 'intf-type sa'\n"
           "      --hash                      Generate hash info of idl files, only support 'intf-type hdi'\n"
           "  -r <rootPackage>:<rootPath>     Set root path of root package, only support 'intf-type hdi'\n"
           "  -c                              Compile the .idl file\n"
           "  -D <directory>                  Directory of the idl file, only support 'intf-type hdi'\n"
           "  -d <directory>                  Place generated codes into <directory>\n"
           "  -o <file>                       Place the output into <file>, only support 'intf-type hdi'\n"
           "  -s <file>                       Place the metadata into <file>, only support 'intf-type sa'\n"
           "      --gen-c                     Generate C codes, only support 'intf-type hdi'\n"
           "      --gen-cpp                   Generate C++ codes\n"
           "      --gen-java                  Generate Java codes, only support 'intf-type hdi'\n"
           "      --gen-rust                  Generate Rust codes, only support 'intf-type sa'\n"
           "      --gen-ts                    Generate Ts codes, only support 'intf-type sa'\n"
           "      --log-domainid <domainid>   Place the service domain in <domainid>, Enable log(Pair with -log-tag), "
           "only support 'intf-type sa'\n"
           "      --client-enable             Generate client codes, only support 'intf-type sa'\n"
           "      --log-tag <tag>             Place the subsystem name in <tag>, Enable log(Pair with -log-domainid), "
           "only support 'intf-type sa'\n"
           "  -t <hitrace tag>                Place the constant name from hitrace_meter.h file in <hitrace tag>, "
           "only support 'intf-type sa'\n"
           "      --intf-type <tag>           Set type of generated codes 'sa' or 'hdi', default type is 'sa'\n");
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * package:ohos.hdi.foo.v1_0
 * rootPackage:ohos.hdi
 */
std::string Options::GetRootPackage(const std::string &package) const
{
    const auto &packagePaths = GetPackagePathMap();
    for (const auto &packageRoot : packagePaths) {
        if (StringHelper::StartWith(package, packageRoot.first)) {
            return packageRoot.first;
        }
    }

    return "";
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * package:ohos.hdi.foo.v1_0
 * rootPath:./drivers/interface
 */
std::string Options::GetRootPath(const std::string &package) const
{
    const auto &packagePaths = GetPackagePathMap();
    for (const auto &packageRoot : packagePaths) {
        if (StringHelper::StartWith(package, packageRoot.first)) {
            return packageRoot.second;
        }
    }

    return "";
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * package:ohos.hdi.foo.v1_0
 * subPackage:foo.v1_0
 */
std::string Options::GetSubPackage(const std::string &package) const
{
    if (interfaceType == InterfaceType::SM ||
        interfaceType == InterfaceType::SM_UDS) {
        return package;
    }
    std::string rootPackage = GetRootPackage(package);
    if (rootPackage.empty()) {
        return package;
    }

    return package.substr(rootPackage.size() + 1);
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * package:ohos.hdi.foo.v1_0
 * packagePath:./drivers/interface/foo/v1_0
 */
std::string Options::GetPackagePath(const std::string &package) const
{
    std::string rootPackage = "";
    std::string rootPath = "";
    const auto &packagePaths = GetPackagePathMap();
    for (const auto &packageRoot : packagePaths) {
        if (StringHelper::StartWith(package, packageRoot.first)) {
            rootPackage = packageRoot.first;
            rootPath = packageRoot.second;
        }
    }

    if (rootPackage.empty()) {
        // The current path is the root path
        std::string curPath = File::AdapterPath(StringHelper::Replace(package, '.', SEPARATOR));
        return File::AdapterRealPath(curPath);
    }

    if (StringHelper::EndWith(rootPath, SEPARATOR)) {
        rootPath.pop_back();
    }

    std::string subPath = StringHelper::Replace(package.substr(rootPackage.size() + 1), '.', SEPARATOR);
    return File::AdapterPath(rootPath + "/" + subPath);
}

/*
 * -r option: -r ohos.hdi:./drivers/interface
 * import: ohos.hdi.foo.v1_0.MyTypes
 * packagePath:./drivers/interface/foo/v1_0/MyTypes.idl
 */
std::string Options::GetImportFilePath(const std::string &import, const std::string &curPath) const
{
    if (interfaceType == InterfaceType::SA && genLanguage == Language::CPP) {
        size_t index = curPath.rfind(SEPARATOR);
        if (index == std::string::npos || index == curPath.size() - 1) {
            Logger::E(TAG, "currrent path:%s, is errno", curPath.c_str());
            return "";
        }
        std::string retPath = curPath.substr(0, index + 1);
        retPath += import + ".idl";
        return File::AdapterRealPath(retPath);
    }

    size_t index = import.rfind('.');
    if (index == std::string::npos) {
        return import;
    }

    std::string dir = GetPackagePath(StringHelper::SubStr(import, 0, index));
    std::string className = import.substr(index + 1);
    return StringHelper::Format("%s%c%s.idl", dir.c_str(), SEPARATOR, className.c_str());
}
} // namespace Idl
} // namespace OHOS