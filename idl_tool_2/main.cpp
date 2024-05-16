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

#include "codegen/code_generator.h"
#include "parser/parser.h"
#include "preprocessor/preprocessor.h"
#include "util/file.h"
#include "util/logger.h"
#include "util/options.h"
#include "hash/hash.h"
#include "metadata/metadata_builder.h"
#include "metadata/metadata_dumper.h"
#include "metadata/metadata_reader.h"
#include "metadata/metadata_serializer.h"

using namespace OHOS::Idl;

int FileParse(Parser &parser)
{
    std::vector<FileDetail> fileDetails;
    if (!Preprocessor::Preprocess(fileDetails)) {
        Logger::E(TAG, "failed to preprocess");
        return -1;
    }

    if (!parser.Parse(fileDetails)) {
        Logger::E(TAG, "failed to parse file");
        return -1;
    }

    return 0;
}

void DumpAst(const StrAstMap &allAst)
{
    Options &options = Options::GetInstance();
    if (!options.DoDumpAST()) {
        return;
    }
    for (const auto &astPair : allAst) {
        AutoPtr<AST> ast = astPair.second;
        if ((options.GetInterfaceType() == InterfaceType::SA) &&
            (ast->GetASTFileType() != ASTFileType::AST_IFACE)) {
            continue;
        }
        printf("%s\n", astPair.second->Dump("").c_str());
    }
}

int DumpMetaData(const StrAstMap &allAst)
{
    Options &options = Options::GetInstance();
    if ((options.GetInterfaceType() != InterfaceType::SA)) {
        return 0;
    }

    AutoPtr<AST> ast;
    for (const auto &astPair : allAst) {
        ast = astPair.second;
        if (ast->GetASTFileType() == ASTFileType::AST_IFACE) {
            break;
        }
    }
    MetadataBuilder builder(ast);
    std::shared_ptr<MetaComponent> metadata = builder.Build();
    if (metadata == nullptr) {
        Logger::E(TAG, "Generate metadata failed.");
        return -1;
    }

    if (options.DoDumpMetadata()) {
        MetadataDumper dumper(metadata.get());
        printf("%s", dumper.Dump("").c_str());
    }

    if (options.DoSaveMetadata()) {
        File metadataFile(options.GetMetadataFile(), File::write_);
        if (!metadataFile.IsValid()) {
            Logger::E(TAG, "Create metadata file failed.");
            return -1;
        }

        MetadataSerializer serializer(metadata.get());
        serializer.Serialize();
        uintptr_t data = serializer.GetData();
        int size = serializer.GetDataSize();

        metadataFile.WriteData(reinterpret_cast<void*>(data), size);
        metadataFile.Flush();
        metadataFile.Close();
    }
    return 0;
}

int ReadMetaData(StrAstMap &astList)
{
    Options &options = Options::GetInstance();
    if ((options.GetInterfaceType() != InterfaceType::SA) || options.DoCompile()) {
        return 0;
    }
    std::string metadataFile = Options::GetInstance().GetMetadataFile();
    std::shared_ptr<MetaComponent> metadata = MetadataReader::ReadMetadataFromFile(metadataFile);
    if (metadata == nullptr) {
        Logger::E(TAG, "Get metadata from \"%s\" failed.", metadataFile.c_str());
        return -1;
    }

    MetadataReader reader(metadata.get());
    astList = reader.ReadMetadataToAst();
    return 0;
}

int main(int argc, char **argv)
{
    Options &options = Options::GetInstance();
    if (!options.Parse(argc, argv)) {
        return -1;
    }

    if (options.DoShowUsage()) {
        options.ShowUsage();
        return 0;
    }
    if (options.DoShowVersion()) {
        options.ShowVersion();
        return 0;
    }
    if (options.HasWarning()) {
        options.ShowWarning();
    }

    if (options.DoGetHashKey()) {
        return Hash::GenHashKey() ? 0 : -1;
    }

    StrAstMap astList;
    Parser parser;
    if (options.DoCompile()) {
        if (FileParse(parser) != 0) {
            return -1;
        }

        astList = parser.GetAllAst();
        DumpAst(astList);

        if (DumpMetaData(astList) != 0) {
            return -1;
        }
    }

    if (!options.DoGenerateCode()) {
        return 0;
    }

    if (ReadMetaData(astList) != 0) {
        return -1;
    }

    if (!CodegenBuilder::GetInstance().Generate(astList)) {
        Logger::E(TAG, "failed to generate code");
        return -1;
    }
    return 0;
}