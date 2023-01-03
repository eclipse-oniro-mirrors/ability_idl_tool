/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
void RustCodeEmitter::EmitInterface()
{
    String filePath = String::Format("%s/%s.rs", directory_.string(), FileName(interfaceName_).string());
    File file(filePath, File::WRITE);
    
    StringBuilder sb;

    if (metaInterface_->license_) {
        EmitLicense(sb);
        sb.Append("\n");
    }

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
    sb.Append("\n");

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

String RustCodeEmitter::FileName(const String& name)
{
    if (name.IsEmpty()) {
        return name;
    }

    StringBuilder sb;

    for (int i = 0; i < name.GetLength(); i++) {
        char c = name[i];
        if (isupper(c) != 0) {
            if (i > 1 && name[i - 1] != '.' && name[i - 2] != '.') {
                sb.Append('_');
            }
            sb.Append(tolower(c));
        } else {
            sb.Append(c);
        }
    }

    return sb.ToString().Replace('.', '/');
}

void RustCodeEmitter::EmitLicense(StringBuilder& sb)
{
    sb.Append(metaInterface_->license_).Append("\n");
}

void RustCodeEmitter::EmitHeaders(StringBuilder& sb)
{
    EmitCommonHeaders(sb);
    sb.Append("\n");
    EmitIPCHeaders(sb);
    sb.Append("\n");
    if (EmitCustomHeaders(sb)) {
        sb.Append("\n");
    }
}

void RustCodeEmitter::EmitIPCHeaders(StringBuilder& sb)
{
    sb.Append("extern crate ipc_rust;\n");
    sb.Append("use ipc_rust::{\n");
    sb.Append("    IRemoteBroker, IRemoteObj, RemoteStub,\n");
    sb.Append("    RemoteObj, define_remote_object, FIRST_CALL_TRANSACTION\n");
    sb.Append("};\n");
    sb.Append("use ipc_rust::{MsgParcel, BorrowedMsgParcel}; \n");
}

void RustCodeEmitter::EmitCommonHeaders(StringBuilder& sb)
{
    bool useMap = false;
    for (int i = 0; i < metaComponent_->typeNumber_; i++) {
        MetaType* mt = metaComponent_->types_[i];
        switch (mt->kind_)
        {
            case TypeKind::Map: {
                if (!useMap) {
                    sb.Append("use std::collections::HashMap;\n");
                    useMap = true;
                }
            }
            break;
        
        default:
            break;
        }
    }
}

String RustCodeEmitter::TrimDot(const String& fpnp)
{
    if (fpnp.IsEmpty()) {
        return nullptr;
    }

    int left = 0;
    int right = fpnp.GetLength() - 1;
    while(fpnp[left] == ' ' || fpnp[left] == '.') {
        left++;
    }

    while(fpnp[right] == ' ' || fpnp[right] == '.') {
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
    if (!result.IsEmpty()) {
        sb.Append("use ").Append(result).Append(";\n");
        return true;
    }
    return false;
}

bool RustCodeEmitter::EmitCustomHeaders(StringBuilder& sb)
{
    bool custom = false;
    for (int i = 0; i < metaComponent_->sequenceableNumber_; i++) {
        MetaSequenceable* mp = metaComponent_->sequenceables_[i];
        custom |= AppendRealPath(sb, String(mp->namespace_) + String(mp->name_));
    }

    for (int i = 0; i < metaComponent_->interfaceNumber_; i++) {
        MetaInterface* mi = metaComponent_->interfaces_[i];
        if (mi->external_) {
            custom |= AppendRealPath(sb, String(mi->namespace_) + String(mi->name_));
        }
    }
    return custom;
}

void RustCodeEmitter::EmitCommands(StringBuilder& sb)
{
    EmitCommandEnums(sb);
    sb.Append("\n");
    EmitCommandImpls(sb);
}

void RustCodeEmitter::AppendCommandEnums(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.Append("    ");
        sb.AppendFormat("%s = FIRST_CALL_TRANSACTION + %d", GetCodeFromMethod(mm->name_).string(), i);
        sb.Append(",\n");
    }
}

String RustCodeEmitter::GetCodeFromMethod(const char* name)
{
    String code = "Code";
    return code + name;
}

void RustCodeEmitter::EmitCommandEnums(StringBuilder& sb)
{
    sb.AppendFormat("pub enum %sCode {\n", interfaceName_.string());
    AppendCommandEnums(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::EmitCommandImpls(StringBuilder& sb)
{
    sb.AppendFormat("impl %sCode {\n", interfaceName_.string());
    sb.Append("    fn to_u32(self) -> u32 {\n");
    sb.Append("        self as u32\n");
    sb.Append("    }\n");
    sb.Append("}\n");
}

void RustCodeEmitter::EmitRemoteObject(StringBuilder& sb)
{
    sb.Append("define_remote_object!(\n");
    sb.AppendFormat("    %s[\"%s\"] {\n", interfaceName_.string(), interfaceFullName_.string());
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

void RustCodeEmitter::AppendBrokerMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    fn %s(&self", mm->name_);
        for (int i = 0; i < mm->parameterNumber_; i++) {
            sb.Append(", ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }
        
        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat(", result: &mut %s", ConvertType(mt).string());
        }
        sb.Append(") -> u32");
        sb.Append(";\n");
    }
}

void RustCodeEmitter::AppendBrokerParameters(StringBuilder& sb, MetaParameter* mp) {
    sb.Append(mp->name_);
    sb.Append(": ");
    MetaType* mt = metaComponent_->types_[mp->typeIndex_];
    if (mp->attributes_ & ATTR_OUT) {
        sb.Append("&mut ");
    } else {
        sb.Append("&");
    }
    sb.Append(ConvertType(mt).string());
}

String RustCodeEmitter::ConvertType(MetaType* mt) {
    switch (mt->kind_) {
        case TypeKind::Unknown:
            return "()";
        case TypeKind::Char:
            return "char";
        case TypeKind::Boolean:
            return "bool";
        case TypeKind::Byte:
            return "u8";
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
            return "String";
        case TypeKind::Void:
            return "()";
        case TypeKind::Sequenceable: {
            MetaSequenceable* mp = metaComponent_->sequenceables_[mt->index_];
            return mp->name_;
        }
        case TypeKind::Interface: {
            MetaInterface* mi = metaComponent_->interfaces_[mt->index_];
            return mi->name_;
        }
        case TypeKind::List: {
            MetaType* elementType = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return String::Format("Vec<%s>", ConvertType(elementType).string());
        }
        case TypeKind::Map: {
            MetaType* keyType = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            MetaType* valueType = metaComponent_->types_[mt->nestedTypeIndexes_[1]];
            return String::Format("HashMap<%s, %s>", ConvertType(keyType).string(), ConvertType(valueType).string());
        }
        case TypeKind::Array: {
            MetaType* elementType = metaComponent_->types_[mt->nestedTypeIndexes_[0]];
            return String::Format("Vec<%s>", ConvertType(elementType).string());
        }
        default:
            return "()";
    }
}

void RustCodeEmitter::EmitRemoteRequest(StringBuilder& sb)
{
    sb.Append("fn on_remote_request(");
    sb.AppendFormat("stub: &dyn %s, ", interfaceName_.string());
    sb.Append("code: u32, ");
    sb.Append("data: &BorrowedMsgParcel,\n");
    sb.Append("    reply: &mut BorrowedMsgParcel) -> i32 {\n");
    sb.Append("    match code {\n");
    AddRemoteRequestMethods(sb);
    sb.Append("        _ => -1\n");
    sb.Append("    }\n");
    sb.Append("}\n");
}

void RustCodeEmitter::AddRemoteRequestMethods(StringBuilder& sb)
{
    for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("        %d => {\n", i + 1);
        for (int j = 0; j < mm->parameterNumber_; j++) {
            MetaParameter* mp = mm->parameters_[j];
            sb.Append("            let ");
            if (mp->attributes_ & ATTR_OUT) {
                sb.AppendFormat("mut ");
            }
            sb.Append(mp->name_);
            sb.AppendFormat(" : %s", ConvertType(metaComponent_->types_[mp->typeIndex_]).string());
            sb.Append(" = data.read();\n");
        }

        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat("            let mut result : %s = data.read();\n", ConvertType(mt).string());
        }

        sb.AppendFormat("            let value = stub.%s(", mm->name_);
        for (int j = 0; j < mm->parameterNumber_; j++) {
            MetaParameter* mp = mm->parameters_[j];
            if (mp->attributes_ & ATTR_OUT) {
                sb.Append("&mut ");
            } else {
                sb.Append("&");
            }
            sb.Append(mp->name_);
            if (j != mm->parameterNumber_ - 1) {
                sb.Append(", ");
            }
        }
    
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat(", &mut result", ConvertType(mt).string());
        }

        sb.Append(")\n");
        for (int j = 0; j < mm->parameterNumber_; j++) {
            MetaParameter* mp = mm->parameters_[j];
            if (mp->attributes_ & ATTR_OUT) {
                sb.AppendFormat("            reply.write(&%s);\n", mp->name_);
            }
        }
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.Append("            reply.write(&result);\n");
        }
        sb.Append("            reply.write(&value);\n");
        sb.Append("            0\n");
        sb.Append("        }\n");

        if (i != metaInterface_->methodNumber_ - 1) {
            sb.Append("\n");
        }
    }
}

