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


def get_time_stamp():
    return int(round(time.time() * 1000))


def print_success(info):
    print("\033[32m{}\033[0m".format(info))


def print_failure(info):
    print("\033[31m{}\033[0m".format(info))

def print_info(info):
    print("\033[33m{}\033[0m".format(info))

def is_subsequence(first_file, second_file):
    first_info = first_file.read()
    second_info = second_file.readline()
    while second_info:
        if first_info.find(second_info) == -1:
            print("line\n", second_info, "is not in output file")
            return False
        second_info = second_file.readline()
    return True

def content_compare(file1, file2, lhd, rhd):
    line_num = 0
    for line1, line2 in zip(lhd, rhd):
        line_num += 1
        if line1 != line2:
            print_failure(f"{file1}:{line_num}:{line1}")
            print_failure(f"{file2}:{line_num}:{line2}")
            return False
    return True


def compare_file(first_file_path, second_file_path):
    with open(first_file_path, 'r') as first_file:
        with open(second_file_path, 'r') as second_file:
            return content_compare(first_file_path, second_file_path, first_file.readlines(), second_file.readlines())


def compare_target_files(first_file_path, second_file_path):
    first_files_list = get_all_files(first_file_path)
    second_files_list = get_all_files(second_file_path)

    first_files = set([file[len(first_file_path):] for file in first_files_list])
    second_files = set([file[len(second_file_path):-4] for file in second_files_list])

    common_files = first_files & second_files

    for files in common_files:
        if not compare_file("{}{}".format(first_file_path, files), "{}{}.txt".format(second_file_path, files)):
            print("file ", "{}{}".format(first_file_path, files), "{}{}.txt".format(second_file_path, files), \
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
    relative_path = os.path.join(current_path, get_idl_name())
    return os.path.realpath(relative_path)

def get_idl_name():
    idl_name = 'idl-gen'
    if os.name == 'nt':
        idl_name = 'idl-gen.exe'
    elif os.name == 'posix':
        idl_name = 'idl-gen'
    return idl_name

def get_subclasses(cls):
    return cls.__subclasses__()

