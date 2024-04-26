/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "util/logger.h"

#include "codegen/rust_code_emitter.h"

#include "securec.h"
#include "util/file.h"

namespace OHOS {
namespace Idl {
namespace {
const uint32_t WRAP_ANCHOR = 4;
}

void RustCodeEmitter::EmitInterface()
{
    String filePath = String::Format("%s/%s.rs", directory_.string(), interfaceName_.string());
    File file(filePath, File::WRITE);
    StringBuilder sb;
    EmitInterface(sb);
    String data = sb.ToString();
    file.WriteData(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

void RustCodeEmitter::EmitInterfaceProxy()
{
    return;
}

void RustCodeEmitter::EmitInterfaceStub()
{
    return;
}

void RustCodeEmitter::EmitInterface(StringBuilder& sb)
{
    if (metaInterface_->license_) {
        EmitLicense(sb);
        sb.Append("\n");
    }
    EmitMacros(sb);
    EmitHeaders(sb);
    sb.Append("\n");
    EmitCommands(sb);
    sb.Append("\n");
    EmitRemoteObject(sb);
    sb.Append("\n");
    EmitBrokers(sb);
    sb.Append("\n");
    EmitRemoteRequest(sb);
    sb.Append("\n");
    EmitStub(sb);
    sb.Append("\n");
    EmitProxy(sb);
}

void RustCodeEmitter::EmitLicense(StringBuilder& sb)
{
    sb.Append(metaInterface_->license_).Append("\n");
}

void RustCodeEmitter::EmitMacros(StringBuilder& sb)
{
    sb.Append("#![allow(missing_docs)]\n");
    sb.Append("#![allow(unused_variables)]\n");
    sb.Append("#![allow(unused_mut)]\n");
    sb.Append("\n");
}
void RustCodeEmitter::EmitHeaders(StringBuilder& sb)
{
    EmitCommonHeaders(sb);
    EmitIPCHeaders(sb);
    if (EmitCustomHeaders(sb)) {
        sb.Append("\n");
    }
}

void RustCodeEmitter::EmitIPCHeaders(StringBuilder& sb)
{
    sb.Append("extern crate ipc_rust;\n");
    sb.Append("\n");
    sb.Append("use ipc_rust::{\n");
    sb.Append("    IRemoteBroker, IRemoteObj, RemoteStub, Result,\n");
    sb.Append("    RemoteObj, define_remote_object, FIRST_CALL_TRANSACTION\n");
    sb.Append("};\n");
    sb.Append("use ipc_rust::{MsgParcel, BorrowedMsgParcel};\n");
    sb.Append("\n");
}

void RustCodeEmitter::EmitCommonHeaders(StringBuilder& sb)
{
    bool useMap = false;
    for (int i = 0; i < metaComponent_->typeNumber_; i++) {
        MetaType* mt = metaComponent_->types_[i];
        switch (mt->kind_) {
            case TypeKind::Map: {
                if (!useMap) {
                    sb.Append("use std::collections::HashMap;\n");
                    useMap = true;
                }
                break;
            }

            default:
                break;
        }
    }
    if (useMap) {
        sb.Append("\n");
    }
}

String RustCodeEmitter::TrimDot(const String& fpnp)
{
    if (fpnp.IsEmpty()) {
        return nullptr;
    }

    int left = 0;
    int right = fpnp.GetLength() - 1;
    while (fpnp[left] == ' ' || fpnp[left] == '.') {
        left++;
    }

    while (fpnp[right] == ' ' || fpnp[right] == '.') {
        right--;
    }

    if (left >= right) {
        return nullptr;
    }

    return fpnp.Substring(left, right + 1);
}

String RustCodeEmitter::GeneratePath(const String& fpnp)
{
    int pos = fpnp.IndexOf("..");
    if (pos == -1) {
        String path = TrimDot(fpnp);
        if (path.IsEmpty()) {
            return nullptr;
        }
        return path.Replace(".", "::");
    }

    String path = TrimDot(fpnp.Substring(0, pos + 1));
    String file = TrimDot(fpnp.Substring(pos));
    if (path.IsEmpty()) {
        return nullptr;
    }

    if (path.IndexOf("..") != -1 || file.IndexOf("..") != -1) {
        return nullptr;
    }

    StringBuilder realPath;
    realPath.Append(path.Replace(".", "::")).Append("::{");
    realPath.Append(file.Replace(".", ", "));
    realPath.Append("}");

    return realPath.ToString();
}

bool RustCodeEmitter::AppendRealPath(StringBuilder& sb, const String& fpnpp)
{
    String result = GeneratePath(fpnpp);
    if (result.IsEmpty()) {
        return false;
    }
    sb.Append("use ").Append(result).Append(";\n");
    return true;
}

bool RustCodeEmitter::EmitCustomHeaders(StringBuilder& sb)
{
    uint32_t custom = false;
    for (int i = 0; i < metaComponent_->sequenceableNumber_; i++) {
        MetaSequenceable* ms = metaComponent_->sequenceables_[i];
        bool addPathMsRes = AppendRealPath(sb, String(ms->namespace_) + String(ms->name_));
        custom |= static_cast<uint32_t>(addPathMsRes);
    }

    for (int i = 0; i < metaComponent_->interfaceNumber_; i++) {
        MetaInterface* mi = metaComponent_->interfaces_[i];
        if (mi->external_) {
            bool addPathMiRes = AppendRealPath(sb, String(mi->namespace_) + String(mi->name_));
            custom |= static_cast<uint32_t>(addPathMiRes);
        }
    }
    return static_cast<bool>(custom);
}

void RustCodeEmitter::EmitCommands(StringBuilder& sb)
{
    EmitCommandEnums(sb);
}

void RustCodeEmitter::AppendCommandEnums(StringBuilder& sb)
{
    if (metaInterface_->methodNumber_ > 0) {
        sb.AppendFormat("    %s  = FIRST_CALL_TRANSACTION,\n",
            GetCodeFromMethod(metaInterface_->methods_[0]->name_).string());
    }

    for (int i = 1; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    %s,\n", GetCodeFromMethod(mm->name_).string(), i);
    }
}

String RustCodeEmitter::GetCodeFromMethod(const char* name)
{
    StringBuilder sb;
    sb.Append("Code");
    const char* p = name;
    bool hasUpper = false;
    while (p != nullptr && *p != '\0') {
        if (*p != '_') {
            if (!hasUpper) {
                sb.Append(toupper(*p));
                hasUpper = true;
            } else {
                sb.Append(*p);
            }
        } else {
            hasUpper = false;
        }
        p++;
    }
    return sb.ToString();
}

String RustCodeEmitter::GetNameFromParameter(const char* name)
{
    StringBuilder sb;
    const char* p = name;
    bool start = true;
    while (p != nullptr && *p != '\0') {
        if (start) {
            if (isupper(*p)) {
                sb.Append('p');
            }
            start = false;
        }

        if (isupper(*p)) {
            sb.Append('_');
            sb.Append(tolower(*p));
        } else {
            sb.Append(*p);
        }
        p++;
    }
    return sb.ToString();
}

void RustCodeEmitter::EmitCommandEnums(StringBuilder& sb)
{
    sb.AppendFormat("pub enum %sCode {\n", interfaceName_.string());
    AppendCommandEnums(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::EmitRemoteObject(StringBuilder& sb)
{
    sb.Append("define_remote_object!(\n");
    if (interfaceFullName_.StartsWith(".")) {
        sb.AppendFormat("    %s[\"%s\"] {\n", interfaceName_.string(), interfaceName_.string());
    } else {
        sb.AppendFormat("    %s[\"%s\"] {\n", interfaceName_.string(), interfaceFullName_.string());
    }
    sb.AppendFormat("        stub: %s(on_remote_request),\n", stubName_.string());
    sb.AppendFormat("        proxy: %s,\n", proxyName_.string());
    sb.Append("    }\n");
    sb.Append(");\n");
}

void RustCodeEmitter::EmitBrokers(StringBuilder& sb)
{
    sb.AppendFormat("pub trait %s: IRemoteBroker {\n", interfaceName_.string());
    AppendBrokerMethods(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::WrapLine(StringBuilder& sb, int index, const String& prefix)
{
    if ((index + 1) % WRAP_ANCHOR == 0) {
        sb.AppendFormat(",\n%s", prefix.string());
    } else {
        sb.Append(", ");
    }
}

void RustCodeEmitter::AppendBrokerMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    fn %s(&self", mm->name_);
        for (int i = 0; i < mm->parameterNumber_; i++) {
            WrapLine(sb, i, "        ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }
        sb.AppendFormat(") -> Result<%s>;\n", ConvertType(metaComponent_->types_[mm->returnTypeIndex_]).string());
    }
}

void RustCodeEmitter::AppendBrokerParameters(StringBuilder& sb, MetaParameter* mp)
{
    sb.AppendFormat("%s: &%s",
        GetNameFromParameter(mp->name_).string(), ConvertType(metaComponent_->types_[mp->typeIndex_], true).string());
}

String RustCodeEmitter::ConvertType(MetaType* mt, bool pt)
{
    switch (mt->kind_) {
        case TypeKind::Unknown:
        case TypeKind::Void:
            return "()";
        case TypeKind::Char:
            return "char";
        case TypeKind::Boolean:
            return "bool";
        case TypeKind::Byte:
            return "i8";
        case TypeKind::Short:
            return "i16";
        case TypeKind::Integer:
            return "i32";
        case TypeKind::Long:
            return "i64";
        case TypeKind::Float:
            return "f32";
        case TypeKind::Double:
            return "f64";
        case TypeKind::String:
            return pt ? "str" : "String";
        case TypeKind::Sequenceable:
            return  metaComponent_->sequenceables_[mt->index_]->name_;
        case TypeKind::Interface:
            return metaComponent_->interfaces_[mt->index_]->name_;
        case TypeKind::Map:
            return String::Format("HashMap<%s, %s>",
                ConvertType(metaComponent_->types_[mt->nestedTypeIndexes_[0]]).string(),
                ConvertType(metaComponent_->types_[mt->nestedTypeIndexes_[1]]).string());
        case TypeKind::List:
        case TypeKind::Array:
            return  String::Format((pt ? "[%s]" : "Vec<%s>"),
                ConvertType(metaComponent_->types_[mt->nestedTypeIndexes_[0]]).string());
        default:
            return "()";
    }
}

void RustCodeEmitter::EmitRemoteRequest(StringBuilder& sb)
{
    sb.AppendFormat("fn on_remote_request(stub: &dyn %s, code: u32, data: &BorrowedMsgParcel,\n",
        interfaceName_.string());
    sb.Append("    reply: &mut BorrowedMsgParcel) -> Result<()> {\n");
    sb.Append("    match code {\n");
    AddRemoteRequestMethods(sb);
    sb.Append("        _ => Err(-1)\n");
    sb.Append("    }\n");
    sb.Append("}\n");
}

void RustCodeEmitter::AddRemoteRequestParameters(StringBuilder& sb, MetaMethod* mm)
{
    for (int i = 0; i < mm->parameterNumber_; i++) {
        MetaParameter* mp = mm->parameters_[i];
        sb.AppendFormat("&%s", GetNameFromParameter(mp->name_).string());
        if (i + 1 != mm->parameterNumber_) {
            WrapLine(sb, i, "                ");
        }
    }
}

void RustCodeEmitter::ReadListFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    sb.Append(prefix).AppendFormat("let %s : %s = %s.read()?;\n",
        name.string(), ConvertType(mt).string(), result.string());
}

void RustCodeEmitter::ReadMapFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    sb.Append(prefix).AppendFormat("let mut %s = HashMap::new();\n", name.string());
    sb.Append(prefix).AppendFormat("let len = %s.read()?;\n", result.string());
    sb.Append(prefix).Append("for i in 0..len {\n");
    StringBuilder k;
    StringBuilder v;
    k.Append(name).Append("k");
    v.Append(name).Append("v");
    ReadFromParcel(sb, metaComponent_->types_[mt->nestedTypeIndexes_[0]],
        result, k.ToString().string(), prefix + "    ");
    ReadFromParcel(sb, metaComponent_->types_[mt->nestedTypeIndexes_[1]],
        result, v.ToString().string(), prefix + "    ");
    sb.Append(prefix + "    ").AppendFormat("%s.insert(%s, %s);\n",
        name.string(), k.ToString().string(), v.ToString().string());
    sb.Append(prefix).Append("}\n");
}

void RustCodeEmitter::ReadFromParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    if (mt->kind_ == TypeKind::Map) {
        ReadMapFromParcel(sb, mt, result, name, prefix);
    } else if (mt->kind_ == TypeKind::List || mt->kind_ == TypeKind::Array) {
        ReadListFromParcel(sb, mt, result, name, prefix);
    } else {
        sb.Append(prefix).AppendFormat("let %s : %s = %s.read()?;\n",
            name.string(), ConvertType(mt).string(), result.string());
    }
}

void RustCodeEmitter::WriteListToParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    sb.Append(prefix).AppendFormat("%s.write(&%s)?;\n", result.string(), name.string());
}

void RustCodeEmitter::WriteMapToParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    sb.Append(prefix).AppendFormat("%s.write(&(%s.len() as u32))?;\n", result.string(), name.string());
    sb.Append(prefix).AppendFormat("for (key, value) in %s.iter() {\n", name.string());
    WriteToParcel(sb, metaComponent_->types_[mt->nestedTypeIndexes_[0]], result, "key", prefix + "    ");
    WriteToParcel(sb, metaComponent_->types_[mt->nestedTypeIndexes_[1]], result, "value", prefix + "    ");
    sb.Append(prefix).Append("}\n");
}

void RustCodeEmitter::WriteToParcel(StringBuilder& sb, MetaType* mt, const String& result,
    const String& name, const String& prefix)
{
    if (mt->kind_ == TypeKind::Map) {
        WriteMapToParcel(sb, mt, result, name, prefix);
    } else if (mt->kind_ == TypeKind::List || mt->kind_ == TypeKind::Array) {
        WriteListToParcel(sb, mt, result, name, prefix);
    } else {
        sb.Append(prefix).AppendFormat("%s.write(&%s)?;\n", result.string(), name.string());
    }
}

void RustCodeEmitter::AddRemoteRequestMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("        %d => {\n", i + 1);
        for (int j = 0; j < mm->parameterNumber_; j++) {
            ReadFromParcel(sb, metaComponent_->types_[mm->parameters_[j]->typeIndex_], "data",
                GetNameFromParameter(mm->parameters_[j]->name_), "            ");
        }
        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat("            let result = stub.%s(", mm->name_);
        } else {
            sb.AppendFormat("            stub.%s(", mm->name_);
        }
        AddRemoteRequestParameters(sb, mm);
        sb.Append(")?;\n");
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            WriteToParcel(sb, mt, "reply", "result", "            ");
        }
        sb.Append("            Ok(())\n");
        sb.Append("        }\n");
    }
}

