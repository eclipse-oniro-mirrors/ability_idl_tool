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

#ifndef OHOS_IDL_OPTIONS_H
#define OHOS_IDL_OPTIONS_H

#include "util/string.h"

namespace OHOS {
namespace Idl {
class Options {
public:
    Options(int argc, char** argv)
    {
        Parse(argc, argv);
    }

    ~Options() = default;

struct Attribute {
        String hitraceTag;
        String logTag;
        String domainId;
        bool doHitrace;
        bool doLog;
    };

    bool DoShowUsage() const
    {
        return doShowUsage_;
    }

    bool DoShowVersion() const
    {
        return doShowVersion_;
    }

    bool DoCompile() const
    {
        return doCompile_;
    }

    bool DoDumpAST() const
    {
        return doDumpAST_;
    }

    bool DoDumpMetadata() const
    {
        return doDumpMetadata_;
    }

    bool DoSaveMetadata() const
    {
        return doSaveMetadata_;
    }

    bool DoGenerateCode() const
    {
        return doGenerateCode_;
    }

    bool DoHitraceState() const
    {
        return doHitrace_;
    }

    bool DoSearchKeywords() const
    {
        return doKeywords_;
    }

    bool DoLogOn() const
    {
        if (!domainId_.IsNull() && !logTag_.IsNull()) {
            return true;
        }
        return false;
    }

    bool DoIllegalParameter(const String argv) const
    {
        if (argv.IsEmpty() || argv.Equals("-t") || argv.Equals("-log-domainid") ||
            argv.Equals("-log-tag")) {
            return true;
        }
        return false;
    }

    bool DoLegalLog() const
    {
        if (targetLanguage_.Equals("cpp")) {
            if (!domainId_.IsNull() && !logTag_.IsNull()) {
                return true;
            } else if (domainId_.IsNull() && logTag_.IsNull()) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    bool HasErrors() const
    {
        return !illegalOptions_.IsEmpty() || sourceFile_.IsEmpty() || !DoLegalLog() || !doLegalParameters_;
    }

    String GetSourceFile() const
    {
        return sourceFile_;
    }

    String GetMetadataFile() const
    {
        return metadataFile_;
    }

    String GetTargetLanguage() const
    {
        return targetLanguage_;
    }

    String GetGenerationDirectory() const
    {
        return generationDirectory_;
    }

    String GetGenerateHitraceTag() const
    {
        return hitraceTag_;
    }

    String GetDomainId() const
    {
        return domainId_;
    }

    String GetLogTag() const
    {
        return logTag_;
    }

    Attribute GetAttribute() const
    {
        return attribute_;
    }

    void ShowErrors();

    void ShowVersion();

    void ShowUsage();

    void ShowWarning();

private:
    void Parse(int argc, char** argv);

    bool ParseSub(const String& option, int& i, char** argv);

    static constexpr int VERSION_MAJOR = 0;
    static constexpr int VERSION_MINOR = 1;

    String program_;
    String sourceFile_;
    String metadataFile_;
    String targetLanguage_;
    String generationDirectory_;
    String illegalOptions_;
    String hitraceTag_;
    String domainId_;
    String logTag_;
    Attribute attribute_ = {"", "", "", false, false};

    bool doShowUsage_ = false;
    bool doShowVersion_ = false;
    bool doCompile_ = false;
    bool doDumpAST_ = false;
    bool doDumpMetadata_ = false;
    bool doSaveMetadata_ = false;
    bool doGenerateCode_ = false;
    bool doHitrace_ = false;
    bool doKeywords_ = false;
    bool doLegalParameters_ = true;
};
}
}
#endif // OHOS_IDL_OPTIONS_H
