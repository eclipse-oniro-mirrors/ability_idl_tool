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
            case TokenType::IMPORT:
            case TokenType::SEQ:
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
    } else {
        ParseSequenceableInfo();
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
        default:
            LogError(__func__, __LINE__, token, StringHelper::Format("'%s' is a illegal attribute",
                token.value.c_str()));
            lexer_.SkipToken(TokenType::BRACKETS_RIGHT);
            return false;
    }
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
        if (ipcCodeValue < MIN_TRANSACTION_ID || ipcCodeValue > MAX_TRANSACTION_ID) {
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
    if (options.GetInterfaceType() == InterfaceType::SA && options.GetLanguage() == Language::CPP) {
        size_t index = token.value.rfind('.');
        if (std::count(token.value.begin(), token.value.end(), '.') > 1) {
            ParserPackageInfo(token.value.substr(0, index));
        }
    }

    AutoPtr<ASTInterfaceType> interfaceType = new ASTInterfaceType;
    AutoPtr<ASTAttr> astAttr = ParseInfAttrInfo(attrs);
    interfaceType->SetAttribute(astAttr);

    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected interface name"));
    } else {
        size_t index = token.value.rfind('.');
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
        ipcCodeValue = std::stoi(method->GetIpcCodeStr());
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
        token.kind == TokenType::LIST || token.kind == TokenType::MAP || token.kind == TokenType::SMQ) {
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
        AutoPtr<ASTInterfaceType> ifaceType = dynamic_cast<ASTInterfaceType *>(paramType.Get());
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
            case TokenType::MAP:
                type = ParseMapType();
                break;
            case TokenType::SMQ:
                type = ParseSmqType();
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

AutoPtr<ASTType> Parser::ParseUserDefType()
{
    Token token = lexer_.GetToken();
    if (token.kind == TokenType::ID) {
        return ast_->FindType(token.value);
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected identifier"));
        return nullptr;
    }
    lexer_.GetToken();
    AutoPtr<ASTType> type = ast_->FindType(token.value);
    ast_->AddType(type);
    return type;
}

void Parser::ParseEnumDeclaration(const AttrSet &attrs)
{
    AutoPtr<ASTEnumType> enumType = new ASTEnumType;
    currentEnum_ = enumType;
    enumType->SetAttribute(ParseUserDefTypeAttr(attrs));

    lexer_.GetToken();
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected enum type name"));
    } else {
        lexer_.GetToken();
        enumType->SetName(token.value);
    }

    token = lexer_.PeekToken();
    if (token.kind == TokenType::COLON || token.kind == TokenType::BRACES_LEFT) {
        enumType->SetBaseType(ParseEnumBaseType());
    } else {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ':' or '{'"));
    }

    ParserEnumMember(enumType);
    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '}'"));
        return;
    }

    lexer_.GetToken();

    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ';'"));
    } else {
        lexer_.GetToken();
    }

    enumType->SetNamespace(ast_->ParseNamespace(ast_->GetFullName()));
    ast_->AddTypeDefinition(enumType.Get());
    currentEnum_ = nullptr;
}

AutoPtr<ASTType> Parser::ParseEnumBaseType()
{
    Token token = lexer_.PeekToken();
    lexer_.GetToken();
    if (token.kind != TokenType::COLON) {
        return ast_->FindType("int");
    }

    token = lexer_.PeekToken();
    AutoPtr<ASTType> baseType = ParseType();
    if (baseType != nullptr) {
        switch (baseType->GetTypeKind()) {
            case TypeKind::TYPE_BYTE:
            case TypeKind::TYPE_SHORT:
            case TypeKind::TYPE_INT:
            case TypeKind::TYPE_LONG:
            case TypeKind::TYPE_UCHAR:
            case TypeKind::TYPE_USHORT:
            case TypeKind::TYPE_UINT:
            case TypeKind::TYPE_ULONG:
            case TypeKind::TYPE_ENUM:
                break;
            default: {
                LogError(__func__, __LINE__, token, std::string("illegal base type of enum"));
                lexer_.SkipUntilToken(TokenType::BRACES_LEFT);
                break;
            }
        }
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '{'"));
    }
    lexer_.GetToken();
    return baseType;
}

