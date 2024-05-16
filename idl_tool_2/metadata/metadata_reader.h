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

#ifndef OHOS_IDL_METADATAREADER_H
#define OHOS_IDL_METADATAREADER_H

#include <memory>
#include <cstdlib>
#include <unordered_map>
#include "util/autoptr.h"
#include "ast/ast.h"
#include "metadata/meta_component.h"

namespace OHOS {
namespace Idl {
class MetadataReader {
public:
    explicit MetadataReader(MetaComponent* mc)
        : metaComponent_(mc)
    {}

    ~MetadataReader() = default;
    static std::shared_ptr<MetaComponent> ReadMetadataFromFile(const std::string& filePath);
    std::unordered_map<std::string, AutoPtr<AST>> ReadMetadataToAst();

private:
    AutoPtr<ASTType> ReadMetaType(MetaType* type);
    void ReadMetaSequenceable(MetaSequenceable* mp);
    void ReadMetaInterface(MetaInterface* mi);
    void ReadMetaMethod(AutoPtr<ASTInterfaceType>& interface, MetaMethod* mm);
    void ReadMetaParam(AutoPtr<ASTMethod>& interface, MetaParameter* mp);
    std::string MetaTypeName(MetaType* mt);
    static std::string tag_;
    MetaComponent* metaComponent_;
    AutoPtr<AST> ast_;
};
} // namespace Idl
} // namespace OHOS
#endif // OHOS_IDL_METADATAREADER_H
