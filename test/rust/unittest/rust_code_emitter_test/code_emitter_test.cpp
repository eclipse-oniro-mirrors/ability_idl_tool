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
#include <gtest/gtest.h>
#include "idl_common.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Idl::TestCommon;

namespace OHOS {
namespace Idl {
namespace UnitTest {
class RustCodeEmitterTest : public testing::Test, public IdlCommon {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RustCodeEmitterTest::SetUpTestCase()
{}

void RustCodeEmitterTest::TearDownTestCase()
{}

void RustCodeEmitterTest::SetUp()
{}

void RustCodeEmitterTest::TearDown()
{}

/*
 * Feature: idl
 * Function: RustCodeEmitter
 * SubFunction: NA
 * FunctionPoints: Check whether the emitter is existed.
 * EnvConditions: NA
 * CaseDescription: RustCodeEmitter is existed.
 */
HWTEST_F(RustCodeEmitterTest, IDLRust_Generator_0100, TestSize.Level1)
{
    EXPECT_EQ(PrepareIdlFile(UNKNOW_TYPE_IDL_NAME.c_str(), UNKNOW_TYPE_CASE1_IDL_CONTENT.c_str()), ERR_OK);
    int argc = 6;
    const char* argvArray[] = {"./idl", "-c", UNKNOW_TYPE_IDL_NAME.c_str(), "-gen-rust", "-d", "."};
    ParameterArgv parameters(argvArray, argc);
    EXPECT_EQ(Ready(argc, parameters.GetArgv()), ERR_OK);
    EXPECT_NE(rustCodeGen_, nullptr);
    StringBuilder sb;
    rustCodeGen_->EmitInterface(sb);
    EXPECT_FALSE(sb.ToString().IsEmpty());
    EXPECT_NE(sb.ToString().IndexOf("Copyright"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("use ipc_rust::"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("FIRST_CALL_TRANSACTION"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("define_remote_object"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IRemoteBroker"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("on_remote_request"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IdlTestStub"), ERR_FAIL);
    EXPECT_NE(sb.ToString().IndexOf("IdlTestProxy"), ERR_FAIL);
}

/*
 * Feature: idl
 * Function: EmitHeaders_0100
 * SubFunction: NA
 * FunctionPoints: Check whether the RustCodeEmitter is existed.
 * EnvConditions: NA
 * CaseDescription: RustCodeEmitter is existed.
 */
HWTEST_F(RustCodeEmitterTest, EmitHeaders_0100, TestSize.Level1)
{
    MetaComponent mc;
    RustCodeEmitter* rustCode = new RustCodeEmitter(&mc);
    StringBuilder sb;

    char mptSeqName[] = "includedir.Test..test.myseq";
    char mptIntfName[] = "includedir.Test..test.myinterface";
    MetaType mtMap = {TypeKind::Map, 0, 0, 0};
    rustCode->metaComponent_->typeNumber_ = 1;
    rustCode->metaComponent_->types_ = new MetaType* [1] {&mtMap};
    MetaSequenceable mSeq;
    mSeq.name_ = mptSeqName;
    rustCode->metaComponent_->sequenceableNumber_ = 1;
    rustCode->metaComponent_->sequenceables_ = new MetaSequenceable* [1] {&mSeq};

    MetaInterface mIntf;
    mIntf.name_ = mptIntfName;
    rustCode->metaComponent_->interfaceNumber_ = 1;
    rustCode->metaComponent_->interfaces_ = new MetaInterface* [1] {&mIntf};

    rustCode->EmitHeaders(sb);
    std::string expectedCode =
    "use std::collections::HashMap;\n"
    "\n"
    "extern crate ipc_rust;\n"
    "\n"
    "use ipc_rust::{\n"
    "    IRemoteBroker, IRemoteObj, RemoteStub, Result,\n"
    "    RemoteObj, define_remote_object, FIRST_CALL_TRANSACTION\n"
    "};\n"
    "use ipc_rust::{MsgParcel, BorrowedMsgParcel};\n"
    "\n"
    "use includedir::Test::{test, myseq};\n"
    "\n";

    delete [] rustCode->metaComponent_->types_;
    delete [] rustCode->metaComponent_->sequenceables_;
    delete [] rustCode->metaComponent_->interfaces_;
    delete rustCode;
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
}

/*
 * Feature: idl
 * Function: ReadFromParcel_0100
 * SubFunction: NA
 * FunctionPoints: Check whether the RustCodeEmitter is existed.
 * EnvConditions: NA
 * CaseDescription: RustCodeEmitter is existed.
 */
HWTEST_F(RustCodeEmitterTest, ReadFromParcel_0100, TestSize.Level1)
{
    MetaComponent mc;
    RustCodeEmitter* rustCode = new RustCodeEmitter(&mc);
    StringBuilder sb;
    char reply[] = "reply";
    char result[] = "result";

    int* nestedTypeIndexes = new int[2] {0, 1};
    MetaType mtMap = {TypeKind::Map, 0, 0, nestedTypeIndexes};
    MetaType mtList = {TypeKind::List, 0, 0, nestedTypeIndexes};
    MetaType mtArray = {TypeKind::Array, 0, 0, nestedTypeIndexes};

    MetaType* mtInteger = new MetaType{TypeKind::Integer, 0, 0, nullptr};
    MetaType* mtString = new MetaType{TypeKind::String, 0, 0, nullptr};
    rustCode->metaComponent_->types_ = new MetaType* [2] {mtInteger, mtString};
    rustCode->ReadFromParcel(sb, &mtMap, reply, result, "");
    rustCode->ReadFromParcel(sb, &mtList, reply, result, "");
    rustCode->ReadFromParcel(sb, &mtArray, reply, result, "");

    rustCode->WriteToParcel(sb, &mtMap, reply, result, "");
    rustCode->WriteToParcel(sb, &mtList, reply, result, "");
    rustCode->WriteToParcel(sb, &mtArray, reply, result, "");

    std::string expectedCode =
    "let mut result = HashMap::new();\n"
    "let len = reply.read()?;\n"
    "for i in 0..len {\n"
    "    let resultk : i32 = reply.read()?;\n"
    "    let resultv : String = reply.read()?;\n"
    "    result.insert(resultk, resultv);\n"
    "}\n"
    "let result : Vec<i32> = reply.read()?;\n"
    "let result : Vec<i32> = reply.read()?;\n"
    "reply.write(&(result.len() as u32))?;\n"
    "for (key, value) in result.iter() {\n"
    "    reply.write(&key)?;\n"
    "    reply.write(&value)?;\n"
    "}\n"
    "reply.write(&result)?;\n"
    "reply.write(&result)?;\n";
    delete mtInteger;
    delete mtString;
    delete [] nestedTypeIndexes;
    delete [] rustCode->metaComponent_->types_;
    delete rustCode;
    EXPECT_STREQ(sb.buffer_, expectedCode.c_str());
}
}  // namespace UnitTest
}  // namespace Idl
}  // namespace OHOS