void Parser::ParserEnumMember(const AutoPtr<ASTEnumType> &enumType)
{
    while (lexer_.PeekToken().kind == TokenType::ID) {
        Token token = lexer_.GetToken();
        AutoPtr<ASTEnumValue> enumValue = new ASTEnumValue(token.value);

        token = lexer_.PeekToken();
        if (token.kind == TokenType::ASSIGN) {
            lexer_.GetToken();
            enumValue->SetExprValue(ParseExpr());
        }

        enumValue->SetType(enumType->GetBaseType());
        if (!enumType->AddMember(enumValue)) {
            LogError(__func__, __LINE__, StringHelper::Format("AddMemberException:member '%s' already exists !",
                enumValue->GetName().c_str()));
        }
        token = lexer_.PeekToken();
        if (token.kind == TokenType::COMMA) {
            lexer_.GetToken();
            continue;
        }

        if (token.kind != TokenType::BRACES_RIGHT) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ',' or '}'"));
        }
    }
}

void Parser::ParseStructDeclaration(const AttrSet &attrs)
{
    AutoPtr<ASTStructType> structType = new ASTStructType;
    structType->SetAttribute(ParseUserDefTypeAttr(attrs));

    lexer_.GetToken();
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected struct name"));
    } else {
        structType->SetName(token.value);
        lexer_.GetToken();
    }

    token = lexer_.PeekToken();
    if (token.kind == TokenType::COLON) {
        AutoPtr<ASTStructType> parentType = ParseStructParentType();
        structType->SetParentType(parentType);
    } else if (token.kind != TokenType::BRACES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '{'"));
    } else {
        lexer_.GetToken();
    }

    ParseStructMember(structType);

    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '}'"));
    } else {
        lexer_.GetToken();
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ';'"));
    } else {
        lexer_.GetToken();
    }

    structType->SetNamespace(ast_->ParseNamespace(ast_->GetFullName()));
    ast_->AddTypeDefinition(structType.Get());
}

AutoPtr<ASTStructType> Parser::ParseStructParentType()
{
    lexer_.GetToken();
    Token token = lexer_.PeekToken();
    AutoPtr<ASTType> baseType = ParseType();
    if (baseType == nullptr) {
        LogError(__func__, __LINE__, token, std::string("expected base type name before '{' token"));
        return nullptr;
    }

    if (baseType->GetTypeKind() != TypeKind::TYPE_STRUCT) {
        LogError(__func__, __LINE__, token, StringHelper::Format("illegal base type of struct: '%s'",
            baseType->ToString().c_str()));
        lexer_.SkipUntilToken(TokenType::BRACES_LEFT);
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '{'"));
    }
    lexer_.GetToken();
    return dynamic_cast<ASTStructType *>(baseType.Get());
}

void Parser::ParseStructMember(const AutoPtr<ASTStructType> &structType)
{
    Token token = lexer_.PeekToken();
    while (token.kind != TokenType::BRACES_RIGHT && token.kind != TokenType::END_OF_FILE) {
        AutoPtr<ASTType> memberType = ParseType();
        if (memberType == nullptr) {
            lexer_.SkipToken(TokenType::SEMICOLON);
            token = lexer_.PeekToken();
            continue;
        }

        token = lexer_.PeekToken();
        if (token.kind != TokenType::ID) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected member name"));
            lexer_.SkipToken(TokenType::SEMICOLON);
            token = lexer_.PeekToken();
            continue;
        }

        lexer_.GetToken();
        std::string memberName = token.value;
        structType->AddMember(memberType, memberName);

        token = lexer_.PeekToken();
        if (token.kind == TokenType::SEMICOLON) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
            continue;
        }

        if (token.kind != TokenType::BRACES_RIGHT) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ',' or '}'"));
        }
    }
}

