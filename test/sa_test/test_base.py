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
import shutil
import re

from util import exec_command, compare_target_files, file_exists
from util import get_time_stamp, print_success, print_failure
from util import get_idl


class Test:
    def __init__(self):
        self.name = self.__class__.__name__
        self._file_name = self.get_file_name()
        self.working_dir = self.get_working_dir()
        self.idl_file = os.path.join(self.working_dir, "foo", "IFoo.idl")
        self.output_dir = os.path.join(self.working_dir, "out")
        self.target_dir = os.path.join(self.working_dir, "target")
        self._gen_langauge = "--gen-cpp"
        self._idl = get_idl()
        self._command_format = "{} {} --intf-type sa -c {} -d {}"
        self.command = self._command_format.format(self._idl, self._gen_langauge, self.idl_file, self.output_dir)

    def get_file_name(self):
        # 子类继承实现
        return ""

    def get_working_dir(self):
        current_path = os.path.dirname(os.path.abspath(__file__))
        return os.path.join(current_path, os.path.splitext(os.path.basename(self._file_name))[0])

    def set_command_attr(self, attr):
        self._command_attr = attr

    def set_command_gen_langauge(self, langauge):
        self._gen_langauge = langauge

    def set_output_dir(self, output_dir):
        self.output_dir = os.path.join(self.working_dir, output_dir)

    def set_target_dir(self, target_dir):
        self.target_dir = os.path.join(self.working_dir, target_dir)

    def set_idl_file(self, idl_file):
        self.idl_file = os.path.join(self.working_dir, "foo", idl_file)

    def update_command(self):
        self.command = self._command_format.format(self._idl, self._gen_langauge, self.idl_file, self.output_dir)

    def set_gen_c_env(self):
        self.set_command_gen_langauge("--gen-c")
        self.set_output_dir("out_c")
        self.set_target_dir("target_c")
        self.update_command()

    def set_gen_cpp_env(self):
        self.set_command_gen_langauge("--gen-cpp")
        self.set_output_dir("out_cpp")
        self.set_target_dir("target_cpp")
        self.update_command()

    def set_gen_rust_env(self):
        self.set_command_gen_langauge("--gen-rust")
        self.set_output_dir("out_rust")
        self.set_target_dir("target_rust")
        self.update_command()

    def set_gen_ts_env(self):
        self.set_command_gen_langauge("--gen-ts")
        self.set_output_dir("out_ts")
        self.set_target_dir("target_ts")
        self.set_idl_file("IFooTs.idl")
        self.update_command()

    def set_cmd_test_env(self):
        self._command_format = "{} --intf-type sa -c {} {}"
        self.command = self._command_format.format(self._idl, self.idl_file, self._command_attr)

    def run(self):
        # please add test code here
        return False

    def run_choose(self, choose, no_output=False):
        if choose:
            result = self.run_success(no_output)
        else:
            result = self.run_fail()
        self.remove_output()
        return result

    def deal_result(self, result):
        # 子类继承实现
        return result

    def run_success(self, no_output=False):
        status, result = exec_command(self.command)
        if status != 0:
            print_failure(f"[ERROR] command:{self.command} run err")
            return False
        if no_output is True:
            return True
        if file_exists(os.path.join(self.target_dir, "output.txt")):
            with open(os.path.join(self.target_dir, "output.txt"), 'r') as target_output:
                expected_output = target_output.read()
            if self.deal_result(result) == expected_output:
                return True
            else:
                print_failure(f"[ERROR] command:{self.command} not meet expectations")
                return False
        if 'type_import' in self.working_dir:
            self.set_output_dir("out_file")
            self.set_target_dir("target_file")
        return compare_target_files(self.output_dir, self.target_dir)

    def sa_gen_fail_check_ignore_line(self, result: str, target: str):
        fail_template = r"(.*): \[(\S+):\d+\] \[?(.*)error:(.*)"
        result_lines = result.split("\n")
        target_lines = target.split("\n")
        if len(result_lines) != len(target_lines):
            print_failure(f"[ERROR] result line(len(result_lines)) != target line(len(target_lines))")
            return False

        for result_line, target_line in zip(result_lines, target_lines):
            lhd_obj = re.search(fail_template, result_line)
            rhd_obj = re.search(fail_template, target_line)
            if not lhd_obj and not rhd_obj:
                if result_line == target_line:
                    continue
                else:
                    print_failure(f"[ERROR] actual: {result_line}")
                    print_failure(f"[ERROR] expect: {target_line}")
                    return False
            elif not lhd_obj or not rhd_obj:
                print_failure(f"[ERROR] actual: {result_line}")
                print_failure(f"[ERROR] expect: {target_line}")
                return False
            lhd_start_check_content = lhd_obj.group(1)
            rhd_start_check_content = rhd_obj.group(1)
            lhd_err_func_check_content = lhd_obj.group(2)
            rhd_err_func_check_content = rhd_obj.group(2)
            lhd_median_check_content = lhd_obj.group(3)
            rhd_median_check_content = rhd_obj.group(3)
            lhd_end_check_content = lhd_obj.group(4)
            rhd_end_check_content = rhd_obj.group(4)
            if lhd_start_check_content != rhd_start_check_content or \
                lhd_err_func_check_content != rhd_err_func_check_content or \
                lhd_median_check_content != rhd_median_check_content or \
                lhd_end_check_content != rhd_end_check_content:
                print_failure(f"[ERROR] actual: {result_line}")
                print_failure(f"[ERROR] expect: {target_line}")
                return False

        return True

    def run_fail(self):
        status, result = exec_command(self.command)
        with open(os.path.join(self.target_dir, "fail_output.txt"), 'r') as target_output:
            expected_fail_output = target_output.read()

        if status != 0 and self.sa_gen_fail_check_ignore_line(result, expected_fail_output):
            return True
        return False

    def remove_output(self):
        if os.path.exists(self.output_dir):
            shutil.rmtree(self.output_dir)

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
