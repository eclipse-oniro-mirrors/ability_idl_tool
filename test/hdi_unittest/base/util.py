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
import subprocess
import time
import re

__all__ = ["get_time_stamp", "print_success", "print_failure", "compare_target_files", "exec_command", "file_exists",
           "make_binary_file", "clean_binary_file", "get_all_idl_files", "get_idl", "get_subclasses",
           "dump_ast_compare", "hdi_gen_fail_check_ignore_line"]

def get_time_stamp():
    return int(round(time.time() * 1000))


def print_success(info):
    print("\033[32m{}\033[0m".format(info))


def print_failure(info):
    print("\033[31m{}\033[0m".format(info))


def is_subsequence(first_file, second_file):
    first_info = first_file.read()
    second_info = second_file.readline()
    while second_info:
        if first_info.find(second_info) == -1:
            print("line\n", second_info, "is not in output file")
            return False
        second_info = second_file.readline()
    return True


def dump_ast_compare(line_lhd:str, line_rhd:str):
    if line_lhd == line_rhd:
        return True
    if not line_lhd.startswith("AST") or not line_rhd.startswith("AST"):
        return False
    ast_templage = r"AST\[name:\s(.*)\sfile:\s(.*)\]"
    obj_lhd = re.search(ast_templage, line_lhd)
    obj_rhd = re.search(ast_templage, line_rhd)
    if not obj_lhd or not obj_rhd:
        return False
    ast_name_lhd = obj_lhd.group(1)
    ast_name_rhd = obj_rhd.group(1)
    if ast_name_lhd.strip() != ast_name_rhd.strip():
        return False
    ast_path_lhd = obj_lhd.group(2)
    ast_path_rhd = obj_rhd.group(2)
    lhd_index = ast_path_lhd.find("foo")
    rhd_index = ast_path_rhd.find("foo")
    if ast_path_lhd[lhd_index:].strip() != ast_path_rhd[rhd_index:].strip():
        return False
    return True


def content_compare(file1, file2, lhd, rhd, special_proc_func=None):
    if len(lhd) != len(rhd):
        print_failure(f"{file1} line num({len(lhd)}) != {file2} line num({len(rhd)}))")
        return False
    line_num = 0
    for line1, line2 in zip(lhd, rhd):
        line_num += 1
        if ((special_proc_func is None and line1 != line2) or
            (special_proc_func is not None and not special_proc_func(line1, line2))):
            print_failure(f"{file1}:{line_num}:{line1}")
            print_failure(f"{file2}:{line_num}:{line2}")
            return False
    return True


def compare_file(first_file_path, second_file_path, special_proc_func=None):
    with open(first_file_path, 'r') as first_file:
        with open(second_file_path, 'r') as second_file:
            return content_compare(first_file_path,
                                   second_file_path,
                                   first_file.readlines(),
                                   second_file.readlines(),
                                   special_proc_func)


def check_c_cpp_file(file_list):
    for filename in file_list:
        if filename.endswith(".h") or filename.endswith(".c") or filename.endswith(".cpp"):
            return True
    return False


def compare_target_files(first_file_path, second_file_path, special_proc_func=None):
    first_files_list = get_all_files(first_file_path)
    second_files_list = get_all_files(second_file_path)

    first_files = set([file[len(first_file_path):] for file in first_files_list])
    if_c_cpp = check_c_cpp_file(first_files)
    if if_c_cpp:
        # 去除末尾.txt符号
        second_files = set([file[len(second_file_path):-4] for file in second_files_list])
    else:
        second_files = set([file[len(second_file_path):] for file in second_files_list])

    if len(first_files) != len(second_files):
        print_failure(f"[ERROR] file num not compile({len(first_files)}, {len(second_files)})")
        return False

    common_files = first_files & second_files

    if len(common_files) == 0:
        print_failure("[ERROR] not compile target")
        return False

    if if_c_cpp:
        check_file_name_format = "{}{}.txt"
    else:
        check_file_name_format = "{}{}"

    for files in common_files:
        if not compare_file("{}{}".format(first_file_path, files),
                            check_file_name_format.format(second_file_path, files), special_proc_func):
            print("file ", "{}{}".format(first_file_path, files), "{}{}".format(second_file_path, files), \
                "is different")
            return False
    return True


def exec_command(command):
    return subprocess.getstatusoutput(command)


def file_exists(file_path):
    return os.path.isfile(file_path)


def make_binary_file(file_path):
    print("making idl-gen...")
    return exec_command("make --directory={} --jobs=4".format(file_path))


def clean_binary_file(file_path):
    return exec_command("make --directory={} clean".format(file_path))


def get_all_files(path):
    file_list = []
    items = os.listdir(path)
    for item in items:
        item_path = os.path.join(path, item)
        if not os.path.isdir(item_path):
            file_list.append(item_path)
        else:
            file_list += get_all_files(item_path)
    return file_list


def get_all_idl_files(idl_path):
    file_list = get_all_files(idl_path)
    idl_file_list = []
    for file in file_list:
        if os.path.splitext(file)[-1] == ".idl":
            idl_file_list.append(file)
    return idl_file_list


def get_idl():
    current_path = os.path.dirname(os.path.abspath(__file__))
    relative_path = os.path.join(current_path, "..", "..", "..", "idl_tool_2", "idl-gen")
    return os.path.realpath(relative_path)


def get_subclasses(cls):
    return cls.__subclasses__()


def hdi_gen_fail_check_ignore_line(result: str, target: str):
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
