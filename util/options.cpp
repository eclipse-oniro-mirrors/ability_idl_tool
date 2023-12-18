/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "options.h"

#include <cstdio>
#include <cstring>

#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
void Options::Parse(int argc, char** argv)
{
    StringBuilder errors;
    program_ = argv[0];

    int i = 1;
    while (i < argc) {
        String option(argv[i++]);
        if (ParseSub(option, i, argv)) {
            continue;
        } else if (option.Equals("-t")) {
            doKeywords_ = true;
            if (targetLanguage_.Equals("cpp") && DoIllegalParameter(argv[i])) {
                doLegalParameters_ = false;
                return;
            }
            hitraceTag_ = argv[i++];
            doHitrace_ = true;
        } else if (option.Equals("-log-domainid")) {
            doKeywords_ = true;
            if (targetLanguage_.Equals("cpp") && DoIllegalParameter(argv[i])) {
                doLegalParameters_ = false;
                return;
            }
            domainId_ = argv[i++];
        } else if (option.Equals("-log-tag")) {
            doKeywords_ = true;
            if (targetLanguage_.Equals("cpp") && DoIllegalParameter(argv[i])) {
                doLegalParameters_ = false;
                return;
            }
            logTag_ = argv[i++];
        } else if (!option.StartsWith("-")) {
            sourceFile_ = option;
        } else {
            errors.Append(option);
            errors.Append(" ");
        }
    }

    illegalOptions_ = errors.ToString();

    attribute_.doHitrace = doHitrace_;
    attribute_.hitraceTag = hitraceTag_;
    attribute_.logTag = logTag_;
    attribute_.domainId = domainId_;
    attribute_.doLog = DoLogOn();
}

bool Options::ParseSub(const String& option, int& i, char** argv)
{
    if (option.Equals("--help")) {
        doShowUsage_ = true;
        return true;
    } else if (option.Equals("--version")) {
        doShowVersion_ = true;
        return true;
    } else if (option.Equals("-c")) {
        doCompile_ = true;
        return true;
    } else if (option.Equals("-dump-ast")) {
        doDumpAST_ = true;
        return true;
    } else if (option.Equals("-dump-metadata")) {
        doDumpMetadata_ = true;
        return true;
    } else if (option.Equals("-s")) {
        doSaveMetadata_ = true;
        metadataFile_ = argv[i++];
        return true;
    } else if (option.Equals("-gen-rust")) {
        doGenerateCode_ = true;
        targetLanguage_ = "rust";
        return true;
    } else if (option.Equals("-gen-cpp")) {
        doGenerateCode_ = true;
        targetLanguage_ = "cpp";
        return true;
    } else if (option.Equals("-gen-ts")) {
        doGenerateCode_ = true;
        targetLanguage_ = "ts";
        return true;
    } else if (option.Equals("-d")) {
        generationDirectory_ = argv[i++];
        return true;
    }
    return false;
}

void Options::ShowErrors()
{
    if (!illegalOptions_.IsEmpty()) {
        String options = illegalOptions_;
        int index;
        while ((index = options.IndexOf(' ')) != -1) {
            printf("The Option \"%s\" is illegal.\n", options.Substring(0, index).string());
            options = options.Substring(index + 1);
        }
    }
    printf("Use \"--help\" to show usage.\n");
}

void Options::ShowVersion()
{
    printf("idl %d.%d\n"
          "Copyright (c) Huawei Technologies Co., Ltd. 2022. All rights reserved.\n\n",
           VERSION_MAJOR, VERSION_MINOR);
}

void Options::ShowWarning()
{
    printf("Warning : Executed successfully, but contains invalid commands !\n");
}

void Options::ShowUsage()
{
    printf("Compile a .idl file and generate metadata, or generate Rust/C++/Ts codes from metadata.\n"
           "Usage: idl [options] file\n"
           "Options:\n"
           "  --help                    Display command line options\n"
           "  --version                 Display toolchain version information\n"
           "  -dump-ast                 Display the AST of the compiled file\n"
           "  -dump-metadata            Display the metadata generated from the compiled file\n"
           "  -c                        Compile the .idl file\n"
           "  -s <file>                 Place the metadata into <file>\n"
           "  -gen-rust                 Generate Rust codes\n"
           "  -gen-cpp                  Generate C++ codes\n"
           "  -gen-ts                   Generate Ts codes\n"
           "  -d <directory>            Place generated codes into <directory>\n"
           "  -log-domainid <domainid>  Place the service domain in <domainid>, Enable log(Pair with -log-tag)\n"
           "  -log-tag <tag>            Place the subsystem name in <tag>, Enable log(Pair with -log-domainid)\n"
           "  -t <hitrace tag>          Place the constant name from hitrace_meter.h file in <hitrace tag>\n");
}
}
}