void Parser::ParseUnionDeclaration(const AttrSet &attrs)
{
    AutoPtr<ASTUnionType> unionType = new ASTUnionType;
    unionType->SetAttribute(ParseUserDefTypeAttr(attrs));

    lexer_.GetToken();
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected struct name"));
    } else {
        unionType->SetName(token.value);
        lexer_.GetToken();
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_LEFT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '{'"));
    } else {
        lexer_.GetToken();
    }

    ParseUnionMember(unionType);

    token = lexer_.PeekToken();
    if (token.kind != TokenType::BRACES_RIGHT) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected '}'"));
    } else {
        lexer_.GetToken();
    }

    token = lexer_.PeekToken();
    if (token.kind != TokenType::SEMICOLON) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ';'"));
    } else {
        lexer_.GetToken();
    }

    unionType->SetNamespace(ast_->ParseNamespace(ast_->GetFullName()));
    ast_->AddTypeDefinition(unionType.Get());
}

void Parser::ParseUnionMember(const AutoPtr<ASTUnionType> &unionType)
{
    Token token = lexer_.PeekToken();
    while (token.kind != TokenType::BRACES_RIGHT && token.kind != TokenType::END_OF_FILE) {
        AutoPtr<ASTType> memberType = ParseType();
        if (memberType == nullptr) {
            lexer_.SkipToken(TokenType::SEMICOLON);
            token = lexer_.PeekToken();
            continue;
        }

        token = lexer_.PeekToken();
        if (token.kind != TokenType::ID) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected member name"));
            lexer_.SkipToken(TokenType::SEMICOLON);
            token = lexer_.PeekToken();
            continue;
        }
        lexer_.GetToken();

        std::string memberName = token.value;
        if (!AddUnionMember(unionType, memberType, memberName)) {
            LogError(__func__, __LINE__, token, StringHelper::Format(
                "union not support this type or name of member duplicate '%s'", token.value.c_str()));
        }

        token = lexer_.PeekToken();
        if (token.kind == TokenType::SEMICOLON) {
            lexer_.GetToken();
            token = lexer_.PeekToken();
            continue;
        }

        if (token.kind != TokenType::BRACES_RIGHT) {
            LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected ',' or '}'"));
        }
    }
}

bool Parser::AddUnionMember(
    const AutoPtr<ASTUnionType> &unionType, const AutoPtr<ASTType> &type, const std::string &name) const
{
    for (size_t i = 0; i < unionType->GetMemberNumber(); i++) {
        if (name == unionType->GetMemberName(i)) {
            return false;
        }
    }

    // Non pod type members are not allowed in the union
    if (!type->IsPod()) {
        return false;
    }

    unionType->AddMember(type, name);
    return true;
}

AutoPtr<ASTAttr> Parser::ParseUserDefTypeAttr(const AttrSet &attrs)
{
    AutoPtr<ASTAttr> attr = new ASTAttr();
    for (const auto &token : attrs) {
        switch (token.kind) {
            case TokenType::FULL:
                attr->SetValue(ASTAttr::FULL);
                break;
            case TokenType::LITE:
                attr->SetValue(ASTAttr::LITE);
                break;
            case TokenType::MINI:
                attr->SetValue(ASTAttr::MINI);
                break;
            default:
                LogError(__func__, __LINE__, token, StringHelper::Format("invalid attribute '%s' for type decl",
                    token.value.c_str()));
                break;
        }
    }

    if (attr->IsNone()) {
        attr->SetValue(ASTAttr::FULL | ASTAttr::LITE | ASTAttr::MINI);
    }

    return attr;
}

AutoPtr<ASTExpr> Parser::ParseExpr()
{
    lexer_.SetParseMode(Lexer::ParseMode::EXPR_MODE);
    AutoPtr<ASTExpr> value = ParseAndExpr();
    lexer_.SetParseMode(Lexer::ParseMode::DECL_MODE);
    return value;
}

