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
#![allow(unused_variables)]

//! IPC test server

extern crate ipc_rust;
extern crate rust_idl_ipc_test_service;

use std::ptr;
use std::ffi::{c_char, CString};

use ipc_rust::{IRemoteBroker, join_work_thread, Result, add_service};
use rust_idl_ipc_test_service::{IIdlTestService, IdlTestServiceStub};

const IPC_TEST_SERVICE_ID: i32 = 1118;

#[repr(C)]
struct TokenInfoParams {
    dcaps_num: i32,
    perms_num: i32,
    acls_num: i32,
    dcaps: *const *const c_char,
    perms: *const *const c_char,
    acls: *const *const c_char,
    process_name: *const c_char,
    apl_str: *const c_char,
}

fn init_access_token()
{
    let name = CString::new("com.ipc.test").expect("process name is invalid");
    let apl = CString::new("normal").expect("apl string is invalid");
    let mut param = TokenInfoParams {
        dcaps_num: 0,
        perms_num: 0,
        acls_num: 0,
        dcaps: ptr::null(),
        perms: ptr::null(),
        acls: ptr::null(),
        process_name: name.as_ptr(),
        apl_str: apl.as_ptr(),
    };

    unsafe {
        let token_id = GetAccessTokenId(&mut param as *mut TokenInfoParams);
        SetSelfTokenID(token_id);
    }
}

extern "C" {
    fn GetAccessTokenId(tokenInfo: *mut TokenInfoParams) -> u64;
    fn SetSelfTokenID(tokenID: u64) -> i32;
}

/// test.ipc.ITestService type
pub struct TestService;

impl IIdlTestService for TestService {
    fn idl_ipc_test_basic_001(&self) -> Result<()> {
        println!("idl_ipc_test_basic_001 start");
        Ok(())
    }

    fn idl_ipc_test_basic_002(&self) -> Result<()> {
        println!("idl_ipc_test_basic_002 start");
        Ok(())
    }
}

impl IRemoteBroker for TestService {}

fn main() {
    init_access_token();
    // create stub
    let service = IdlTestServiceStub::new_remote_stub(TestService).expect("create TestService success");
    add_service(&service.as_object().expect("get IIdlTestService service failed"),
        IPC_TEST_SERVICE_ID).expect("add server to samgr failed");
    println!("join to ipc work thread");
    join_work_thread();
}
