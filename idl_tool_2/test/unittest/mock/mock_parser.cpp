/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "parser/parser.h"
#include "parser/intf_type_check.h"

namespace OHOS {
namespace Idl {

static constexpr unsigned int RE_PACKAGE_NUM = 3;
static constexpr unsigned int RE_PACKAGE_INDEX = 0;
static constexpr unsigned int RE_PACKAGE_MAJOR_VER_INDEX = 1;
static constexpr unsigned int RE_PACKAGE_MINOR_VER_INDEX = 2;

static constexpr char RE_BIN_DIGIT[] = "0[b][0|1]+";        // binary digit
static constexpr char RE_OCT_DIGIT[] = "0[0-7]+";           // octal digit
static constexpr char RE_DEC_DIGIT[] = "[0-9]+";            // decimal digit
static constexpr char RE_HEX_DIFIT[] = "0[xX][0-9a-fA-F]+"; // hexadecimal digit
static constexpr char RE_DIGIT_SUFFIX[] = "(u|l|ll|ul|ull|)$";
static constexpr char RE_IDENTIFIER[] = "[a-zA-Z_][a-zA-Z0-9_]*";

static const std::regex RE_PACKAGE(std::string(RE_IDENTIFIER) + "(?:\\." + std::string(RE_IDENTIFIER) +
    ")*\\.[V|v](" + std::string(RE_DEC_DIGIT) + ")_(" + std::string(RE_DEC_DIGIT) + ")");

static const std::regex RE_PACKAGE_OR_IMPORT_SM(std::string(RE_IDENTIFIER) +
    "(?:\\." + std::string(RE_IDENTIFIER) + ")*");

static const std::regex RE_PACKAGE_OR_IMPORT_SA(
    "(?:\\.\\./)*" +                                        // matches zero or more relative path segments ../
    std::string(RE_IDENTIFIER) +                            // matches the first identifier
    "(?:[\\.\\/]" + std::string(RE_IDENTIFIER) + ")*");     // matches additional identifiers, separated by . or /

static const std::regex RE_IMPORT(std::string(RE_IDENTIFIER) + "(?:\\." + std::string(RE_IDENTIFIER) +
    ")*\\.[V|v]" + std::string(RE_DEC_DIGIT) + "_" + std::string(RE_DEC_DIGIT) + "." + std::string(RE_IDENTIFIER));
static const std::regex RE_BIN_NUM(std::string(RE_BIN_DIGIT) + std::string(RE_DIGIT_SUFFIX),
    std::regex_constants::icase);
static const std::regex RE_OCT_NUM(std::string(RE_OCT_DIGIT) + std::string(RE_DIGIT_SUFFIX),
    std::regex_constants::icase);
static const std::regex RE_DEC_NUM(std::string(RE_DEC_DIGIT) + std::string(RE_DIGIT_SUFFIX),
    std::regex_constants::icase);
static const std::regex RE_HEX_NUM(std::string(RE_HEX_DIFIT) + std::string(RE_DIGIT_SUFFIX),
    std::regex_constants::icase);

bool Parser::Parse(const std::vector<FileDetail> &fileDetails)
{
    for (const auto &fileDetail : fileDetails) {
        if (!ParseOne(fileDetail.filePath_)) {
            return false;
        }
    }

    return PostProcess();
}

bool Parser::ParseOne(const std::string &sourceFile)
{
    if (!Reset(sourceFile)) {
        return false;
    }

    bool ret = ParseFile();
    IntfTypeChecker checker(ast_);
    ret = checker.CheckIntegrity() && ret;
    ret = AddAst(ast_) && ret;
    if (!ret || !errors_.empty()) {
        ShowError();
        return false;
    }

    return true;
}

bool Parser::Reset(const std::string &sourceFile)
{
    if (!lexer_.Reset(sourceFile)) {
        Logger::E(TAG, "Fail to open file '%s'.", sourceFile.c_str());
        return false;
    }

    errors_.clear();
    ast_ = nullptr;
    return true;
}

bool Parser::ParseFile()
{
    ast_ = new AST();
    ast_->SetIdlFile(lexer_.GetFilePath());
    ast_->SetLicense(ParseLicense());

    TokenType tokenKind;
    bool ret = true;
    while (((tokenKind = lexer_.PeekToken().kind) != TokenType::END_OF_FILE) && ret) {
        switch (tokenKind) {
            case TokenType::PACKAGE:
                ret = ParsePackage() && ret;
                continue;
            case TokenType::INTERFACE_TOKEN:
                ret = ParseInterfaceToken() && ret;
                continue;
            case TokenType::SUPPORT_DELEGATOR:
                ret = ParseSupportDelegator() && ret;
                continue;
            case TokenType::OPTION_STUB_HOOKS:
                ret = ParseOptionStubHooks() && ret;
                continue;
            case TokenType::OPTION_PARCEL_HOOKS:
                ret = ParseOptionParcelHooks() && ret;
                continue;
            case TokenType::OPTION_MACRO_HILOG:
                ret = ParseOptionMacroHilog() && ret;
                continue;
            case TokenType::OPTION_USING_HOOKS:
                ret = ParseOptionUsingHooks() && ret;
                continue;
            case TokenType::IMPORT:
            case TokenType::SEQ:
            case TokenType::RAWDATA:
                ret = ParseImports() && ret;
                continue;
            default:
                ret = ParseTypeDecls() && ret;
        }
    }

    SetAstFileType();
    return ret;
}

std::string Parser::ParseLicense()
{
    Token token = lexer_.PeekToken(false);
    if (token.kind == TokenType::COMMENT_BLOCK) {
        lexer_.GetToken(false);
        return token.value;
    }

    return std::string("");
}

bool Parser::ParsePackage()
{
    // package
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::PACKAGE) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected 'package'"));
        return false;
    }
    lexer_.GetToken();