AutoPtr<ASTExpr> Parser::ParseAndExpr()
{
    AutoPtr<ASTExpr> left = ParseXorExpr();
    Token token = lexer_.PeekToken();
    while (token.kind == TokenType::AND) {
        lexer_.GetToken();
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = BinaryOpKind::AND;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseXorExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseXorExpr()
{
    AutoPtr<ASTExpr> left = ParseOrExpr();
    Token token = lexer_.PeekToken();
    while (token.kind == TokenType::XOR) {
        lexer_.GetToken();
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = BinaryOpKind::XOR;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseOrExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseOrExpr()
{
    AutoPtr<ASTExpr> left = ParseShiftExpr();
    Token token = lexer_.PeekToken();
    while (token.kind == TokenType::OR) {
        lexer_.GetToken();
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = BinaryOpKind::OR;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseShiftExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseShiftExpr()
{
    AutoPtr<ASTExpr> left = ParseAddExpr();
    Token token = lexer_.PeekToken();
    while (token.kind == TokenType::LEFT_SHIFT || token.kind == TokenType::RIGHT_SHIFT) {
        lexer_.GetToken();
        BinaryOpKind op = (token.kind == TokenType::LEFT_SHIFT) ? BinaryOpKind::LSHIFT : BinaryOpKind::RSHIFT;
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = op;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseAddExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseAddExpr()
{
    AutoPtr<ASTExpr> left = ParseMulExpr();
    Token token = lexer_.PeekToken();
    while (token.kind == TokenType::ADD || token.kind == TokenType::SUB) {
        lexer_.GetToken();
        BinaryOpKind op = (token.kind == TokenType::ADD) ? BinaryOpKind::ADD : BinaryOpKind::SUB;
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = op;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseMulExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseMulExpr()
{
    AutoPtr<ASTExpr> left = ParseUnaryExpr();
    Token token = lexer_.PeekToken();
    while (
        token.kind == TokenType::STAR || token.kind == TokenType::SLASH || token.kind == TokenType::PERCENT_SIGN) {
        lexer_.GetToken();
        BinaryOpKind op = BinaryOpKind::MUL;
        if (token.kind == TokenType::SLASH) {
            op = BinaryOpKind::DIV;
        } else if (token.kind == TokenType::PERCENT_SIGN) {
            op = BinaryOpKind::MOD;
        }
        AutoPtr<ASTBinaryExpr> expr = new ASTBinaryExpr;
        expr->op_ = op;
        expr->lExpr_ = left;
        expr->rExpr_ = ParseUnaryExpr();

        left = expr.Get();
        token = lexer_.PeekToken();
    }
    return left;
}

AutoPtr<ASTExpr> Parser::ParseUnaryExpr()
{
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::TILDE: {
            lexer_.GetToken();
            AutoPtr<ASTUnaryExpr> expr = new ASTUnaryExpr;
            expr->op_ = UnaryOpKind::PLUS;
            if (token.kind == TokenType::SUB) {
                expr->op_ = UnaryOpKind::MINUS;
            } else if (token.kind == TokenType::TILDE) {
                expr->op_ = UnaryOpKind::TILDE;
            }

            expr->expr_ = ParseUnaryExpr();
            return expr.Get();
        }
        default:
            return ParsePrimaryExpr();
    }
}

AutoPtr<ASTExpr> Parser::ParsePrimaryExpr()
{
    Token token = lexer_.PeekToken();
    switch (token.kind) {
        case TokenType::PARENTHESES_LEFT: {
            lexer_.GetToken();
            AutoPtr<ASTExpr> expr = ParseExpr();
            token = lexer_.PeekToken();
            if (token.kind != TokenType::PARENTHESES_RIGHT) {
                LogErrorBeforeToken(__func__, __LINE__, token, StringHelper::Format("expected ')'"));
            } else {
                lexer_.GetToken();
                expr->isParenExpr = true;
            }
            return expr;
        }
        case TokenType::NUM:
            return ParseNumExpr();
        case TokenType::ID:
            if (currentEnum_ == nullptr) {
                LogError(__func__, __LINE__, token, std::string("this expression is not supported"));
                lexer_.SkipUntilToken(TokenType::COMMA);
                return nullptr;
            }
            return ParseEnumExpr();
        default:
            LogError(__func__, __LINE__, token, std::string("this expression is not supported"));
            lexer_.SkipUntilToken(TokenType::COMMA);
            return nullptr;
    }
}

AutoPtr<ASTExpr> Parser::ParseNumExpr()
{
    Token token = lexer_.GetToken();
    if (!CheckNumber(token.value)) {
        LogError(__func__, __LINE__, token, StringHelper::Format("unknown integer number: '%s'", token.value.c_str()));
        return nullptr;
    }

    AutoPtr<ASTNumExpr> expr = new ASTNumExpr;
    expr->value_ = token.value;
    return expr.Get();
}

AutoPtr<ASTExpr> Parser::ParseEnumExpr()
{
    Token token = lexer_.GetToken();
    if (!currentEnum_->HasMember(token.value)) {
        LogError(__func__, __LINE__, token, StringHelper::Format("unknown enum member: '%s'", token.value.c_str()));
        return nullptr;
    }

    AutoPtr<ASTEnumExpr> expr = new ASTEnumExpr;
    expr->value_ = token.value;
    return expr.Get();
}

bool Parser::CheckNumber(const std::string& integerVal) const
{
    return std::regex_match(integerVal, RE_BIN_NUM) || std::regex_match(integerVal, RE_OCT_NUM)||
        std::regex_match(integerVal, RE_DEC_NUM) || std::regex_match(integerVal, RE_HEX_NUM);
}

bool Parser::CheckType(const Token &token, const AutoPtr<ASTType> &type)
{
    if ((type == nullptr) || !CheckTypeByMode(token, type)) {
        return false;
    }

    if (Options::GetInstance().GetLanguage() == Language::C) {
        if (type->IsSequenceableType() || type->IsSmqType() || type->IsAshmemType()) {
            LogError(__func__, __LINE__, token, StringHelper::Format("The %s type is not supported by c language.",
                type->ToString().c_str()));
            return false;
        }
    } else if (Options::GetInstance().GetLanguage() == Language::JAVA) {
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_UCHAR:
            case TypeKind::TYPE_USHORT:
            case TypeKind::TYPE_UINT:
            case TypeKind::TYPE_ULONG:
            case TypeKind::TYPE_ENUM:
            case TypeKind::TYPE_STRUCT:
            case TypeKind::TYPE_UNION:
            case TypeKind::TYPE_SMQ:
            case TypeKind::TYPE_UNKNOWN:
                LogError(__func__, __LINE__, token, StringHelper::Format("The '%s' type is not supported "
                    "by java language.", type->ToString().c_str()));
                return false;
            default:
                break;
        }
    }

    return true;
}

bool Parser::CheckTypeByMode(const Token &token, const AutoPtr<ASTType> &type)
{
    if (!Options::GetInstance().DoPassthrough() && type->IsPointerType()) {
        LogError(__func__, __LINE__, token, StringHelper::Format("The %s type is only supported by passthrough mode.",
            type->ToString().c_str()));
        return false;
    }

    if (Options::GetInstance().DoGenerateKernelCode()) {
        switch (type->GetTypeKind()) {
            case TypeKind::TYPE_FLOAT:
            case TypeKind::TYPE_DOUBLE:
            case TypeKind::TYPE_FILEDESCRIPTOR:
            case TypeKind::TYPE_INTERFACE:
            case TypeKind::TYPE_SMQ:
            case TypeKind::TYPE_ASHMEM:
            case TypeKind::TYPE_NATIVE_BUFFER:
            case TypeKind::TYPE_POINTER:
                LogError(__func__, __LINE__, token,
                    StringHelper::Format("The '%s' type is not supported by kernel mode.", type->ToString().c_str()));
                return false;
            default:
                break;
        }
    }
    return true;
}

void Parser::SetAstFileType()
{
    if (ast_->GetInterfaceDef() != nullptr) {
        if (ast_->GetInterfaceDef()->IsCallback()) {
            ast_->SetAStFileType(ASTFileType::AST_ICALLBACK);
        } else {
            ast_->SetAStFileType(ASTFileType::AST_IFACE);
        }
    } else {
        ast_->SetAStFileType(ASTFileType::AST_TYPES);
    }
}

/*
 * filePath: ./ohos/interface/foo/v1_0/IFoo.idl
 * package OHOS.Hdi.foo.v1_0;
 */
bool Parser::CheckPackageName(const std::string &filePath, const std::string &packageName) const
{
    std::string pkgToPath = Options::GetInstance().GetPackagePath(packageName);

    size_t index = filePath.rfind(SEPARATOR);
    if (index == std::string::npos) {
        return false;
    }

    std::string parentDir = filePath.substr(0, index);
    return parentDir == pkgToPath;
}

bool Parser::CheckImport(const Token &token)
{
    std::string importName = token.value;
    if (Options::GetInstance().GetInterfaceType() == InterfaceType::HDI) {
        if (!std::regex_match(importName.c_str(), RE_IMPORT)) {
            LogError(__func__, __LINE__, StringHelper::Format("invalid impirt name '%s'", importName.c_str()));
            return false;
        }
    } else if (Options::GetInstance().GetInterfaceType() == InterfaceType::SA) {
        if (!std::regex_match(importName.c_str(), RE_PACKAGE_OR_IMPORT_SA)) {
            LogError(__func__, __LINE__, StringHelper::Format("invalid impirt name '%s'", importName.c_str()));
            return false;
        }
    } else {
        if (!std::regex_match(importName.c_str(), RE_PACKAGE_OR_IMPORT_SM)) {
            LogError(__func__, __LINE__, StringHelper::Format("invalid impirt name '%s'", importName.c_str()));
            return false;
        }
    }
    std::string idlFilePath = File::CanonicalPath(
        Options::GetInstance().GetImportFilePath(importName, token.location.filePath));
    if (!File::CheckValid(idlFilePath)) {
        LogError(__func__, __LINE__, StringHelper::Format("can not import '%s'", idlFilePath.c_str()));
        return false;
    }
    return true;
}

bool Parser::AddAst(const AutoPtr<AST> &ast)
{
    if (ast == nullptr) {
        LogError(__func__, __LINE__, std::string("ast is nullptr."));
        return false;
    }

    allAsts_[ast->GetFullName()] = ast;
    return true;
}

void Parser::ParseInterfaceExtends(AutoPtr<ASTInterfaceType> &interface)
{
    Token token = lexer_.PeekToken();
    if (token.kind != TokenType::EXTENDS) {
        return;
    }
    lexer_.GetToken();
    token = lexer_.PeekToken();
    if (token.kind != TokenType::ID) {
        LogErrorBeforeToken(__func__, __LINE__, token, std::string("expected  extends interface name"));
        lexer_.SkipToken(TokenType::BRACES_LEFT);
        return;
    }
    ParseExtendsInfo(interface);
    lexer_.GetToken();
}

void Parser::ParseExtendsInfo(AutoPtr<ASTInterfaceType> &interfaceType)
{
    Token token = lexer_.PeekToken();
    std::string extendsInterfaceName = token.value;
    if (extendsInterfaceName.empty()) {
        LogError(__func__, __LINE__, token, std::string("extends interface name is empty"));
        return;
    }
    if (!CheckImport(token)) {
        LogError(__func__, __LINE__, token, std::string("extends interface name is illegal"));
        return;
    }
    auto iter = allAsts_.find(extendsInterfaceName);
    AutoPtr<AST> extendsAst = (iter != allAsts_.end()) ? iter->second : nullptr;
    if (extendsAst == nullptr) {
        LogError(__func__, __LINE__, token, StringHelper::Format("can not find idl file by extends interface "
            "name '%s', please check import info", extendsInterfaceName.c_str()));
        return;
    }
    if (!CheckExtendsName(interfaceType, extendsInterfaceName)) {
        LogError(__func__, __LINE__, token, StringHelper::Format(
            "extends interface name must same as current interface name '%s'", interfaceType->GetName().c_str()));
        return;
    }
    if (!CheckExtendsVersion(extendsAst)) {
        LogError(__func__, __LINE__, token,
            std::string("extends interface version must less than current interface version"));
        return;
    }
    if (!interfaceType->AddExtendsInterface(extendsAst->GetInterfaceDef())) {
        LogError(__func__, __LINE__, token, StringHelper::Format("multiple extends of '%s'",
            interfaceType->GetName().c_str()));
    }
}

bool Parser::CheckExtendsName(AutoPtr<ASTInterfaceType> &interfaceType, const std::string &extendsInterfaceName)
{
    return extendsInterfaceName.substr(extendsInterfaceName.rfind('.') + 1) == interfaceType->GetName();
}

bool Parser::CheckExtendsVersion(AutoPtr<AST> extendsAst)
{
    return !(extendsAst->GetMajorVer() != ast_->GetMajorVer() || extendsAst->GetMinorVer() >= ast_->GetMinorVer());
}

bool Parser::CheckImportsVersion(AutoPtr<AST> extendsAst)
{
    return !(extendsAst->GetMajorVer() != ast_->GetMajorVer() || extendsAst->GetMinorVer() > ast_->GetMinorVer());
}

bool Parser::PostProcess()
{
    if ((Options::GetInstance().GetLanguage() != Language::C) || !CheckExistExtends()) {
        return true;
    }
    std::vector<size_t> genVersion = {0, 0};
    std::string genPackageName;
    AutoPtr<ASTNamespace> ns;
    if (!GetGenVersion(genVersion, genPackageName)) {
        return false;
    }
    GetGenNamespace(ns);
    AstMergeMap mergeMap;
    SortAstByName(mergeMap, allAsts_);
    allAsts_.clear();
    MergeAsts(mergeMap);
    ModifyImport(allAsts_, genPackageName);
    ModifyPackageNameAndVersion(allAsts_, genPackageName, genVersion);
    ModifyInterfaceNamespace(ns);

    return true;
}

bool Parser::CheckExistExtends()
{
    return std::any_of(allAsts_.begin(), allAsts_.end(), [](const std::pair<std::string, AutoPtr<AST>> &astPair) {
        return astPair.second->GetInterfaceDef() != nullptr &&
            astPair.second->GetInterfaceDef()->GetExtendsInterface() != nullptr;
    });
}

bool Parser::GetGenVersion(std::vector<size_t> &version, std::string &genPackageName)
{
    std::set<std::string> sourceFile = Options::GetInstance().GetSourceFiles();
    for (const auto &ast : allAsts_) {
        if (sourceFile.find(ast.second->GetIdlFilePath()) != sourceFile.end()) {
            if (genPackageName.empty()) {
                genPackageName = ast.second->GetPackageName();
                version[0] = ast.second->GetMajorVer();
                version[1] = ast.second->GetMinorVer();
                continue;
            }
            if (genPackageName != ast.second->GetPackageName() || version[0] != ast.second->GetMajorVer() ||
                version[1] != ast.second->GetMinorVer()) {
                LogError(__func__, __LINE__,
                    StringHelper::Format("merge ast failed, source files must have same package and version"));
                return false;
            }
        }
    }
    return true;
}

void Parser::GetGenNamespace(AutoPtr<ASTNamespace> &ns)
{
    std::set<std::string> sourceFile = Options::GetInstance().GetSourceFiles();
    for (const auto &ast : allAsts_) {
        if ((sourceFile.find(ast.second->GetIdlFilePath()) != sourceFile.end()) &&
            (ast.second->GetInterfaceDef() != nullptr)) {
            ns = ast.second->GetInterfaceDef()->GetNamespace();
            return;
        }
    }
}

void Parser::SortAstByName(AstMergeMap &mergeMap, StrAstMap &allAsts)
{
    for (const auto &astPair : allAsts) {
        AutoPtr<AST> ast = astPair.second;
        mergeMap[ast->GetName()].emplace(ast);
    }
}

void Parser::MergeAsts(AstMergeMap &mergeMap)
{
    for (const auto &setPair : mergeMap) {
        AutoPtr<AST> targetAst = nullptr;
        for (const auto &ast : setPair.second) {
            MergeAst(targetAst, ast);
        }
        AddAst(targetAst);
    }
}

void Parser::MergeAst(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    if (targetAst == nullptr) {
        targetAst = sourceAst;
        return;
    }
    MergeImport(targetAst, sourceAst);
    MergeInterfaceDef(targetAst, sourceAst);
    MergeTypeDefinitions(targetAst, sourceAst);
    MergeTypes(targetAst, sourceAst);
    MergeSequenceableDef(targetAst, sourceAst);
}

void Parser::MergeImport(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    for (const auto &importPair : sourceAst->GetImports()) {
        AutoPtr<AST> importAst = importPair.second;
        targetAst->AddImport(importAst);
    }
}

void Parser::MergeInterfaceDef(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    AutoPtr<ASTInterfaceType> sourceInterface = sourceAst->GetInterfaceDef();
    if (sourceInterface == nullptr) {
        return;
    }
    AutoPtr<ASTInterfaceType> targetInterface = targetAst->GetInterfaceDef();
    if (targetInterface == nullptr) {
        targetInterface = sourceInterface;
        return;
    }

    for (size_t i = 0; i < sourceInterface->GetMethodNumber(); i++) {
        targetInterface->AddMethod(sourceInterface->GetMethod(i));
    }
    targetInterface->SetSerializable(sourceInterface->IsSerializable());
}

void Parser::MergeTypeDefinitions(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    for (size_t i = 0; i < sourceAst->GetTypeDefinitionNumber(); i++) {
        targetAst->AddTypeDefinition(sourceAst->GetTypeDefintion(i));
    }
}

void Parser::MergeSequenceableDef(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    if (sourceAst->GetSequenceableDef() != nullptr) {
        targetAst->AddSequenceableDef(sourceAst->GetSequenceableDef());
    }
}

void Parser::MergeTypes(AutoPtr<AST> &targetAst, AutoPtr<AST> sourceAst)
{
    for (const auto &typePair : sourceAst->GetTypes()) {
        targetAst->AddType(typePair.second);
    }
}

void Parser::ModifyImport(StrAstMap &allAsts, std::string &genPackageName)
{
    for (const auto &astPair : allAsts) {
        StrAstMap modifiedImport;
        StrAstMap import = astPair.second->GetImports();
        for (const auto &importPair : import) {
            if (importPair.second->GetName() == astPair.second->GetName()) {
                continue;
            }
            modifiedImport[importPair.second->GetName()] = importPair.second;
        }
        astPair.second->ClearImport();
        for (const auto &importPair : modifiedImport) {
            importPair.second->SetPackageName(genPackageName);
            astPair.second->AddImport(importPair.second);
        }
    }
}

void Parser::ModifyPackageNameAndVersion(
    StrAstMap &allAsts, std::string &genPackageName, std::vector<size_t> genVersion)
{
    for (const auto &astPair : allAsts) {
        astPair.second->SetPackageName(genPackageName);
        astPair.second->SetVersion(genVersion[0], genVersion[1]);
    }
}

void Parser::ModifyInterfaceNamespace(AutoPtr<ASTNamespace> &ns)
{
    for (const auto &astPair : allAsts_) {
        AutoPtr<ASTInterfaceType> interface = astPair.second->GetInterfaceDef();
        if (interface != nullptr) {
            interface->SetNamespace(ns);
        }
    }
}
} // namespace Idl
} // namespace OHOS