void RustCodeEmitter::EmitStub(StringBuilder& sb)
{
    sb.AppendFormat("impl %s for RemoteStub<%s> {\n", interfaceName_.string(), stubName_.string());
    AppendStubMethods(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::AppendStubParameters(StringBuilder& sb, MetaMethod* mm)
{
    for (int i = 0; i < mm->parameterNumber_; i++) {
        sb.Append(GetNameFromParameter(mm->parameters_[i]->name_));
        if (i + 1 != mm->parameterNumber_) {
            WrapLine(sb, i, "                ");
        }
    }
}

void RustCodeEmitter::AppendStubMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    fn %s(&self", mm->name_);
        for (int i = 0; i < mm->parameterNumber_; i++) {
            WrapLine(sb, i, "        ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }

        sb.AppendFormat(") -> Result<%s> {\n",
            ConvertType(metaComponent_->types_[mm->returnTypeIndex_]).string());
        sb.AppendFormat("        self.0.%s(", mm->name_);
        AppendStubParameters(sb, mm);
        sb.Append(")\n");
        sb.Append("    }\n");
        if (i != metaInterface_->methodNumber_ - 1) {
            sb.Append("\n");
        }
    }
}

void RustCodeEmitter::EmitProxy(StringBuilder& sb)
{
    sb.AppendFormat("impl %s for %s {\n", interfaceName_.string(), proxyName_.string());
    AppendProxyMethods(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::AppendProxyMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    fn %s(&self", mm->name_);
        for (int i = 0; i < mm->parameterNumber_; i++) {
            WrapLine(sb, i, "        ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }
        sb.AppendFormat(") -> Result<%s> {\n",
            ConvertType(metaComponent_->types_[mm->returnTypeIndex_]).string());
        sb.Append("        let mut data = MsgParcel::new().expect(\"MsgParcel should success\");\n");
        for (int j = 0; j < mm->parameterNumber_; j++) {
            WriteToParcel(sb, metaComponent_->types_[mm->parameters_[j]->typeIndex_], "data",
                GetNameFromParameter(mm->parameters_[j]->name_), "        ");
        }
        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ == TypeKind::Unknown || mt->kind_ == TypeKind::Void) {
            sb.AppendFormat("        let _reply = self.remote.send_request(%sCode", interfaceName_.string());
        } else {
            sb.AppendFormat("        let reply = self.remote.send_request(%sCode", interfaceName_.string());
        }
        sb.AppendFormat("::%s as u32, &data, ", GetCodeFromMethod(mm->name_).string());
        if ((mm->properties_ & METHOD_PROPERTY_ONEWAY) != 0) {
            sb.Append("true");
        } else {
            sb.Append("false");
        }
        sb.Append(")?;\n");
        if (mt->kind_ == TypeKind::Unknown || mt->kind_ == TypeKind::Void) {
            sb.Append("        ").Append("Ok(())\n");
        } else {
            ReadFromParcel(sb, mt, "reply", "result", "        ");
            sb.Append("        ").Append("Ok(result)\n");
        }
        sb.Append("    }\n");

        if (i != metaInterface_->methodNumber_ - 1) {
            sb.Append("\n");
        }
    }
}
}
}
