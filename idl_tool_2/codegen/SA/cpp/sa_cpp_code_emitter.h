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

#ifndef OHOS_IDL_SA_CPP_CODE_EMITTER_H
#define OHOS_IDL_SA_CPP_CODE_EMITTER_H

#include "codegen/SA/sa_code_emitter.h"

namespace OHOS {
namespace Idl {
class SACppCodeEmitter : public SACodeEmitter {
public:
    ~SACppCodeEmitter() override = default;

protected:
    void GetStdlibInclusions(HeaderFile::HeaderFileSet &headerFiles);

    void EmitBeginNamespace(StringBuilder &sb) const;

    void EmitEndNamespace(StringBuilder &sb) const;

    void EmitInterfaceMethodParams(AutoPtr<ASTMethod> &method, StringBuilder &sb, const std::string &prefix) const;

    void EmitWriteMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName,
        StringBuilder &sb, const std::string &prefix) const;

    void EmitReadMethodParameter(const AutoPtr<ASTParameter> &param, const std::string &parcelName,
        bool emitType, StringBuilder &sb, const std::string &prefix) const;

    void EmitInterfaceMethodCommands(StringBuilder &sb, const std::string &prefix) override;

private:
    std::string EmitCppParameter(AutoPtr<ASTParameter> &param) const;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_CPP_CODE_EMITTER_H