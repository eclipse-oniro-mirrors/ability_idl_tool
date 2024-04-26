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

#ifndef OHOS_IDL_CODEEMITTER_H
#define OHOS_IDL_CODEEMITTER_H

#include "metadata/meta_component.h"
#include "metadata/meta_interface.h"
#include "util/light_refcount_base.h"
#include "util/string.h"

namespace OHOS {
namespace Idl {
class CodeEmitter : public LightRefCountBase {
public:
    CodeEmitter(MetaComponent* mc);

    void SetDirectory(const String& directory)
    {
        directory_ = directory;
    }

    virtual void EmitInterface() = 0;

    virtual void EmitInterfaceProxy() = 0;

    virtual void EmitInterfaceStub() = 0;

    void SetHitraceOn(const bool& hitraceOn)
    {
        hitraceOn_ = hitraceOn;
    }

    void SetLogOn(const bool& logOn)
    {
        logOn_ = logOn;
    }

    void SetHitraceTag(const String& setHitraceTag)
    {
        hitraceTag_ = setHitraceTag;
    }

    void SetLogTag(const String& logTag)
    {
        logTag_ = logTag;
    }

    void SetDomainId(const String& domainId)
    {
        domainId_ = domainId;
    }

protected:
    static const char* TAB;

    MetaComponent* metaComponent_;
    MetaInterface* metaInterface_;
    String directory_;
    String interfaceName_;
    String interfaceFullName_;
    String proxyName_;
    String proxyFullName_;
    String stubName_;
    String stubFullName_;
    String deathRecipientName_;
    String domainId_;
    String logTag_;
    String hitraceTag_;
    bool hitraceOn_ = false;
    bool logOn_ = false;
    bool enteredVector_ = false;
    bool readSequenceable_ = false;
    bool createSequenceableForOut_ = false;
};
} // namespace Idl
} // namespace OHOS
#endif // OHOS_IDL_CODEEMITTER_H
