#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
from base import Test, exec_command, print_failure


class big_d_2000_file_test(Test):
    def __init__(self):
        super().__init__()
        self.gen_file_num = 2000

    def get_file_name(self):
        return __file__

    def get_fool_content(self, loop):
        content = "package ohos.hdi.foo.v1_0;\n"
        content += "\n"
        content += f"import ohos.hdi.foo.v1_0.Types_{loop};\n"
        content += "\n"
        content += f"interface IFoo_{loop} "
        content += "{\n"
        content += "    Ping([in] String sendMsg,[out] String recvMsg);\n"
        content += "\n"
        content += f"    GetData([out] struct FooInfo_{loop} info);\n"
        content += "\n"
        content += f"    InfoTest([in] struct FooInfo_{loop} inParam, [out] struct FooInfo_{loop} outParam);\n"
        content += "};"
        return content

    def get_types_content(self, loop):
        content = "package ohos.hdi.foo.v1_0;\n"
        content += "\n"
        content += f"enum FooType_{loop} "
        content += "{\n"
        content += "    FOO_TYPE_ONE = 1,\n"
        content += "    FOO_TYPE_TWO = 2,\n"
        content += "};\n"
        content += "\n"
        content += f"struct FooInfo_{loop} "
        content += "{\n"
        content += "    unsigned int id;\n"
        content += "    String name;\n"
        content += f"    enum FooType_{loop} type;\n"
        content += "};"
        return content

    def file_gen(self):
        idl_dir = os.path.join(self.get_working_dir(), "foo", "v1_0")
        for loop in range(self.gen_file_num):
            fool_file_conntent = self.get_fool_content(loop)
            types_file_conntent = self.get_types_content(loop)
            foo_file_dst_name = os.path.join(idl_dir, f"IFoo_{loop}.idl")
            type_file_dst_name = os.path.join(idl_dir, f"Types_{loop}.idl")
            with os.fdopen(os.open(foo_file_dst_name, os.O_CREAT | os.O_WRONLY, 0o644),
                           mode='w', encoding='utf-8') as fd:
                fd.write(fool_file_conntent)
            with os.fdopen(os.open(type_file_dst_name, os.O_CREAT | os.O_WRONLY, 0o644),
                           mode='w', encoding='utf-8') as fd:
                fd.write(types_file_conntent)

    def file_remove(self):
        idl_dir = os.path.join(self.get_working_dir(), "foo", "v1_0")
        for loop in range(self.gen_file_num):
            foo_file_dst_name = os.path.join(idl_dir, f"IFoo_{loop}.idl")
            type_file_dst_name = os.path.join(idl_dir, f"Types_{loop}.idl")
            os.remove(foo_file_dst_name)
            os.remove(type_file_dst_name)

    def set_idl_attr(self):
        work_dir = self.get_working_dir()
        self.set_package_path(work_dir)
        idl_dir = os.path.join(work_dir, "foo", "v1_0")
        self.set_idl_dir(idl_dir)
        self.set_command_attr(f"-D {idl_dir}")
        self.set_not_use_default_add_idl_file(True)
        self.set_system_attr("full")
        self.set_mode_attr("ipc")
        self.set_gen_cpp_env()
        self.update_command()

    def run_cpp(self):
        self.file_gen()
        self.set_idl_attr()
        status, ret = exec_command(self.command)
        self.file_remove()
        if status != 0:
            print_failure(f"[ERROR] command:{self.command} run err")
            print_failure(f"[ERROR] {ret}")
            return False
        return True

    def run(self):
        return self.run_cpp()


if __name__ == "__main__":
    big_d_2000_file_test().test()

