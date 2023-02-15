/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#![allow(missing_docs)]
#![allow(unused_variables)]
#![allow(unused_mut)]

extern crate ipc_rust;

use ipc_rust::{
    IRemoteBroker, IRemoteObj, RemoteStub, Result,
    RemoteObj, define_remote_object, FIRST_CALL_TRANSACTION
};
use ipc_rust::{MsgParcel, BorrowedMsgParcel};


pub enum IIdlTestServiceCode {
    CodeIdlIpcTestBasic001  = FIRST_CALL_TRANSACTION,
    CodeIdlIpcTestBasic002,
    CodeIdlIpcTestBasic101,
    CodeIdlIpcTestBasic102,
    CodeIdlIpcTestBasic103,
    CodeIdlIpcTestBasic104,
    CodeIdlIpcTestBasic105,
    CodeIdlIpcTestBasic106,
    CodeIdlIpcTestBasic107,
    CodeIdlIpcTestBasic108,
    CodeIdlIpcTestBasic199,
}

define_remote_object!(
    IIdlTestService["IIdlTestService"] {
        stub: IdlTestServiceStub(on_remote_request),
        proxy: IdlTestServiceProxy,
    }
);

pub trait IIdlTestService: IRemoteBroker {
    fn idl_ipc_test_basic_001(&self) -> Result<()>;
    fn idl_ipc_test_basic_002(&self) -> Result<()>;
    fn idl_ipc_test_basic_101(&self, i_bool: &bool) -> Result<bool>;
    fn idl_ipc_test_basic_102(&self, i_byte: &i8) -> Result<i8>;
    fn idl_ipc_test_basic_103(&self, i_short: &i16) -> Result<i16>;
    fn idl_ipc_test_basic_104(&self, i_int: &i32) -> Result<i32>;
    fn idl_ipc_test_basic_105(&self, i_long: &i64) -> Result<i64>;
    fn idl_ipc_test_basic_106(&self, i_float: &f32) -> Result<f32>;
    fn idl_ipc_test_basic_107(&self, i_double: &f64) -> Result<f64>;
    fn idl_ipc_test_basic_108(&self, i_string: &str) -> Result<String>;
    fn idl_ipc_test_basic_199(&self, i_bool: &bool, i_long: &i64, i_float: &f32,
        i_double: &f64, i_string: &str) -> Result<bool>;
}

fn on_remote_request(stub: &dyn IIdlTestService, code: u32, data: &BorrowedMsgParcel,
    reply: &mut BorrowedMsgParcel) -> Result<()> {
    match code {
        1 => {
            stub.idl_ipc_test_basic_001()?;
            Ok(())
        }
        2 => {
            stub.idl_ipc_test_basic_002()?;
            Ok(())
        }
        3 => {
            let i_bool : bool = data.read()?;
            let result = stub.idl_ipc_test_basic_101(&i_bool)?;
            reply.write(&result)?;
            Ok(())
        }
        4 => {
            let i_byte : i8 = data.read()?;
            let result = stub.idl_ipc_test_basic_102(&i_byte)?;
            reply.write(&result)?;
            Ok(())
        }
        5 => {
            let i_short : i16 = data.read()?;
            let result = stub.idl_ipc_test_basic_103(&i_short)?;
            reply.write(&result)?;
            Ok(())
        }
        6 => {
            let i_int : i32 = data.read()?;
            let result = stub.idl_ipc_test_basic_104(&i_int)?;
            reply.write(&result)?;
            Ok(())
        }
        7 => {
            let i_long : i64 = data.read()?;
            let result = stub.idl_ipc_test_basic_105(&i_long)?;
            reply.write(&result)?;
            Ok(())
        }
        8 => {
            let i_float : f32 = data.read()?;
            let result = stub.idl_ipc_test_basic_106(&i_float)?;
            reply.write(&result)?;
            Ok(())
        }
        9 => {
            let i_double : f64 = data.read()?;
            let result = stub.idl_ipc_test_basic_107(&i_double)?;
            reply.write(&result)?;
            Ok(())
        }
        10 => {
            let i_string : String = data.read()?;
            let result = stub.idl_ipc_test_basic_108(&i_string)?;
            reply.write(&result)?;
            Ok(())
        }
        11 => {
            let i_bool : bool = data.read()?;
            let i_long : i64 = data.read()?;
            let i_float : f32 = data.read()?;
            let i_double : f64 = data.read()?;
            let i_string : String = data.read()?;
            let result = stub.idl_ipc_test_basic_199(&i_bool, &i_long, &i_float, &i_double,
                &i_string)?;
            reply.write(&result)?;
            Ok(())
        }
        _ => Err(-1)
    }
}

