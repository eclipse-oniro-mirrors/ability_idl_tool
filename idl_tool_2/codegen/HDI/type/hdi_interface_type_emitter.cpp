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

#include "hdi_interface_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiInterfaceTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_INTERFACE;
}

std::string HdiInterfaceTypeEmitter::EmitCType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("struct %s", name_.c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format("struct %s*", name_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format("struct %s**", name_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("struct %s*", name_.c_str());
        default:
            return "unknow type";
    }
}

std::string HdiInterfaceTypeEmitter::EmitCppType(TypeMode mode) const
{
    std::string pointerName = "sptr";
    if (Options::GetInstance().GetSystemLevel() == SystemLevel::LITE) {
        pointerName = "std::shared_ptr";
    }
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format("%s<%s>", pointerName.c_str(), typeName_.c_str());
        case TypeMode::PARAM_IN:
        case TypeMode::PARAM_INOUT:
            return StringHelper::Format(
                "const %s<%s>&", pointerName.c_str(), typeName_.c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format(
                "%s<%s>&", pointerName.c_str(), typeName_.c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format("%s<%s>", pointerName.c_str(), typeName_.c_str());
        default:
            return "unknow type";
    }
}

std::string HdiInterfaceTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return name_;
}

std::string HdiInterfaceTypeEmitter::EmitDescMacroName() const
{
    return StringHelper::Format("%s_INTERFACE_DESC", StringHelper::StrToUpper(name_).c_str());
}

void HdiInterfaceTypeEmitter::EmitCWriteVar(TypeMode mode, const std::string &name,
    const std::string &gotoLabel, StringBuilder &sb, const std::string &prefix) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    sb.Append(prefix).AppendFormat("if (!WriteInterface(%s, %s, %s)) {\n", parcelName.c_str(),
        EmitDescMacroName().c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiInterfaceTypeEmitter::EmitCProxyReadVar(const std::string &name, bool isInnerType, const std::string &gotoLabel,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("*%s = Read%s(%s);\n", name.c_str(), name_.c_str(), replyParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (*%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiInterfaceTypeEmitter::EmitCStubReadVar(const std::string &name, const std::string &gotoLabel, StringBuilder &sb,
    const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s = Read%s(%s);\n", name.c_str(), name_.c_str(), dataParcelName_.c_str());
    sb.Append(prefix).AppendFormat("if (%s == NULL) {\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: read %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).AppendFormat("%s = HDF_ERR_INVALID_PARAM;\n", errorCodeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("goto %s;\n", gotoLabel.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiInterfaceTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB)
        .AppendFormat("HDF_LOGE(\"%%{public}s: parameter %s is nullptr!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    sb.Append('\n');
    sb.Append(prefix).AppendFormat("if (!%s.WriteRemoteObject(", parcelName.c_str());
    sb.AppendFormat("OHOS::HDI::ObjectCollector::GetInstance().GetOrNewObject(%s, %s::GetDescriptor()))) {\n",
        name.c_str(), typeName_.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

std::string HdiInterfaceTypeEmitter::GetNamespaceWithColon() const
{
    if (typeName_.empty()) {
        return "";
    }
    size_t index = typeName_.rfind(':');
    return (index == std::string::npos) ? typeName_.substr(0) : typeName_.substr(0, index + 1);
}

void HdiInterfaceTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string interfaceNamespace = GetNamespaceWithColon();
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("sptr<%s> %s;\n", typeName_.c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("sptr<IRemoteObject> %sRemote = %s.ReadRemoteObject();\n", name.c_str(),
        parcelName.c_str());
    sb.Append(prefix).AppendFormat("if (%sRemote == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: read an invalid remote object\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("%s = new %s(%sRemote);\n", name.c_str(), (interfaceNamespace +
        (StringHelper::StartWith(name_, "I") ? name_.substr(1) : name_) + "Proxy").c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("if (%s == nullptr) {\n", name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to create interface object\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
}

void HdiInterfaceTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("%s.writeRemoteObject(%s.asObject());\n", parcelName.c_str(), name.c_str());
}

void HdiInterfaceTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    std::string stubName = StringHelper::StartWith(name_, "I") ? (name_.substr(1) + "Stub") : (name_ + "Stub");
    sb.Append(prefix).AppendFormat(
        "%s = %s.asInterface(%s.readRemoteObject());\n", name.c_str(), stubName.c_str(), parcelName.c_str());
}

void HdiInterfaceTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    std::string stubName = StringHelper::StartWith(name_, "I") ? (name_.substr(1) + "Stub") : (name_ + "Stub");
    sb.Append(prefix).AppendFormat("%s %s = %s.asInterface(%s.readRemoteObject());\n",
        EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(), stubName.c_str(), parcelName.c_str());
}

void HdiInterfaceTypeEmitter::EmitCWriteMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sWriteInterface", methodPrefix.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *parcel, const char *desc, void *interface);\n",
            methodName.c_str());
    } else {
        sb.Append('\n');
        sb.Append(prefix).AppendFormat("static bool %s(struct HdfSBuf *parcel, const char *desc, void *interface)\n",
            methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("if (interface == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: invalid interface object\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append(
            "struct HdfRemoteService **stub = StubCollectorGetOrNewObject(desc, interface);\n");
        sb.Append(prefix + TAB).Append("if (stub == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append(
            "HDF_LOGE(\"%{public}s: failed to get stub of '%{public}s'\", __func__, desc);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("if (HdfSbufWriteRemoteService(parcel, *stub) != HDF_SUCCESS) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: failed to write remote service\", __func__);\n");
        sb.Append(prefix + TAB + TAB).Append("return false;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).Append("return true;\n");
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiInterfaceTypeEmitter::EmitCReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    StringBuilder sb;
    std::string methodName = StringHelper::Format("%sRead%s", methodPrefix.c_str(), name_.c_str());
    if (isDecl) {
        sb.Append(prefix).AppendFormat("static struct %s *%s(struct HdfSBuf *parcel);\n", name_.c_str(),
            methodName.c_str());
    } else {
        sb.Append('\n');
        sb.Append(prefix).AppendFormat("static struct %s *%s(struct HdfSBuf *parcel)\n", name_.c_str(),
            methodName.c_str());
        sb.Append(prefix).Append("{\n");
        sb.Append(prefix + TAB).Append("struct HdfRemoteService *remote = HdfSbufReadRemoteService(parcel);\n");
        sb.Append(prefix + TAB).Append("if (remote == NULL) {\n");
        sb.Append(prefix + TAB + TAB).Append("HDF_LOGE(\"%{public}s: ");
        sb.AppendFormat(" failed to read remote service of '%s'\", __func__);\n", name_.c_str());
        sb.Append(prefix + TAB + TAB).Append("return NULL;\n");
        sb.Append(prefix + TAB).Append("}\n\n");
        sb.Append(prefix + TAB).AppendFormat("return %sGet(remote);\n", name_.c_str());
        sb.Append(prefix).Append("}\n");
    }
    methods.emplace(methodName, sb.ToString());
}

void HdiInterfaceTypeEmitter::EmitCStubReadMethods(
    UtilMethodMap &methods, const std::string &prefix, const std::string &methodPrefix, bool isDecl) const
{
    EmitCReadMethods(methods, prefix, methodPrefix, isDecl);
}
} // namespace Idl
} // namespace OHOS
