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
from .util import exec_command, compare_target_files, get_all_idl_files
from .util import get_time_stamp, print_success, print_failure, dump_ast_compare
from .util import get_idl, hdi_gen_fail_check_ignore_line


__all__ = ["Test"]

class Test:
    def __init__(self):
        self.name = self.__class__.__name__
        self._file_name = self.get_file_name()
        self.working_dir = self.get_working_dir()
        self._package_path = self.working_dir
        self.idl_dir = os.path.join(self.working_dir, "foo")
        self.output_dir = os.path.join(self.working_dir, "out")
        self.target_dir = os.path.join(self.working_dir, "target")
        self._system_attr = "full"
        self._mode_attr = "ipc"
        self._command_attr = ""
        self._command_ext_attr = ""
        self._gen_langauge = "--gen-cpp"
        self._idl = get_idl()
        self._command_format = "{} --intf-type hdi --system {} -m {} {} {} {} -r ohos.hdi:{} -d {}"
        self.command = self._command_format.format(self._idl, self._system_attr, self._mode_attr,
                                                   self._command_attr, self._command_ext_attr,
                                                   self._gen_langauge, self._package_path, self.output_dir)
        self._not_use_default_add_idl_file = False
        self._output_path = None
        self._hash_command_format = "{} --intf-type hdi --hash -r ohos.hdi:{} {} -o {}"
        self._dump_ast_command_format = "{} --intf-type hdi --dump-ast -r ohos.hdi:{} {} {} -o {}"
        self._dump_ast_check = False
        self._check_fail_ignore_line = False

    def get_file_name(self):
        # 子类继承实现
        return ""

    def set_check_fail_ignore_line(self, attr):
        self._check_fail_ignore_line = attr

    def get_working_dir(self):
        current_path = os.path.dirname(os.path.abspath(__file__))
        return os.path.realpath(os.path.join(current_path,
                                             "..", os.path.splitext(os.path.basename(self._file_name))[0]))

    def set_command_attr(self, attr):
        self._command_attr = attr

    def set_command_ext_attr(self, attr):
        self._command_ext_attr = attr

    def set_command_gen_langauge(self, langauge):
        self._gen_langauge = langauge

    def set_output_dir(self, output_dir):
        self.output_dir = os.path.join(self.working_dir, output_dir)

    def set_target_dir(self, target_dir):
       self.target_dir = os.path.join(self.working_dir, target_dir)

    def set_system_attr(self, attr):
        self._system_attr = attr

    def set_mode_attr(self, attr):
        self._mode_attr = attr

    def set_package_path(self, package_path):
        self._package_path = package_path

    def set_idl_dir(self, path):
        self.idl_dir = path

    def set_not_use_default_add_idl_file(self, opt):
        self._not_use_default_add_idl_file = opt

    def update_command(self):
        self.command = self._command_format.format(self._idl, self._system_attr, self._mode_attr,
                                                   self._command_attr, self._command_ext_attr,
                                                   self._gen_langauge, self._package_path, self.output_dir)

    def set_output_path(self, output_path):
        self._output_path = output_path

    def hash_command_update(self):
        hash_command = self._hash_command_format.format(self._idl, self._package_path,
                                                        self._command_attr, self._output_path)
        self.set_command(hash_command)

    def dump_ast_command_update(self, use_tee=True):
        dump_ast_command = self._dump_ast_command_format.format(self._idl, self._package_path, self._command_attr,
                                                                self._command_ext_attr, self._output_path)
        if use_tee:
            dump_ast_command += f" | tee {self._output_path}"
        self.set_command(dump_ast_command)

    def set_hash_param(self, package_path, command_attr):
        self.set_package_path(package_path)
        self.set_command_attr(f"-D {command_attr}")
        output_path = os.path.join(self.working_dir, "hash_out")
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        self.set_output_path(os.path.join(output_path, "hash.txt"))
        self.set_output_dir(output_path)
        self.set_target_dir(os.path.join(self.working_dir, "hash_target"))
        self.set_not_use_default_add_idl_file(True)
        self.hash_command_update()

    def set_dump_ast_check(self, if_check):
        self._dump_ast_check = if_check

    def set_dump_ast_param(self, package_path, command_attr, use_tee=True):
        self.set_package_path(package_path)
        self.set_command_attr(f"-D {command_attr}")
        output_path = os.path.join(self.working_dir, "dump_ast_out")
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        self.set_output_path(os.path.join(output_path, "dump.txt"))
        self.set_output_dir(output_path)
        self.set_target_dir(os.path.join(self.working_dir, "dump_ast_target"))
        self.set_not_use_default_add_idl_file(True)
        self.set_dump_ast_check(True)
        self.dump_ast_command_update(use_tee)

    def set_command(self, command):
        self.command = command

    def set_gen_c_env(self):
        self.set_command_gen_langauge("--gen-c")
        self.set_output_dir("c_out")
        self.set_target_dir("c_target")
        self.update_command()

    def set_gen_cpp_env(self):
        self.set_command_gen_langauge("--gen-cpp")
        self.set_output_dir("cpp_out")
        self.set_target_dir("cpp_target")
        self.update_command()

    def set_gen_java_env(self):
        self.set_command_gen_langauge("--gen-java")
        self.set_output_dir("java_out")
        self.set_target_dir("java_target")
        self.update_command()

    def run(self):
        # please add test code here
        return False

    def run_choose(self, choose):
        if choose:
            return self.run_success()
        return self.run_fail()

    def run_success(self):
        if not self._not_use_default_add_idl_file:
            self.add_idl_files()
        status, ret = exec_command(self.command)
        if status != 0:
            print_failure(f"[ERROR] command:{self.command} run err")
            print_failure(f"[ERROR] {ret}")
            return False
        special_proc_func = None
        if self._dump_ast_check:
            special_proc_func = dump_ast_compare
        if compare_target_files(self.output_dir, self.target_dir, special_proc_func):
            return True
        return False

    def run_fail(self):
        if not self._not_use_default_add_idl_file:
            self.add_idl_files()
        status, result = exec_command(self.command)

        if status == 0:
            print_failure(f"[ERROR] ret({status}) run cmd: {self.command}")
            return False

        expected_fail_output = ""
        with open(os.path.join(self.target_dir, "fail_output.txt"), 'r') as target_output:
            expected_fail_output = target_output.read()

        if self._check_fail_ignore_line:
            ret = hdi_gen_fail_check_ignore_line(result, expected_fail_output)
            if not ret:
                print_failure(f"[ERROR] Expect ret: {expected_fail_output}")
                print_failure(f"[ERROR] Actual ret: {result}")
            return ret

        if expected_fail_output != result:
            print_failure(f"[ERROR] Expect ret: {expected_fail_output}")
            print_failure(f"[ERROR] Actual ret: {result}")
            return False
        return True

    def remove_output(self):
        exec_command("rm -rf {}".format(self.output_dir))
        return True

    def add_idl_files(self):
        idl_list = get_all_idl_files(self.idl_dir)
        for idl in idl_list:
            self.command = "".join((self.command, " -c {}".format(idl)))

    def test(self):
        print_success("[ RUN       ] {}".format(self.name))
        start_time = get_time_stamp()
        result = self.run()
        end_time = get_time_stamp()

        if result:
            print_success("[        OK ] {} ({}ms)".format(self.name, end_time - start_time))
        else:
            print_failure("[    FAILED ] {} ({}ms)".format(self.name, end_time - start_time))
        return result
