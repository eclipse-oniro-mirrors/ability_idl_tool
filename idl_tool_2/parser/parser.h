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

#ifndef OHOS_IDL_PARSER_H
#define OHOS_IDL_PARSER_H

#include <memory>
#include <set>
#include <vector>
#include <regex>
#include <unordered_set>
#include <algorithm>
#include "ast/ast.h"
#include "ast/ast_array_type.h"
#include "ast/ast_set_type.h"
#include "ast/ast_attribute.h"
#include "ast/ast_interface_type.h"
#include "ast/ast_method.h"
#include "ast/ast_type.h"
#include "ast/ast_enum_type.h"
#include "ast/ast_map_type.h"
#include "ast/ast_orderedmap_type.h"
#include "ast/ast_parameter.h"
#include "ast/ast_ptr_type.h"
#include "ast/ast_rawdata_type.h"
#include "ast/ast_sequenceable_type.h"
#include "ast/ast_smq_type.h"
#include "ast/ast_struct_type.h"
#include "ast/ast_union_type.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "preprocessor/preprocessor.h"
#include "util/autoptr.h"
#include "util/light_refcount_base.h"
#include "util/logger.h"

namespace OHOS {
namespace Idl {

struct AstCompare {
    bool operator()(const AutoPtr<AST> &lhs, const AutoPtr<AST> &rhs) const
    {
        return lhs->GetMinorVer() < rhs->GetMinorVer();
    }
};

using AttrSet = std::set<Token, TokenTypeCompare>;

using AstMergeMap = std::map<std::string, std::set<AutoPtr<AST>, AstCompare>>;

class Parser {
public:
    Parser() = default;

    ~Parser() = default;

    bool Parse(const std::vector<FileDetail> &fileDetails);

    using StrAstMap = std::unordered_map<std::string, AutoPtr<AST>>;
    inline const StrAstMap &GetAllAst() const
    {
        return allAsts_;
    }

private:
    static constexpr int MIN_TRANSACTION_ID = 0x01;
    static constexpr int MAX_TRANSACTION_ID = 0x00ffffff;
    static constexpr int MIN_IPC_CAPACITY = 1;
    static constexpr int MAX_IPC_CAPACITY = 0x0001ffff;

    class Attribute : public LightRefCountBase {
    public:
        bool isOneWay = false;
        bool isCallback = false;
        bool isFull = false;
        bool isLite = false;
    };

    bool ParseOne(const std::string &sourceFile);

    bool Reset(const std::string &sourceFile);

    bool ParseFile();

    std::string ParseLicense();

    bool ParsePackage();

    bool ParseInterfaceToken();

    bool ParseKeywordWithId(TokenType expectedKeyword, const std::string& context);

    bool ParseSupportDelegator();

    bool ParseOptionStubHooks();

    bool ParseOptionParcelHooks();

    bool ParserPackageInfo(const std::string &packageName);

    bool ParseImports();

    void ParseImportInfo();

    void ParseSequenceableInfo();

    void ParseRawDataInfo();

    bool ParseTypeDecls();

    // parse attributes of type
    void ParseAttribute();

    AttrSet ParseAttributeInfo();

    bool ParseAttrUnit(AttrSet &attrs);

    void ParseAttrUnitFreezecontrol(AttrSet &attrs, Token &token);

    void ParseAttrUnitCustomMsgOption(AttrSet &attrs, Token &token);

    void ParseAttrUnitMarco(AttrSet &attrs, Token &token);

    void ParseAttrUnitIpccode(AttrSet &attrs, Token &token);

    void ParseAttrUnitIpcCapacity(AttrSet &attrs, Token &token);

    // parse interface type
    void ParseInterface(const AttrSet &attrs = {});

    AutoPtr<ASTAttr> ParseInfAttrInfo(const AttrSet &attrs);

    void CheckInterfaceAttr(const AutoPtr<ASTInterfaceType> &interface, Token token);

    void CheckIpcCodeValue(
        const AutoPtr<ASTMethod> &method, int32_t &ipcCodeValue, std::unordered_set<int32_t> &ipcCodeSet);

    void ParseInterfaceExternal(const AutoPtr<ASTInterfaceType> &interface);

    void ParseInterfaceBody(const AutoPtr<ASTInterfaceType> &interface);

    AutoPtr<ASTMethod> ParseMethod(const AutoPtr<ASTInterfaceType> &interface);

    AutoPtr<ASTType> ParseMethodReturnType();

    AutoPtr<ASTAttr> ParseMethodAttr();

    AutoPtr<ASTMethod> CreateGetVersionMethod();

    void CheckMethodAttr(const AutoPtr<ASTInterfaceType> &interface, const AutoPtr<ASTMethod> &method);

    void ParseMethodParamList(const AutoPtr<ASTMethod> &method);

    AutoPtr<ASTParameter> ParseParam();

    bool CheckParamAttr();

    void SetParamAttrVal(Token token, AutoPtr<ASTParamAttr> attr);

    AutoPtr<ASTParamAttr> ParseParamAttr();

    // parse type
    AutoPtr<ASTType> ParseType();

    AutoPtr<ASTType> ParseBasicType();

    AutoPtr<ASTType> ParseUnsignedType();

    AutoPtr<ASTType> ParseArrayType(const AutoPtr<ASTType> &elementType);