    // package name
    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected name of package"));
        lexer_.SkipToken(TokenType::SEMICOLON);
        return false;
    }
    std::string packageName = token.value;
    lexer_.GetToken();

    // expect symbol ";"
    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ';'"));
        return false;
    }
    lexer_.GetToken();

    InterfaceType interfaceType = Options::GetInstance().GetInterfaceType();
    if (packageName.empty()) {
        LogError(__func__, __LINE__, std::string("package name is not expected."));
        return false;
    } else if (interfaceType != InterfaceType::SA && !CheckPackageName(lexer_.GetFilePath(), packageName)) {
        LogError(__func__, __LINE__, StringHelper::Format(
            "package name '%s' does not match file apth '%s'.", packageName.c_str(), lexer_.GetFilePath().c_str()));
        return false;
    }

    if (!ParserPackageInfo(packageName)) {
        LogError(__func__, __LINE__, StringHelper::Format("parse package '%s' infomation failed.",
            packageName.c_str()));
        return false;
    }

    return true;
}

bool Parser::ParserPackageInfo(const std::string &packageName)
{
    std::cmatch result;
    try {
        if (Options::GetInstance().GetInterfaceType() == InterfaceType::HDI) {
            if (!std::regex_match(packageName.c_str(), result, RE_PACKAGE) || (result.size() < RE_PACKAGE_NUM)) {
                return false;
            }

            ast_->SetPackageName(result.str(RE_PACKAGE_INDEX));
            size_t majorVersion = std::stoul(result.str(RE_PACKAGE_MAJOR_VER_INDEX));
            size_t minorVersion = std::stoul(result.str(RE_PACKAGE_MINOR_VER_INDEX));
            ast_->SetVersion(majorVersion, minorVersion);
        } else if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
            if (!std::regex_match(packageName.c_str(), result, RE_PACKAGE_OR_IMPORT_SA)) {
                return false;
            }
            ast_->SetPackageName(result.str(RE_PACKAGE_INDEX));
        } else {
            if (!std::regex_match(packageName.c_str(), result, RE_PACKAGE_OR_IMPORT_SM)) {
                return false;
            }
            ast_->SetPackageName(result.str(RE_PACKAGE_INDEX));
        }
    } catch (...) {
        LogError(__func__, __LINE__, StringHelper::Format(
            "Unknown error while parsing package '%s'", packageName.c_str()));
        return false;
    }

    return true;
}

bool Parser::ParseInterfaceToken()
{
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::INTERFACE_TOKEN) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected 'interface token'"));
        return false;
    }
    lexer_.GetToken();

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected name of interface_token before '%s' token",
        token.value.c_str()));
        lexer_.SkipToken(TokenType::SEMICOLON);
        return false;
    }
    std::string interfaceToken = token.value;
    lexer_.GetToken();

    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected ';' before '%s' token",
        token.value.c_str()));
        return false;
    }
    lexer_.GetToken();

    if (interfaceToken.empty()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("interface_token name is not expected."));
        return false;
    }

    ast_->SetInterfaceToken(interfaceToken);
    return true;
}

bool Parser::ParseKeywordWithId(TokenType expectedKeyword, const std::string& context)
{
    Token token = lexer_.PeekToken();
    if (token.kind != expectedKeyword) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected '%s'", context.c_str()));
        return false;
    }
    lexer_.GetToken();

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogError(__func__, __LINE__, token, StringHelper::Format(
            "expected name of %s before '%s' token", context.c_str(), token.value.c_str()));
        lexer_.SkipToken(TokenType::SEMICOLON);
        return false;
    }
    std::string value = token.value;
    lexer_.GetToken();

    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected ';' before '%s' token",
        token.value.c_str()));
        return false;
    }
    lexer_.GetToken();

    if (value.empty()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("%s name is not expected.", context.c_str()));
        return false;
    }

    if (context == "support_delegator") {
        ast_->SetSupportDelegator(value);
    } else if (context == "option_stub_hooks") {
        ast_->SetOptionStubHooks(value);
    } else if (context == "option_parcel_hooks") {
        ast_->SetOptionParcelHooks(value);
    } else if (context == "option_macro_hilog") {
        ast_->SetOptionMacroHilog(value);
    } else if (context == "option_using_hooks") {
        ast_->SetOptionUsingHooks(value);
    }
    return true;
}

bool Parser::ParseSupportDelegator()
{
    return ParseKeywordWithId(TokenType::SUPPORT_DELEGATOR, "support_delegator");
}

bool Parser::ParseOptionStubHooks()
{
    return ParseKeywordWithId(TokenType::OPTION_STUB_HOOKS, "option_stub_hooks");
}

bool Parser::ParseOptionParcelHooks()
{
    return ParseKeywordWithId(TokenType::OPTION_PARCEL_HOOKS, "option_parcel_hooks");
}

bool Parser::ParseOptionMacroHilog()
{
    return ParseKeywordWithId(TokenType::OPTION_MACRO_HILOG, "option_macro_hilog");
}

bool Parser::ParseOptionUsingHooks()
{
    return ParseKeywordWithId(TokenType::OPTION_USING_HOOKS, "option_using_hooks");
}

bool Parser::ParseImports()
{
    Token token = lexer_.PeekToken();
    TokenType kind = token.kind;
    lexer_.GetToken();

    // name
    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected identifier"));
        lexer_.SkipToken(TokenType::SEMICOLON);
        token = lexer_.PeekToken();
        return false;
    }

    if (kind == TokenType::IMPORT) {
        ParseImportInfo();
    } else if (kind == TokenType::SEQ) {
        ParseSequenceableInfo();
    } else if (kind == TokenType::RAWDATA) {
        ParseRawDataInfo();
    }
    lexer_.GetToken();

    // expect symbol ";"
    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogError(__func__, __LINE__, token, StringHelper::Format("expected ';' before '%s'.", token.value.c_str()));
        return false;
    }
    lexer_.GetToken();

    return true;
}

