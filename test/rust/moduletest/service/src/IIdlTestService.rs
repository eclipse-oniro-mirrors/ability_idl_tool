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
}
