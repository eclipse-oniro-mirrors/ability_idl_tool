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

#ifndef OHOS_IDL_METADATADUMPER_H
#define OHOS_IDL_METADATADUMPER_H

#include "metadata/meta_component.h"
#include "metadata/meta_interface.h"
#include "metadata/meta_method.h"
#include "metadata/meta_namespace.h"
#include "metadata/meta_patameter.h"
#include "metadata/meta_sequenceable.h"
#include "metadata/meta_type.h"
#include "util/string_builder.h"

namespace OHOS {
namespace Idl {
class MetadataDumper {
public:
    explicit MetadataDumper(MetaComponent* mc)
        : metaComponent_(mc)
    {}

    ~MetadataDumper() = default;

    std::string Dump(const std::string& prefix);

private:
    std::string DumpMetaComponent(MetaComponent* mc, const std::string& prefix);

    void DumpMetaNamespaces(StringBuilder& sb, MetaComponent* mc, const std::string& prefix);

    void DumpMetaNamespace(StringBuilder& sb, MetaNamespace* mn, const std::string& prefix);

    void DumpMetaSequenceables(StringBuilder& sb, MetaComponent* mc, const std::string& prefix);

    void DumpMetaSequenceable(StringBuilder& sb, MetaSequenceable* mp, const std::string& prefix);

    void DumpMetaInterfaces(StringBuilder& sb, MetaComponent* mc, const std::string& prefix);

    void DumpMetaInterface(StringBuilder& sb, MetaInterface* mi, const std::string& prefix);

    void DumpMetaMethod(StringBuilder& sb, MetaMethod* mm, const std::string& prefix);

    void DumpMetaParameter(StringBuilder& sb, MetaParameter* mp, const std::string& prefix);

    std::string DumpMetaType(MetaType* mt);

    static std::string tab_;
    MetaComponent* metaComponent_;
};
} // namespace Idl
} // namespace OHOS
#endif // OHOS_IDL_METADATADUMPER_H