    AutoPtr<ASTType> ParseListType();

    AutoPtr<ASTType> ParseSetType();

    AutoPtr<ASTType> ParsePtrType(TokenType ptrKind);

    AutoPtr<ASTType> ParseMapType();

    AutoPtr<ASTType> ParseOrderedMapType();

    AutoPtr<ASTType> ParseSmqType();

    AutoPtr<ASTType> ParseUserDefType();

    // parse declaration of enum
    void ParseEnumDeclaration(const AttrSet &attrs = {});

    AutoPtr<ASTType> ParseEnumBaseType();

    void ParserEnumMember(const AutoPtr<ASTEnumType> &enumType);

    // parse declaration of struct
    void ParseStructDeclaration(const AttrSet &attrs = {});

    AutoPtr<ASTStructType> ParseStructParentType();

    void ParseStructMember(const AutoPtr<ASTStructType> &structType);

    // parse declaration of union
    void ParseUnionDeclaration(const AttrSet &attrs = {});

    void ParseUnionMember(const AutoPtr<ASTUnionType> &unionType);

    bool AddUnionMember(
        const AutoPtr<ASTUnionType> &unionType, const AutoPtr<ASTType> &type, const std::string &name) const;

    AutoPtr<ASTAttr> ParseUserDefTypeAttr(const AttrSet &attrs);

    // parse expression
    AutoPtr<ASTExpr> ParseExpr();

    AutoPtr<ASTExpr> ParseAndExpr();

    AutoPtr<ASTExpr> ParseXorExpr();

    AutoPtr<ASTExpr> ParseOrExpr();

    AutoPtr<ASTExpr> ParseShiftExpr();

    AutoPtr<ASTExpr> ParseAddExpr();

    AutoPtr<ASTExpr> ParseMulExpr();

    AutoPtr<ASTExpr> ParseUnaryExpr();

    AutoPtr<ASTExpr> ParsePrimaryExpr();

    AutoPtr<ASTExpr> ParseNumExpr();

    AutoPtr<ASTExpr> ParseEnumExpr();

    bool CheckNumber(const std::string& integerVal) const;

    bool CheckType(const Token &token, const AutoPtr<ASTType> &type);

    bool CheckTypeByMode(const Token &token, const AutoPtr<ASTType> &type);

    void SetAstFileType();

    bool CheckPackageName(const std::string &filePath, const std::string &packageName) const;

    bool CheckImport(const Token &token);

    void ParseInterfaceExtends(AutoPtr<ASTInterfaceType> &interface);

    void ParseExtendsInfo(AutoPtr<ASTInterfaceType> &interfaceType);

    bool CheckExtendsName(AutoPtr<ASTInterfaceType> &interfaceType, const std::string &extendsName);

    bool CheckExtendsVersion(AutoPtr<AST> extendsAst);

    bool CheckImportsVersion(AutoPtr<AST> extendsAst);

    inline static bool IsPrimitiveType(Token token)
    {
        return token.kind >= TokenType::BOOLEAN && token.kind <= TokenType::ASHMEM;
    }

    bool AddAst(const AutoPtr<AST> &ast);

    void LogError(const char *funcName, int fileLine, const std::string &message)
    {
        errors_.push_back(StringHelper::Format("[%s:%d] error:%s", funcName, fileLine, message.c_str()));
    }

    void LogError(const char *funcName, int fileLine, const Token &token, const std::string &message)
    {
        errors_.push_back(StringHelper::Format("[%s:%d] [%s] error:%s",
            funcName, fileLine, LocInfo(token).c_str(), message.c_str()));
    }

    void LogErrorBeforeToken(const char *funcName, int fileLine, const Token &token, const std::string &message)
    {
        errors_.push_back(StringHelper::Format("[%s:%d] [%s] error:%s before '%s' token",
            funcName, fileLine, LocInfo(token).c_str(), message.c_str(), token.value.c_str()));
    }

    void ShowError()
    {
        for (const auto &errMsg : errors_) {
            Logger::E(TAG, "%s", errMsg.c_str());
        }
    }

    bool PostProcess();

    bool CheckExistExtends();

    bool GetGenVersion(std::vector<size_t> &version, std::string &genPackageName);

    void GetGenNamespace(AutoPtr<ASTNamespace> &ns);

    void SortAstByName(AstMergeMap &mergeMap, StrAstMap &allAsts);

    void MergeAsts(AstMergeMap &mergeMap);

    void MergeAst(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeImport(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeInterfaceDef(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeTypes(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeSequenceableDef(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeRawDataDef(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void MergeTypeDefinitions(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst);

    void ModifyImport(StrAstMap &allAsts, std::string &genPackageName);

    void ModifyPackageNameAndVersion(StrAstMap &allAsts, std::string &genPackageName, std::vector<size_t> genVersion);

    void ModifyInterfaceNamespace(AutoPtr<ASTNamespace> &ns);

    Lexer lexer_;
    std::vector<std::string> errors_;
    AutoPtr<AST> ast_;
    StrAstMap allAsts_;
    std::string freezecontrolAttr_;
    AutoPtr<ASTEnumType> currentEnum_;
    std::string messageOption_;
    std::string macroVal_;
    std::string macroType_;
};
} // namespace Idl
} // namespace OHOS

#endif // OHOS_IDL_PARSER_H