void Parser::ParseImportInfo()
{
    Token token = lexer_.PeekToken();
    std::string importName = token.value;
    if (importName.empty()) {
        LogError(__func__, __LINE__, token, std::string("import name is empty"));
        return;
    }

    if (!CheckImport(token)) {
        LogError(__func__, __LINE__, token, std::string("import name is illegal"));
        return;
    }

    auto iter = allAsts_.find(importName);
    AutoPtr<AST> importAst = (iter != allAsts_.end()) ? iter->second : nullptr;

    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() == InterfaceType::SA && options.GetLanguage() == Language::CPP) {
#ifdef __MINGW32__
        std::replace(importName.begin(), importName.end(), '/', '\\');
#endif
        ast_->AddImportName(importName);
        std::string idlFilePath = token.location.filePath;
        size_t index = idlFilePath.rfind(SEPARATOR);
        idlFilePath = File::CanonicalPath(idlFilePath.substr(0, index + 1) + importName + ".idl");
        importAst = nullptr;
        for (iter = allAsts_.begin(); iter != allAsts_.end(); ++iter) {
            AutoPtr<AST> curAst = iter->second;
            std::string idlFile = curAst->GetIdlFile();
            if (idlFile == idlFilePath) {
                importAst = curAst;
            }
        }
    }

    if (importAst == nullptr) {
        LogError(__func__, __LINE__, token,
            StringHelper::Format("can not find idl file from import name '%s'", importName.c_str()));
        return;
    }

    if (options.GetInterfaceType() == InterfaceType::SA && !CheckImportsVersion(importAst)) {
        LogError(__func__, __LINE__, token,
            std::string("extends import version must less than current import version"));
        return;
    }

    if (!ast_->AddImport(importAst)) {
        LogError(__func__, __LINE__, token, StringHelper::Format("multiple import of '%s'", importName.c_str()));
    }
}

void Parser::ParseSequenceableInfo()
{
    Token token = lexer_.PeekToken();
    std::string seqName = token.value;
    if (seqName.empty()) {
        LogError(__func__, __LINE__, token, std::string("sequenceable name is empty"));
        return;
    }

    AutoPtr<ASTSequenceableType> seqType = new ASTSequenceableType();
    size_t index = seqName.rfind('.');
    if (index != std::string::npos) {
        seqType->SetName(seqName.substr(index + 1));
        seqType->SetNamespace(ast_->ParseNamespace(seqName));
    } else {
        seqType->SetName(seqName);
        seqType->SetNamespace(ast_->ParseNamespace(""));
    }

    AutoPtr<AST> seqAst = new AST();
    seqAst->SetFullName(seqName);
    seqAst->AddSequenceableDef(seqType);
    seqAst->SetAStFileType(ASTFileType::AST_SEQUENCEABLE);
    ast_->AddImport(seqAst);
    ast_->AddSequenceableDef(seqType);
    AddAst(seqAst);
}

void Parser::ParseRawDataInfo()
{
    Token token = lexer_.PeekToken();
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Not support rawdata"));
        lexer_.GetToken();
        return;
    }

    std::string rawdataName = token.value;
    if (rawdataName.empty()) {
        LogError(__func__, __LINE__, token, std::string("rawdata name is empty"));
        return;
    }

    AutoPtr<ASTRawDataType> rawdataType = new ASTRawDataType();
    size_t index = rawdataName.rfind('.');
    if (index != std::string::npos) {
        rawdataType->SetName(rawdataName.substr(index + 1));
        rawdataType->SetNamespace(ast_->ParseNamespace(rawdataName));
    } else {
        rawdataType->SetName(rawdataName);
        rawdataType->SetNamespace(ast_->ParseNamespace(""));
    }

    AutoPtr<AST> rawdataAst = new AST();
    rawdataAst->SetFullName(rawdataName);
    rawdataAst->AddRawDataDef(rawdataType);
    rawdataAst->SetAStFileType(ASTFileType::AST_RAWDATA);
    ast_->AddImport(rawdataAst);
    ast_->AddRawDataDef(rawdataType);
    AddAst(rawdataAst);
}

bool Parser::ParseTypeDecls()
{
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::BRACKETS_LEFT:
            ParseAttribute();
            break;
        case TokenType::INTERFACE:
            ParseInterface();
            break;
        case TokenType::ENUM:
            ParseEnumDeclaration();
            break;
        case TokenType::STRUCT:
            ParseStructDeclaration();
            break;
        case TokenType::UNION:
            ParseUnionDeclaration();
            break;
        default:
            LogError(__func__, __LINE__, token, StringHelper::Format("'%s' is not expected", token.value.c_str()));
            lexer_.SkipToken(TokenType::SEMICOLON);
            return false;
    }

    return true;
}

void Parser::ParseAttribute()
{
    AttrSet attrs = ParseAttributeInfo();
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::INTERFACE:
            ParseInterface(attrs);
            break;
        case TokenType::ENUM:
            ParseEnumDeclaration(attrs);
            break;
        case TokenType::STRUCT:
            ParseStructDeclaration(attrs);
            break;
        case TokenType::UNION:
            ParseUnionDeclaration(attrs);
            break;
        default:
            LogError(__func__, __LINE__, token, StringHelper::Format("'%s' is not expected", token.value.c_str()));
            lexer_.SkipToken(token.kind);
            break;
    }
}

AttrSet Parser::ParseAttributeInfo()
{
    AttrSet attrs;
    // [
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '['"));
        lexer_.SkipToken(token.kind);
        return attrs;
    }
    lexer_.GetToken();

    // attrunit
    token = lexer_.PeekToken();
    while (token.kind != TokenType::BRACKETS_RIGHT && token.kind != TokenType::END_OF_FILE) {
        token = lexer_.PeekToken();
        if (attrs.find(token) != attrs.end()) {
            LogError(__func__, __LINE__, token, StringHelper::Format("Duplicate declared attributes '%s'",
                token.value.c_str()));
            return attrs;
        }

        if (!ParseAttrUnit(attrs)) {
            return attrs;
        }
        // expect symbol ","
        token = lexer_.PeekToken();
        if (token.kind == TokenType::COMMA) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
            continue;
        }
        // ]
        if (token.kind == TokenType::BRACKETS_RIGHT) {
            lexer_.GetToken();
        } else {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ',' or ']'"));
            lexer_.SkipToken(TokenType::BRACKETS_RIGHT);
        }
        break;
    }

    return attrs;
}