void RustCodeEmitter::EmitStub(StringBuilder& sb)
{
    sb.AppendFormat("impl %s for RemoteStub<%s> {\n", interfaceName_.string(), stubName_.string());
    AppendStubMethods(sb);
    sb.Append("}\n");
}

void RustCodeEmitter::AppendStubMethods(StringBuilder& sb)
{
     for (int i = 0; i < metaInterface_->methodNumber_; i++) {
        MetaMethod* mm = metaInterface_->methods_[i];
        sb.AppendFormat("    fn %s(&self", mm->name_);
        for (int i = 0; i < mm->parameterNumber_; i++) {
            sb.Append(", ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }
        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat(", result: &mut %s", ConvertType(mt).string());
        }
        sb.Append(") -> u32 {\n");
        sb.AppendFormat("        self.0.%s(", mm->name_);
        for (int j = 0; j < mm->parameterNumber_; j++) {
            sb.Append(mm->parameters_[j]->name_);
            if (j != mm->parameterNumber_ - 1) {
                sb.Append(", ");
            }
        }
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.Append(", result");
        }
        sb.Append(")\n");
        sb.Append("    }\n");
        if (i != metaInterface_->methodNumber_ - 1) {
            sb.Append("\n");
        }
    }
}

void RustCodeEmitter::EmitProxy(StringBuilder& sb)
{
    sb.Append("// proxy\n");
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
            sb.Append(", ");
            AppendBrokerParameters(sb, mm->parameters_[i]);
        }
        MetaType* mt = metaComponent_->types_[mm->returnTypeIndex_];
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.AppendFormat(", result: &mut %s", ConvertType(mt).string());
        }
        sb.Append(") -> u32 {\n");
        sb.Append("        let mut data = MsgParcel::new();\n");

        for (int j = 0; j < mm->parameterNumber_; j++) {
            MetaParameter* mp = mm->parameters_[j];
            // if (mp->attributes_ & ATTR_IN) {
                sb.AppendFormat("        data.write(%s);\n", mp->name_);
            // }
        }
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.Append("        data.write(result);\n");
        }
        sb.AppendFormat("        let reply = self.remote.send_request(%s", interfaceName_.string());
        sb.AppendFormat("::%s as u32, &data, ", GetCodeFromMethod(mm->name_).string());
        if ((mm->properties_ & METHOD_PROPERTY_ONEWAY) != 0) {
            sb.Append("false");
        } else {
            sb.Append("true");
        }
        sb.Append(");\n");
        sb.Append("        match reply {\n");
        sb.Append("            Ok(reply) => {\n");

        for (int j = 0; j < mm->parameterNumber_; j++) {
            MetaParameter* mp = mm->parameters_[j];
            if (mp->attributes_ & ATTR_OUT) {
                sb.AppendFormat("                %s:  = reply.read();\n", mp->name_);
            }
        }
        if (mt->kind_ != TypeKind::Unknown && mt->kind_ != TypeKind::Void) {
            sb.Append("                result:  = reply.read();\n");
        }
        sb.Append("                reply.read()\n");
        sb.Append("            }\n");
        sb.Append("            Err(error) => {\n");
        sb.Append("                error\n");
        sb.Append("            }\n");
        sb.Append("        }\n");
        sb.Append("    }\n");

        if (i != metaInterface_->methodNumber_ - 1) {
            sb.Append("\n");
        }
    }
}
}
}