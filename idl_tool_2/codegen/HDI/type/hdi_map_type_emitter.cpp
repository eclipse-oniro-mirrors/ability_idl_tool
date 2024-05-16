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

#include "hdi_map_type_emitter.h"

namespace OHOS {
namespace Idl {
TypeKind HdiMapTypeEmitter::GetTypeKind()
{
    return TypeKind::TYPE_MAP;
}

std::string HdiMapTypeEmitter::EmitCType(TypeMode mode) const
{
    // c language has no map type
    return "/";
}

std::string HdiMapTypeEmitter::EmitCppType(TypeMode mode) const
{
    switch (mode) {
        case TypeMode::NO_MODE:
            return StringHelper::Format(
                "std::map<%s, %s>", keyEmitter_->EmitCppType().c_str(), valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_IN:
            return StringHelper::Format(
                "const std::map<%s, %s>&", keyEmitter_->EmitCppType().c_str(), valueEmitter_->EmitCppType().c_str());
        case TypeMode::PARAM_OUT:
            return StringHelper::Format(
                "std::map<%s, %s>&", keyEmitter_->EmitCppType().c_str(), valueEmitter_->EmitCppType().c_str());
        case TypeMode::LOCAL_VAR:
            return StringHelper::Format(
                "std::map<%s, %s>", keyEmitter_->EmitCppType().c_str(), valueEmitter_->EmitCppType().c_str());
        default:
            return "unknow type";
    }
}

std::string HdiMapTypeEmitter::EmitJavaType(TypeMode mode, bool isInnerType) const
{
    return StringHelper::Format("HashMap<%s, %s>", keyEmitter_->EmitJavaType(mode, true).c_str(),
        valueEmitter_->EmitJavaType(mode, true).c_str());
}

void HdiMapTypeEmitter::EmitCppWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n");
    std::string elementName = StringHelper::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (auto %s : %s) {\n", elementName.c_str(), name.c_str());

    std::string keyName = StringHelper::Format("(%s.first)", elementName.c_str());
    std::string valueName = StringHelper::Format("(%s.second)", elementName.c_str());
    keyEmitter_->EmitCppWriteVar(parcelName, keyName, sb, prefix + TAB, innerLevel);
    valueEmitter_->EmitCppWriteVar(parcelName, valueName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitCppReadVar(const std::string &name, StringBuilder &sb, const std::string &prefix,
    TypeMode mode, unsigned int innerLevel) const
{
    std::string parcelName = (mode == TypeMode::PARAM_IN) ? dataParcelName_ : replyParcelName_;
    if ((mode == TypeMode::PARAM_IN) || (innerLevel > 0)) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), name.c_str());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = 0;\n", name.c_str());
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%sSize)) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read size\", __func__);\n");
    sb.Append(prefix + TAB).Append("return HDF_ERR_INVALID_PARAM;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %sSize; ++i) {\n", name.c_str());
    std::string keyName = StringHelper::Format("key%d", innerLevel);
    std::string valueName = StringHelper::Format("value%d", innerLevel);
    innerLevel++;
    keyEmitter_->EmitCppReadVar(keyName, sb, prefix + TAB, mode, innerLevel);
    valueEmitter_->EmitCppReadVar(valueName, sb, prefix + TAB, mode, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.c_str(), keyName.c_str(), valueName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitCppMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    sb.Append(prefix).AppendFormat("if (!%s.WriteUint32(%s.size())) {\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix + TAB).AppendFormat("HDF_LOGE(\"%%{public}s: write %s.size failed!\", __func__);\n", name.c_str());
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n");
    std::string elementName = StringHelper::Format("it%d", innerLevel++);
    sb.Append(prefix).AppendFormat("for (const auto& %s : %s) {\n", elementName.c_str(), name.c_str());

    std::string keyName = StringHelper::Format("(%s.first)", elementName.c_str());
    std::string valName = StringHelper::Format("(%s.second)", elementName.c_str());
    keyEmitter_->EmitCppMarshalling(parcelName, keyName, sb, prefix + TAB, innerLevel);
    valueEmitter_->EmitCppMarshalling(parcelName, valName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitCppUnMarshalling(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, unsigned int innerLevel) const
{
    size_t index = name.find('.', 0);
    std::string memberName = (index == std::string::npos) ? name : StringHelper::SubStr(name, index + 1);
    if (innerLevel > 0) {
        sb.Append(prefix).AppendFormat("%s %s;\n", EmitCppType().c_str(), memberName.c_str());
    }
    sb.Append(prefix).AppendFormat("uint32_t %sSize = 0;\n", memberName.c_str());
    sb.Append(prefix).AppendFormat("if (!%s.ReadUint32(%sSize)) {\n", parcelName.c_str(), memberName.c_str());
    sb.Append(prefix + TAB).Append("HDF_LOGE(\"%{public}s: failed to read size\", __func__);\n");
    sb.Append(prefix + TAB).Append("return false;\n");
    sb.Append(prefix).Append("}\n\n");
    sb.Append(prefix).AppendFormat("for (uint32_t i = 0; i < %sSize; ++i) {\n", memberName.c_str());
    std::string keyName = StringHelper::Format("key%d", innerLevel);
    std::string valueName = StringHelper::Format("value%d", innerLevel);
    innerLevel++;
    keyEmitter_->EmitCppUnMarshalling(parcelName, keyName, sb, prefix + TAB, innerLevel);
    valueEmitter_->EmitCppUnMarshalling(parcelName, valueName, sb, prefix + TAB, innerLevel);
    sb.Append(prefix + TAB).AppendFormat("%s[%s] = %s;\n", name.c_str(), keyName.c_str(), valueName.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitJavaWriteVar(const std::string &parcelName, const std::string &name,
    StringBuilder &sb, const std::string &prefix, TypeMode mode) const
{
    sb.Append(prefix).AppendFormat("%s.writeInt(%s.size());\n", parcelName.c_str(), name.c_str());
    sb.Append(prefix).AppendFormat("for (Map.Entry<%s, %s> entry : %s.entrySet()) {\n",
        keyEmitter_->EmitJavaType(TypeMode::NO_MODE, true).c_str(),
        valueEmitter_->EmitJavaType(TypeMode::NO_MODE, true).c_str(), name.c_str());
    keyEmitter_->EmitJavaWriteVar(parcelName, "entry.getKey()", sb, prefix + TAB);
    valueEmitter_->EmitJavaWriteVar(parcelName, "entry.getValue()", sb, prefix + TAB);
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitJavaReadVar(
    const std::string &parcelName, const std::string &name, StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.c_str());

    keyEmitter_->EmitJavaReadInnerVar(parcelName, "key", false, sb, prefix + TAB);
    valueEmitter_->EmitJavaReadInnerVar(parcelName, "value", false, sb, prefix + TAB);

    sb.Append(prefix + TAB).AppendFormat("%s.put(key, value);\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitJavaReadInnerVar(const std::string &parcelName, const std::string &name, bool isInner,
    StringBuilder &sb, const std::string &prefix) const
{
    sb.Append(prefix).AppendFormat("%s %s = new Hash%s();\n", EmitJavaType(TypeMode::NO_MODE).c_str(), name.c_str(),
        EmitJavaType(TypeMode::NO_MODE).c_str());
    sb.Append(prefix).AppendFormat("int %sSize = %s.readInt();\n", name.c_str(), parcelName.c_str());
    sb.Append(prefix).AppendFormat("for (int i = 0; i < %sSize; ++i) {\n", name.c_str());

    keyEmitter_->EmitJavaReadInnerVar(parcelName, "key", true, sb, prefix + TAB);
    valueEmitter_->EmitJavaReadInnerVar(parcelName, "value", true, sb, prefix + TAB);
    sb.Append(prefix + TAB).AppendFormat("%s.put(key, value);\n", name.c_str());
    sb.Append(prefix).Append("}\n");
}

void HdiMapTypeEmitter::EmitCWriteMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    keyEmitter_->EmitCWriteMethods(methods, prefix, methodPrefix, isDecl);
    valueEmitter_->EmitCWriteMethods(methods, prefix, methodPrefix, isDecl);
}

void HdiMapTypeEmitter::EmitCReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    keyEmitter_->EmitCReadMethods(methods, prefix, methodPrefix, isDecl);
    valueEmitter_->EmitCReadMethods(methods, prefix, methodPrefix, isDecl);
}

void HdiMapTypeEmitter::EmitCStubReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    keyEmitter_->EmitCStubReadMethods(methods, prefix, methodPrefix, isDecl);
    valueEmitter_->EmitCStubReadMethods(methods, prefix, methodPrefix, isDecl);
}

void HdiMapTypeEmitter::EmitCppReadMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    keyEmitter_->EmitCppReadMethods(methods, prefix, methodPrefix, isDecl);
    valueEmitter_->EmitCppReadMethods(methods, prefix, methodPrefix, isDecl);
}

void HdiMapTypeEmitter::EmitCppWriteMethods(UtilMethodMap &methods, const std::string &prefix,
    const std::string &methodPrefix, bool isDecl) const
{
    keyEmitter_->EmitCppWriteMethods(methods, prefix, methodPrefix, isDecl);
    valueEmitter_->EmitCppWriteMethods(methods, prefix, methodPrefix, isDecl);
}
} // namespace Idl
} // namespace OHOS