bool Parser::ParseAttrUnit(AttrSet &attrs)
{
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::FULL:
        case TokenType::LITE:
        case TokenType::MINI:
        case TokenType::CALLBACK:
        case TokenType::ONEWAY: {
            attrs.insert(token);
            lexer_.GetToken();
            return true;
        }
        case TokenType::CACHEABLE: {
            if (!lexer_.ReadCacheableTime(token)) {
                LogError(__func__, __LINE__, token, StringHelper::Format("Cacheable time parse failed"));
            }
            attrs.insert(token);
            lexer_.GetToken();
            return true;
        }
        case TokenType::FREEZECONTROL: {
            ParseAttrUnitFreezecontrol(attrs, token);
            return true;
        }
        case TokenType::IPCCODE: {
            ParseAttrUnitIpccode(attrs, token);
            return true;
        }
        case TokenType::IPC_IN_CAPACITY:
        case TokenType::IPC_OUT_CAPACITY: {
            ParseAttrUnitIpcCapacity(attrs, token);
            return true;
        }
        case TokenType::CUSTOM_MSG_OPTION: {
            ParseAttrUnitCustomMsgOption(attrs, token);
            return true;
        }
        case TokenType::MACRODEF:
        case TokenType::MACRONDEF: {
            ParseAttrUnitMarco(attrs, token);
            return true;
        }
        default:
            LogError(__func__, __LINE__, token, StringHelper::Format("'%s' is a illegal attribute",
                token.value.c_str()));
            lexer_.SkipToken(TokenType::BRACKETS_RIGHT);
            return false;
    }
}

void Parser::ParseAttrUnitMarco(AttrSet &attrs, Token &token)
{
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Not support macrodef"));
        lexer_.GetToken();
        return;
    }

    macroType_ = "ifdef";
    if (token.kind == TokenType::MACRONDEF) {
        macroType_ = "ifndef";
    }
    attrs.insert(token);
    lexer_.GetToken();
    token = lexer_.PeekToken();
    macroVal_ = "";
    if (token.kind == TokenType::BRACKETS_RIGHT) {
        LogError(__func__, __LINE__, token, StringHelper::Format("macrodef/marcorndef attr cannot be empty"));
    } else if (token.kind == TokenType::ID) {
        macroVal_ = token.value;
    }
    lexer_.GetToken();
}

void Parser::ParseAttrUnitFreezecontrol(AttrSet &attrs, Token &token)
{
    attrs.insert(token);
    lexer_.GetToken();
    token = lexer_.PeekToken();
    if (token.value == "]") {
        LogError(__func__, __LINE__, token, StringHelper::Format("freezecontrol attr cannot be empty"));
    } else if (token.kind == TokenType::ID) {
        freezecontrolAttr_ = token.value;
    }
    lexer_.GetToken();
}

void Parser::ParseAttrUnitCustomMsgOption(AttrSet &attrs, Token &token)
{
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Not support customMsgOption"));
        lexer_.GetToken();
        return;
    }

    attrs.insert(token);
    lexer_.GetToken();
    token = lexer_.PeekToken();
    messageOption_ = "";
    while (token.kind != TokenType::BRACKETS_RIGHT && token.kind != TokenType::COMMA &&
           token.kind != TokenType::SEMICOLON && token.kind != TokenType::END_OF_FILE) {
        if (token.value == "and" || token.kind == TokenType::AND || token.kind == TokenType::OR) {
            messageOption_ += " " + token.value + " ";
        } else {
            messageOption_ += token.value;
        }
        lexer_.GetToken();
        token = lexer_.PeekToken();
    }
}

void Parser::ParseAttrUnitIpccode(AttrSet &attrs, Token &token)
{
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Not support ipccode"));
        lexer_.GetToken();
        return;
    }

    Token tokenTmp = token;
    lexer_.GetToken();
    token = lexer_.PeekToken();
    if (token.value == "]" || token.value == "," || token.value.empty()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Ipccode attr cannot be empty"));
        return;
    }

    size_t end = 0;
    int ipcCodeValue = 0;
    int base = 10; // 10: decimal
    if (token.value.size() >= sizeof("0x") && token.value[0] == '0' && tolower(token.value[1]) == 'x') {
        base = 16; // 16: hexadecimal
    }
    try {
        ipcCodeValue = std::stoi(token.value, &end, base);
    } catch (...) {
        end = 0;
    }
    if (end != token.value.size()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("%s is illegal ipccode value", token.value.c_str()));
    } else {
        if (ipcCodeValue < 0 || ipcCodeValue > MAX_TRANSACTION_ID) {
            LogError(__func__, __LINE__, token, StringHelper::Format("ipccode %d is out of range [%d, %d]",
                ipcCodeValue, MIN_TRANSACTION_ID, MAX_TRANSACTION_ID));
        } else {
            tokenTmp.value = std::to_string(ipcCodeValue);
            attrs.insert(tokenTmp);
        }
    }
    lexer_.GetToken();
}

void Parser::ParseAttrUnitIpcCapacity(AttrSet &attrs, Token &token)
{
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        LogError(__func__, __LINE__, token, StringHelper::Format("Not support ipc capacity"));
        lexer_.GetToken();
        return;
    }

    Token tokenTmp = token;
    lexer_.GetToken();
    token = lexer_.PeekToken();
    if (token.value == "]" || token.value == "," || token.value.empty()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("ipc capacity attr cannot be empty"));
        return;
    }

    size_t end = 0;
    int capacity = 0;
    try {
        capacity = std::stoi(token.value, &end);
    } catch (...) {
        end = 0;
    }
    if (end != token.value.size()) {
        LogError(__func__, __LINE__, token,
            StringHelper::Format("%s is illegal ipc capacity value", token.value.c_str()));
    } else {
        if (capacity < MIN_IPC_CAPACITY || capacity > MAX_IPC_CAPACITY) {
            LogError(__func__, __LINE__, token, StringHelper::Format("ipc capacity %d is out of range [%d, %d]",
                capacity, MIN_IPC_CAPACITY, MAX_IPC_CAPACITY));
        } else {
            tokenTmp.value = std::to_string(capacity);
            attrs.insert(tokenTmp);
        }
    }
    lexer_.GetToken();
}