impl IIdlTestService for RemoteStub<IdlTestServiceStub> {
    fn idl_ipc_test_basic_001(&self) -> Result<()> {
        self.0.idl_ipc_test_basic_001()
    }

    fn idl_ipc_test_basic_002(&self) -> Result<()> {
        self.0.idl_ipc_test_basic_002()
    }

    fn idl_ipc_test_basic_101(&self, i_bool: &bool) -> Result<bool> {
        self.0.idl_ipc_test_basic_101(i_bool)
    }

    fn idl_ipc_test_basic_102(&self, i_byte: &i8) -> Result<i8> {
        self.0.idl_ipc_test_basic_102(i_byte)
    }

    fn idl_ipc_test_basic_103(&self, i_short: &i16) -> Result<i16> {
        self.0.idl_ipc_test_basic_103(i_short)
    }

    fn idl_ipc_test_basic_104(&self, i_int: &i32) -> Result<i32> {
        self.0.idl_ipc_test_basic_104(i_int)
    }

    fn idl_ipc_test_basic_105(&self, i_long: &i64) -> Result<i64> {
        self.0.idl_ipc_test_basic_105(i_long)
    }

    fn idl_ipc_test_basic_106(&self, i_float: &f32) -> Result<f32> {
        self.0.idl_ipc_test_basic_106(i_float)
    }

    fn idl_ipc_test_basic_107(&self, i_double: &f64) -> Result<f64> {
        self.0.idl_ipc_test_basic_107(i_double)
    }

    fn idl_ipc_test_basic_108(&self, i_string: &str) -> Result<String> {
        self.0.idl_ipc_test_basic_108(i_string)
    }

    fn idl_ipc_test_basic_199(&self, i_bool: &bool, i_long: &i64, i_float: &f32,
        i_double: &f64, i_string: &str) -> Result<bool> {
        self.0.idl_ipc_test_basic_199(i_bool, i_long, i_float, i_double,
                i_string)
    }
}

impl IIdlTestService for IdlTestServiceProxy {
    fn idl_ipc_test_basic_001(&self) -> Result<()> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        let _reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic001 as u32, &data, false)?;
        Ok(())
    }

    fn idl_ipc_test_basic_002(&self) -> Result<()> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        let _reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic002 as u32, &data, true)?;
        Ok(())
    }

    fn idl_ipc_test_basic_101(&self, i_bool: &bool) -> Result<bool> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_bool)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic101 as u32, &data, false)?;
        let result : bool = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_102(&self, i_byte: &i8) -> Result<i8> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_byte)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic102 as u32, &data, false)?;
        let result : i8 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_103(&self, i_short: &i16) -> Result<i16> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_short)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic103 as u32, &data, false)?;
        let result : i16 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_104(&self, i_int: &i32) -> Result<i32> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_int)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic104 as u32, &data, false)?;
        let result : i32 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_105(&self, i_long: &i64) -> Result<i64> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_long)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic105 as u32, &data, false)?;
        let result : i64 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_106(&self, i_float: &f32) -> Result<f32> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_float)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic106 as u32, &data, false)?;
        let result : f32 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_107(&self, i_double: &f64) -> Result<f64> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_double)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic107 as u32, &data, false)?;
        let result : f64 = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_108(&self, i_string: &str) -> Result<String> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_string)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic108 as u32, &data, false)?;
        let result : String = reply.read()?;
        Ok(result)
    }

    fn idl_ipc_test_basic_199(&self, i_bool: &bool, i_long: &i64, i_float: &f32,
        i_double: &f64, i_string: &str) -> Result<bool> {
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        data.write(&i_bool)?;
        data.write(&i_long)?;
        data.write(&i_float)?;
        data.write(&i_double)?;
        data.write(&i_string)?;
        let reply = self.remote.send_request(IIdlTestServiceCode::CodeIdlIpcTestBasic199 as u32, &data, false)?;
        let result : bool = reply.read()?;
        Ok(result)
    }
}
