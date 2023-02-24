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

extern crate ipc_rust;
extern crate rust_idl_ipc_test_service;

use std::ptr;
use std::ffi::{c_char, CString};

use ipc_rust::{FromRemoteObj, RemoteObjRef, get_service};
use rust_idl_ipc_test_service::{IIdlTestService};

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

extern "C" {
    fn GetAccessTokenId(tokenInfo: *mut TokenInfoParams) -> u64;
    fn SetSelfTokenID(tokenID: u64) -> i32;
}

fn get_test_service() -> RemoteObjRef<dyn IIdlTestService>
{
    let object = get_service(IPC_TEST_SERVICE_ID).expect("get itest service failed");
    let remote = <dyn IIdlTestService as FromRemoteObj>::from(object);
    let remote = match remote {
        Ok(x) => x,
        Err(error) => {
            println!("convert RemoteObj to TestProxy failed: {}", error);
            panic!();
        }
    };
    remote
}

/// init access token for client
#[test]
fn init_access_token() {
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

#[test]
fn idl_ipc_test_basic_001() {
    let remote = get_test_service();
    remote.idl_ipc_test_basic_001().expect("should success");
    assert_eq!(true, true);
}

#[test]
fn idl_ipc_test_basic_002() {
    let remote = get_test_service();
    remote.idl_ipc_test_basic_002().expect("should success");
    assert_eq!(true, true);
}

#[test]
fn idl_ipc_test_basic_101() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_101(&true).expect("should success");
    assert!(!anchor);
}

#[test]
fn idl_ipc_test_basic_102() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_102(&12).expect("should success");
    println!("idl_ipc_test_basic_102: {}", anchor);
    assert_eq!(anchor, 24);
}

#[test]
fn idl_ipc_test_basic_103() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_103(&24).expect("should success");
    assert_eq!(anchor, 48);
}

#[test]
fn idl_ipc_test_basic_104() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_104(&48).expect("should success");
    assert_eq!(anchor, 96);
}

#[test]
fn idl_ipc_test_basic_105() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_105(&96).expect("should success");
    assert_eq!(anchor, 192);
}

#[test]
fn idl_ipc_test_basic_106() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_106(&1.7).expect("should success");
    assert!((anchor - 1.7 * 2.0).abs() < 0.1);
}

#[test]
fn idl_ipc_test_basic_107() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_107(&3.4).expect("should success");
    assert!((anchor - 3.4 * 2.0).abs() < 0.1);
}

#[test]
fn idl_ipc_test_basic_108() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_108("Basic").expect("should success");
    assert_eq!(anchor, "BasicBasic");
}

#[test]
fn idl_ipc_test_basic_199() {
    let remote = get_test_service();
    let anchor = remote.idl_ipc_test_basic_199(&true, &96, &1.7, &3.4, "Basic").expect("should success");
    assert!(anchor);
}