void Parser::ParseInterface(const AttrSet &attrs)
{
    // The priority of the "interface" namespace is higher than that of the "package".
    Options &options = Options::GetInstance();
    lexer_.GetToken();
    Token token = lexer_.PeekToken();
    size_t index = token.value.rfind('.');
    if (options.GetInterfaceType() == InterfaceType::SA && options.GetLanguage() == Language::CPP) {
        for (const auto &attr : attrs) {
            if (attr.kind == TokenType::CALLBACK && index != std::string::npos && index != 0) {
                ParserPackageInfo(token.value.substr(0, index));
                break;
            }
        }
    }

    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType;
    AutoPtr<ASTAttr> astAttr = ParseInfAttrInfo(attrs);
    interfaceType->SetAttribute(astAttr);

    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected interface name"));
    } else {
        if (index != std::string::npos) {
            interfaceType->SetName(StringHelper::SubStr(token.value, index + 1));
            interfaceType->SetNamespace(ast_->ParseNamespace(token.value));
        } else {
            interfaceType->SetName(token.value);
            interfaceType->SetNamespace(ast_->ParseNamespace(ast_->GetFullName()));
        }
        interfaceType->SetLicense(ast_->GetLicense());
        lexer_.GetToken();
    }

    CheckInterfaceAttr(interfaceType, token);
    ParseInterfaceExtends(interfaceType);
    token = lexer_.PeekToken();
    if (token.kind == TokenType::SEMICOLON) {
        ParseInterfaceExternal(interfaceType);
    } else {
        if (interfaceType->GetName() != ast_->GetName()) {
            LogError(__func__, __LINE__, token, StringHelper::Format(
                "interface name '%s' is not equal idl file name", interfaceType->GetName().c_str()));
        }
        ParseInterfaceBody(interfaceType);
    }
    interfaceType->SetVersion(ast_->GetMajorVer(), ast_->GetMinorVer());
    ast_->AddInterfaceDef(interfaceType);
}

AutoPtr<ASTAttr> Parser::ParseInfAttrInfo(const AttrSet &attrs)
{
    AutoPtr<ASTAttr> infAttr = new ASTAttr();
    InterfaceType interfaceType = Options::GetInstance().GetInterfaceType();
    for (const auto &attr : attrs) {
        switch (attr.kind) {
            case TokenType::FULL:
                infAttr->SetValue(ASTAttr::FULL);
                break;
            case TokenType::LITE:
                infAttr->SetValue(ASTAttr::LITE);
                break;
            case TokenType::MINI:
                infAttr->SetValue(ASTAttr::MINI);
                break;
            case TokenType::CALLBACK:
                infAttr->SetValue(ASTAttr::CALLBACK);
                break;
            case TokenType::ONEWAY:
                infAttr->SetValue(ASTAttr::ONEWAY);
                break;
            default:
                LogError(__func__, __LINE__, attr, std::string("illegal attribute of interface"));
                break;
        }
    }

    if ((interfaceType == InterfaceType::HDI) &&
        (!infAttr->HasValue(ASTAttr::FULL) && !infAttr->HasValue(ASTAttr::LITE) &&
        !infAttr->HasValue(ASTAttr::MINI))) {
        infAttr->SetValue(ASTAttr::FULL | ASTAttr::LITE | ASTAttr::MINI);
    }

    return infAttr;
}

void Parser::CheckInterfaceAttr(const AutoPtr<ASTInterfaceType> &interface, Token token)
{
    bool ret = true;
    std::string systemName;
    switch (Options::GetInstance().GetSystemLevel()) {
        case SystemLevel::FULL:
            systemName = "full";
            ret = interface->IsFull();
            break;
        case SystemLevel::LITE:
            systemName = "lite";
            ret = interface->IsLite();
            break;
        case SystemLevel::MINI:
            systemName = "mini";
            ret = interface->IsMini();
            break;
        default:
            break;
    }

    if (!ret) {
        LogError(__func__, __LINE__, token, StringHelper::Format("the system option is '%s', but the '%s' interface "
            "has no '%s' attribute", systemName.c_str(), interface->GetName().c_str(), systemName.c_str()));
    }
}

void Parser::CheckIpcCodeValue(
    const AutoPtr<ASTMethod> &method, int32_t &ipcCodeValue, std::unordered_set<int32_t> &ipcCodeSet)
{
    Options &options = Options::GetInstance();
    if (options.GetInterfaceType() != InterfaceType::SA || options.GetLanguage() != Language::CPP) {
        return;
    }
    if (method->HasIpcCode()) {
        try {
            ipcCodeValue = std::stoi(method->GetIpcCodeStr());
        } catch (...) {
            LogError(__func__, __LINE__, StringHelper::Format(
                "Unknown error while converting ipccode string '%s'", method->GetIpcCodeStr().c_str()));
            return;
        }
    }
    if (ipcCodeValue > MAX_TRANSACTION_ID) {
        LogError(__func__, __LINE__, StringHelper::Format("the ipccode %d is out of range [%d, %d]",
            ipcCodeValue, MIN_TRANSACTION_ID, MAX_TRANSACTION_ID));
    } else if (ipcCodeSet.find(ipcCodeValue) != ipcCodeSet.end()) {
        LogError(__func__, __LINE__, StringHelper::Format("the ipccode %d is duplicated", ipcCodeValue));
    } else {
        ipcCodeSet.insert(ipcCodeValue);
    }
    method->SetIpcCode(ipcCodeValue);
    ipcCodeValue++;
}

void Parser::ParseInterfaceExternal(const AutoPtr<ASTInterfaceType> &interface)
{
    Token token = lexer_.PeekToken();
    lexer_.GetToken();
    if (interface->IsAttributeNone()) {
        interface->SetExternal(true);
    } else {
        LogError(__func__, __LINE__, token, std::string("interface forward declaration should not have attribute."));
    }
}

void Parser::ParseInterfaceBody(const AutoPtr<ASTInterfaceType> &interface)
{
    Token token = lexer_.PeekToken();
    // expect symbol "{" or ";"
    if (token.kind != TokenType::BRACES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '{'"));
    } else {
        lexer_.GetToken();
    }

    // parse method
    token = lexer_.PeekToken();
    int32_t ipcCodeValue = MIN_TRANSACTION_ID;
    std::unordered_set<int32_t> ipcCodeSet;
    while (token.kind != TokenType::BRACES_RIGHT && token.kind != TokenType::END_OF_FILE) {
        AutoPtr<ASTMethod> method = ParseMethod(interface);
        interface->AddMethod(method);
        token = lexer_.PeekToken();
        CheckIpcCodeValue(method, ipcCodeValue, ipcCodeSet);
    }

    // expect symbol "}"
    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '}'"));
    } else {
        lexer_.GetToken();
    }

    // expect symbol ";"
    token = lexer_.PeekToken();
    if (token.kind == TokenType::SEMICOLON) {
        lexer_.GetToken();
    }

    interface->AddVersionMethod(CreateGetVersionMethod());
}

AutoPtr<ASTMethod> Parser::ParseMethod(const AutoPtr<ASTInterfaceType> &interface)
{
    freezecontrolAttr_ = "";
    messageOption_ = "";
    macroVal_ = "";
    macroType_ = "";
    AutoPtr<ASTMethod> method = new ASTMethod();
    AutoPtr<ASTAttr> methodAttr = ParseMethodAttr();
    method->SetAttribute(methodAttr);
    method->SetCacheable(methodAttr);
    method->SetIpcCode(methodAttr);
    method->SetIpcInCapacity(methodAttr);
    method->SetIpcOutCapacity(methodAttr);
    method->SetReturnType(ParseMethodReturnType());

    // parser method name
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected method name"));
    } else {
        method->SetName(token.value);
        lexer_.GetToken();
    }

    CheckMethodAttr(interface, method);
    // (param1, param2, ...)
    ParseMethodParamList(method);

    // parse symbol ";"
    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ';'"));
    } else {
        lexer_.GetToken();
    }

    size_t methodsCount = interface->GetMethodNumber() + 1;
    AutoPtr<ASTInterfaceType> extInterface = interface->GetExtendsInterface();
    while (extInterface != nullptr) {
        methodsCount += extInterface->GetMethodNumber();
        extInterface = extInterface->GetExtendsInterface();
    }
    method->SetCmdId(methodsCount);
    method->CheckOverload(interface);

    if (!freezecontrolAttr_.empty()) {
        method->SetFreezeControlReason(freezecontrolAttr_);
    }
    if (!messageOption_.empty() && IntfTypeChecker::CheckMessageOption(messageOption_)) {
        method->SetMessageOption(messageOption_);
    }
    if (!macroVal_.empty() && !macroType_.empty()) {
        method->SetMacroVal(macroVal_);
        method->SetMacroType(macroType_);
    }
    return method;
}

AutoPtr<ASTType> Parser::ParseMethodReturnType()
{
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::HDI) {
        return nullptr;
    }
    Token token = lexer_.PeekToken();
    if (token.kind == TokenType::ID && ast_->FindType(token.value) == nullptr) {
        return nullptr;
    }
    // parse method return type, maybe not exist
    if (IntfTypeChecker::CheckBasicType(token) || IntfTypeChecker::CheckUserDefType(token) ||
        token.kind == TokenType::LIST || token.kind == TokenType::SET || token.kind == TokenType::MAP ||
        token.kind == TokenType::ORDEREDMAP || token.kind == TokenType::SMQ ||
        token.kind == TokenType::SHAREDPTR || token.kind == TokenType::UNIQUEPTR || token.kind == TokenType::SPTR ||
        token.kind == TokenType::NULL_SHAREDPTR || token.kind == TokenType::NULL_UNIQUEPTR ||
        token.kind == TokenType::NULL_SPTR) {
        return ParseType();
    }
    return nullptr;
}

AutoPtr<ASTAttr> Parser::ParseMethodAttr()
{
    if (lexer_.PeekToken().kind != TokenType::BRACKETS_LEFT) {
        return new ASTAttr();
    }

    AttrSet attrs = ParseAttributeInfo();
    AutoPtr<ASTAttr> methodAttr = new ASTAttr();

    for (const auto &attr : attrs) {
        switch (attr.kind) {
            case TokenType::FULL:
                methodAttr->SetValue(ASTAttr::FULL);
                break;
            case TokenType::LITE:
                methodAttr->SetValue(ASTAttr::LITE);
                break;
            case TokenType::MINI:
                methodAttr->SetValue(ASTAttr::MINI);
                break;
            case TokenType::ONEWAY:
                methodAttr->SetValue(ASTAttr::ONEWAY);
                break;
            case TokenType::CACHEABLE:
                methodAttr->SetValue(ASTAttr::CACHEABLE);
                methodAttr->SetCacheableTimeString(attr.value);
                break;
            case TokenType::FREEZECONTROL:
                methodAttr->SetValue(ASTAttr::FREEZECONTROL);
                break;
            case TokenType::IPCCODE:
                methodAttr->SetValue(ASTAttr::IPCCODE);
                methodAttr->SetIpcCodeStr(attr.value);
                break;
            case TokenType::IPC_IN_CAPACITY:
                methodAttr->SetValue(ASTAttr::IPC_IN_CAPACITY);
                methodAttr->SetIpcInCapacity(attr.value);
                break;
            case TokenType::IPC_OUT_CAPACITY:
                methodAttr->SetValue(ASTAttr::IPC_OUT_CAPACITY);
                methodAttr->SetIpcOutCapacity(attr.value);
                break;
            case TokenType::CUSTOM_MSG_OPTION:
                methodAttr->SetValue(ASTAttr::CUSTOM_MSG_OPTION);
                break;
            case TokenType::MACRODEF:
            case TokenType::MACRONDEF:
                methodAttr->SetValue(ASTAttr::MACRO);
                break;
            default:
                LogError(__func__, __LINE__, attr, std::string("illegal attribute of interface"));
                break;
        }
    }

    return methodAttr;
}

AutoPtr<ASTMethod> Parser::CreateGetVersionMethod()
{
    AutoPtr<ASTMethod> method = new ASTMethod();
    method->SetName("GetVersion");

    AutoPtr<ASTType> type = ast_->FindType("unsigned int");
    if (type == nullptr) {
        type = new ASTUintType();
    }

    method->AddParameter(new ASTParameter("majorVer", ASTParamAttr::PARAM_OUT, type));
    method->AddParameter(new ASTParameter("minorVer", ASTParamAttr::PARAM_OUT, type));
    return method;
}

void Parser::CheckMethodAttr(const AutoPtr<ASTInterfaceType> &interface, const AutoPtr<ASTMethod> &method)
{
    // if the attribute of method is empty, the default value is attribute of interface
    if (!method->IsMini() && !method->IsLite() && !method->IsFull()) {
        method->SetAttribute(interface->GetAttribute());
    }

    if (!interface->IsMini() && method->IsMini()) {
        LogError(__func__, __LINE__, StringHelper::Format("the '%s' method can not have 'mini' attribute, because the "
            "'%s' interface has no 'mini' attribute", method->GetName().c_str(), interface->GetName().c_str()));
    }

    if (!interface->IsLite() && method->IsLite()) {
        LogError(__func__, __LINE__, StringHelper::Format("the '%s' method can not have 'lite' attribute, because the "
            "'%s' interface has no 'lite' attribute", method->GetName().c_str(), interface->GetName().c_str()));
    }

    if (!interface->IsFull() && method->IsFull()) {
        LogError(__func__, __LINE__, StringHelper::Format("the '%s' method can not have 'full' attribute, because the "
            "'%s' interface has no 'full' attribute", method->GetName().c_str(), interface->GetName().c_str()));
    }

    // the method has 'oneway' attribute if interface or method has 'oneway' attribute
    if (interface->IsOneWay() || method->IsOneWay()) {
        method->GetAttribute()->SetValue(ASTAttr::ONEWAY);
    }
}

void Parser::ParseMethodParamList(const AutoPtr<ASTMethod> &method)
{
    // (
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::PARENTHESES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '('"));
    } else {
        lexer_.GetToken();
    }

    // ()
    token = lexer_.PeekToken();
    if (token.kind == TokenType::PARENTHESES_RIGHT) {
        lexer_.GetToken();
        return;
    }

    // param
    while (token.kind != TokenType::PARENTHESES_RIGHT && token.kind != TokenType::END_OF_FILE) {
        AutoPtr<ASTParameter> param = ParseParam();
        if (method->IsOneWay() && (param->GetAttribute() & ASTParamAttr::PARAM_OUT)) {
            LogError(__func__, __LINE__, token, StringHelper::Format("the '%s' parameter of '%s' method "
                "can not be 'out'", param->GetName().c_str(), method->GetName().c_str()));
        }
        method->AddParameter(param);

        // ,
        token = lexer_.PeekToken();
        if (token.kind == TokenType::COMMA) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
            if (token.kind == TokenType::PARENTHESES_RIGHT) {
                LogError(__func__, __LINE__, token, std::string(""));
            }
            continue;
        }

        // )
        if (token.kind == TokenType::PARENTHESES_RIGHT) {
            lexer_.GetToken();
        } else {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ',' or ')'"));
            lexer_.SkipToken(TokenType::PARENTHESES_RIGHT);
        }
        break;
    }
}

AutoPtr<ASTParameter> Parser::ParseParam()
{
    AutoPtr<ASTParamAttr> paramAttr = ParseParamAttr();
    AutoPtr<ASTType> paramType = ParseType();
    std::string paramName;

    // param name
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected param name"));
    } else {
        paramName = token.value;
        lexer_.GetToken();
    }

    if (paramType != nullptr && paramType->IsInterfaceType()) {
        AutoPtr<ASTInterfaceType> ifaceType = static_cast<ASTInterfaceType *>(paramType.Get());
        if (!ifaceType->IsCallback()) {
            ifaceType->SetSerializable(true);
        }
    }

    return new ASTParameter(paramName, paramAttr, paramType);
}

AutoPtr<ASTParamAttr> Parser::ParseParamAttr()
{
    AutoPtr<ASTParamAttr> attr = new ASTParamAttr(ASTParamAttr::PARAM_NONE);

    if (!CheckParamAttr()) {
        return attr;
    }

    // [in], [out], [inout], [in, out]
    Token token = lexer_.PeekToken();
    while (token.kind != TokenType::BRACKETS_RIGHT && token.kind != TokenType::END_OF_FILE) {
        SetParamAttrVal(token, attr);

        // ,
        token = lexer_.PeekToken();
        if (token.kind == TokenType::COMMA) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
            continue;
        }

        // ]
        if (token.kind != TokenType::BRACKETS_RIGHT) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ']'"));
            while (token.kind != TokenType::COMMA && token.kind != TokenType::PARENTHESES_RIGHT &&
                token.kind != TokenType::END_OF_FILE) {
                lexer_.GetToken();
                token = lexer_.PeekToken();
            }
            return attr;
        }
    }
    if (attr->value_ == ASTParamAttr::PARAM_NONE) {
        attr->value_ |= ASTParamAttr::PARAM_IN;
    }
    lexer_.GetToken();

    return attr;
}

bool Parser::CheckParamAttr()
{
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '['"));
        while (token.kind != TokenType::COMMA && token.kind != TokenType::PARENTHESES_RIGHT &&
            token.kind != TokenType::END_OF_FILE) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
        }
        return false;
    }
    lexer_.GetToken();
    return true;
}

void Parser::SetParamAttrVal(Token token, AutoPtr<ASTParamAttr> attr)
{
    switch (token.kind) {
        case TokenType::IN:
            attr->value_ |= ASTParamAttr::PARAM_IN;
            break;
        case TokenType::OUT:
            attr->value_ |= ASTParamAttr::PARAM_OUT;
            break;
        case TokenType::INOUT:
            attr->value_ |= ASTParamAttr::PARAM_INOUT;
            break;
        default:
            LogErrorBeforeToken(__func__, __LINE__, token,
                StringHelper::Format("expected 'in', 'out' or 'inout' attribute"));
            return;
    }
    lexer_.GetToken();
}

AutoPtr<ASTType> Parser::ParseType()
{
    AutoPtr<ASTType> type = nullptr;
    Token token = lexer_.PeekToken();
    if (IntfTypeChecker::CheckBasicType(token)) {
        type = ParseBasicType();
    } else if (IntfTypeChecker::CheckUserDefType(token)) {
        type = ParseUserDefType();
    } else {
        switch (token.kind) {
            case TokenType::LIST:
                type = ParseListType();
                break;
            case TokenType::SET:
                type = ParseSetType();
                break;
            case TokenType::MAP:
                type = ParseMapType();
                break;
            case TokenType::ORDEREDMAP:
                type = ParseOrderedMapType();
                break;
            case TokenType::SMQ:
                type = ParseSmqType();
                break;
            case TokenType::SHAREDPTR:
            case TokenType::UNIQUEPTR:
            case TokenType::SPTR:
            case TokenType::NULL_SHAREDPTR:
            case TokenType::NULL_UNIQUEPTR:
            case TokenType::NULL_SPTR:
                type = ParsePtrType(token.kind);
                break;
            default:
                LogError(__func__, __LINE__, token, StringHelper::Format("'%s' of type is illegal",
                    token.value.c_str()));
                return nullptr;
        }
    }

    if (type == nullptr) {
        LogError(__func__, __LINE__, token, std::string("this type was not declared in this scope"));
    }
    if (!CheckType(token, type)) {
        return nullptr;
    }

    while (lexer_.PeekToken().kind == TokenType::BRACKETS_LEFT) {
        type = ParseArrayType(type);
    }
    return type;
}

AutoPtr<ASTType> Parser::ParseBasicType()
{
    AutoPtr<ASTType> type = nullptr;
    Token token = lexer_.PeekToken();
    if (token.kind == TokenType::UNSIGNED) {
        type = ParseUnsignedType();
    } else {
        type = ast_->FindType(token.value);
        lexer_.GetToken();
    }

    ast_->AddType(type);
    return type;
}

AutoPtr<ASTType> Parser::ParseUnsignedType()
{
    AutoPtr<ASTType> type = nullptr;
    std::string namePrefix = lexer_.GetToken().value;
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::CHAR:
        case TokenType::SHORT:
        case TokenType::INT:
        case TokenType::LONG:
            type = ast_->FindType(namePrefix + " " + token.value);
            lexer_.GetToken();
            break;
        default:
            LogError(__func__, __LINE__, token,
                StringHelper::Format("'unsigned %s' was not declared in the idl file", token.value.c_str()));
            break;
    }
    return type;
}

AutoPtr<ASTType> Parser::ParseArrayType(const AutoPtr<ASTType> &elementType)
{
    lexer_.GetToken(); // '['

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ']'"));
        return nullptr;
    }
    lexer_.GetToken(); // ']'

    if (elementType == nullptr) {
        return nullptr;
    }

    AutoPtr<ASTArrayType> arrayType = new ASTArrayType();
    arrayType->SetElementType(elementType);
    AutoPtr<ASTType> retType = ast_->FindType(arrayType->ToString(), false);
    if (retType == nullptr) {
        retType = arrayType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParseListType()
{
    lexer_.GetToken(); // List

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> type = ParseType(); // element type
    if (type == nullptr) {
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken(); // '>'
    }

    AutoPtr<ASTListType> listType = new ASTListType();
    listType->SetElementType(type);
    AutoPtr<ASTType> retType = ast_->FindType(listType->ToString(), false);
    if (retType == nullptr) {
        retType = listType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParseSetType()
{
    lexer_.GetToken(); // Set

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> type = ParseType(); // element type
    if (type == nullptr) {
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken(); // '>'
    }

    AutoPtr<ASTSetType> setType = new ASTSetType();
    setType->SetElementType(type);
    AutoPtr<ASTType> retType = ast_->FindType(setType->ToString(), false);
    if (retType == nullptr) {
        retType = setType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParsePtrType(TokenType ptrKind)
{
    lexer_.GetToken(); // shared_ptr unique_ptr sptr

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> type = ParseType(); // element type
    if (type == nullptr || !IntfTypeChecker::CheckSAPtrType(type)) {
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken(); // '>'
    }

    AutoPtr<ASTPtrType> ptrType = new ASTPtrType(ptrKind);
    ptrType->SetElementType(type);
    AutoPtr<ASTType> retType = ast_->FindType(ptrType->ToString(), false);
    if (retType == nullptr) {
        retType = ptrType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParseMapType()
{
    lexer_.GetToken(); // 'Map'

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> keyType = ParseType(); // key type
    if (keyType == nullptr) {
        LogError(__func__, __LINE__, token, StringHelper::Format("key type '%s' is illegal", token.value.c_str()));
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::COMMA) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ','"));
    } else {
        lexer_.GetToken(); // ','
    }

    AutoPtr<ASTType> valueType = ParseType();
    if (valueType == nullptr) {
        LogError(__func__, __LINE__, token, StringHelper::Format("value type '%s' is illegal", token.value.c_str()));
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken();
    }

    AutoPtr<ASTMapType> mapType = new ASTMapType();
    mapType->SetKeyType(keyType);
    mapType->SetValueType(valueType);
    AutoPtr<ASTType> retType = ast_->FindType(mapType->ToString(), false);
    if (retType == nullptr) {
        retType = mapType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParseOrderedMapType()
{
    lexer_.GetToken(); // 'OrderedMap'

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> keyType = ParseType(); // key type
    if (keyType == nullptr) {
        LogError(__func__, __LINE__, token, StringHelper::Format("key type '%s' is illegal", token.value.c_str()));
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::COMMA) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ','"));
    } else {
        lexer_.GetToken(); // ','
    }

    AutoPtr<ASTType> valueType = ParseType();
    if (valueType == nullptr) {
        LogError(__func__, __LINE__, token, StringHelper::Format("value type '%s' is illegal", token.value.c_str()));
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken();
    }

    AutoPtr<ASTOrderedMapType> orderedmapType = new ASTOrderedMapType();
    orderedmapType->SetKeyType(keyType);
    orderedmapType->SetValueType(valueType);
    AutoPtr<ASTType> retType = ast_->FindType(orderedmapType->ToString(), false);
    if (retType == nullptr) {
        retType = orderedmapType.Get();
        ast_->AddType(retType);
    }
    return retType;
}

AutoPtr<ASTType> Parser::ParseSmqType()
{
    lexer_.GetToken(); // 'SharedMemQueue'

    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '<'"));
    } else {
        lexer_.GetToken(); // '<'
    }

    AutoPtr<ASTType> innerType = ParseType();
    if (innerType == nullptr) {
        lexer_.SkipToken(TokenType::ANGLE_BRACKETS_RIGHT);
        return nullptr;
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ANGLE_BRACKETS_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '>'"));
    } else {
        lexer_.GetToken(); // '>'
    }

    AutoPtr<ASTSmqType> smqType = new ASTSmqType();
    smqType->SetInnerType(innerType);
    AutoPtr<ASTType> retType = smqType.Get();
    ast_->AddType(retType);
    return retType;
}
} // namespace Idl
} // namespace OHOS
