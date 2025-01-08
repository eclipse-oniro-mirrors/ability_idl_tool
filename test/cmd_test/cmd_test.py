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
from enum import Enum
import inspect


class RetStatus(Enum):
    OK = 0
    ERR = 255


def get_idl():
    current_path = os.path.dirname(os.path.abspath(__file__))
    relative_path = os.path.join(current_path, "..", "..", "idl_tool_2", "idl-gen")
    return os.path.realpath(relative_path)


def get_caller_info():
    frame = inspect.currentframe().f_back.f_back
    caller_function = frame.f_code.co_name
    caller_lineno = frame.f_lineno
    caller_class = None
    if 'self' in frame.f_locals:
        caller_class = frame.f_locals['self'].__class__.__name__
    return caller_class, caller_function, caller_lineno


def get_time_stamp():
    return int(round(time.time() * 1000))


def print_success(info):
    print("\033[32m{}\033[0m".format(info))


def print_failure(info):
    print("\033[31m{}\033[0m".format(info))


def compare_file(first_file, second_file):
    with open(first_file, 'r') as first_hash_file:
        with open(second_file, 'r') as second_hash_file:
            first_hash_info = first_hash_file.read()
            second_hash_info = second_hash_file.read()
            return first_hash_info == second_hash_info

def file_exists(file_path):
    return os.path.isfile(file_path)

def exec_command_check(command, expect_result, exp_status):
    status, exec_result = subprocess.getstatusoutput(command)
    caller_class, caller_function, caller_lineno = get_caller_info()
    if status != exp_status.value:
        print_failure(f"[{caller_class}:{caller_function}:{caller_lineno}][{command}][{status}:{exp_status}]"
                      f"{exec_result}")
        return False

    if exec_result != expect_result:
        print_success("=====expect=====\n"
                      f"[{caller_class}:{caller_function}:{caller_lineno}][{command}]{expect_result}")
        print_failure("=====actual=====\n"
                      f"[{caller_class}:{caller_function}:{caller_lineno}][{command}]{exec_result}")
        return False

    return True

class Test:
    def __init__(self):
        self.name = self.__class__.__name__
        self.cmd = get_idl()

    def run(self):
        # please add test code here
        return False

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

# print help info
class TestCmdPrintHelp(Test):
    def run(self):
        expect_result = "Compile a .idl file and generate C/C++/Ts/Rust and Java codes.\n"
        expect_result += "Usage: idl [options] file\n"
        expect_result += "Options:\n"
        expect_result += "  -h, --help                      Display command line options\n"
        expect_result += "  -v, --version                   Display toolchain version information\n"
        expect_result += \
            "      --system <value>            "\
            "Set system level 'mini','lite' or 'full', the default value is 'full', only support 'intf-type hdi'\n"
        expect_result += \
            "  -m, --mode <value>              "\
            "Set generate code mode 'low', 'passthrough', 'ipc' or 'kernel', the default value is 'ipc', "\
            "only support 'intf-type hdi'\n"
        expect_result += "  -p, --package <package name>    Set package of idl files, only support 'intf-type hdi'\n"
        expect_result += "      --dump-ast                  Display the AST of the compiled file\n"
        expect_result += \
            "      --dump-metadata             "\
            "Display the metadata generated from the compiled file, only support 'intf-type sa'\n"
        expect_result += \
            "      --hash                      Generate hash info of idl files, only support 'intf-type hdi'\n"
        expect_result += \
            "  -r <rootPackage>:<rootPath>     Set root path of root package, only support 'intf-type hdi'\n"
        expect_result += "  -c                              Compile the .idl file\n"
        expect_result += "  -D <directory>                  Directory of the idl file, only support 'intf-type hdi'\n"
        expect_result += "  -d <directory>                  Place generated codes into <directory>\n"
        expect_result += \
            "  -o <file>                       Place the output into <file>, only support 'intf-type hdi'\n"
        expect_result += \
            "  -s <file>                       Place the metadata into <file>, only support 'intf-type sa'\n"
        expect_result += "      --gen-c                     Generate C codes, only support 'intf-type hdi'\n"
        expect_result += "      --gen-cpp                   Generate C++ codes\n"
        expect_result += "      --gen-java                  Generate Java codes, only support 'intf-type hdi'\n"
        expect_result += "      --gen-rust                  Generate Rust codes, only support 'intf-type sa'\n"
        expect_result += "      --gen-ts                    Generate Ts codes, only support 'intf-type sa'\n"
        expect_result += \
            "      --log-domainid <domainid>   "\
            "Place the service domain in <domainid>, Enable log(Pair with -log-tag), only support 'intf-type sa'\n"
        expect_result += \
            "      --log-tag <tag>             "\
            "Place the subsystem name in <tag>, Enable log(Pair with -log-domainid), only support 'intf-type sa'\n"
        expect_result += \
            "  -t <hitrace tag>                "\
            "Place the constant name from hitrace_meter.h file in <hitrace tag>, only support 'intf-type sa'\n"
        expect_result += \
            "      --intf-type <tag>           Set type of generated codes 'sa' or 'hdi', default type is 'sa'"
        command = f"{self.cmd} -h"
        ret = exec_command_check(command, expect_result, RetStatus.OK)
        command = f"{self.cmd} --help"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)
        return ret


# print version info
class TestCmdPrintVersion(Test):
    def run(self):
        expect_result = "idl 1.0\n"
        expect_result += "Copyright (c) Hua"\
            "wei Technologies Co., Ltd. 2024. All rights reserved.\n"
        command = f"{self.cmd} -v"
        ret = exec_command_check(command, expect_result, RetStatus.OK)
        command = f"{self.cmd} --version"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)
        return ret


# exec not support cmd
class TestCmdNotSupport(Test):
    def run_sa(self):
        expect_result = """[IDL-GEN]: The Option "-abc" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} -abc"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        # sa not support cmd
        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'system'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --system full"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'm' or 'mode'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --mode ipc"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        command = f"{self.cmd} -m ipc"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'p' or 'package'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} -p abc.test"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        command = f"{self.cmd} --package abc.test"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'hash'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --hash"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'r'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} -r abc:./"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'o'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} -o test.file"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = \
            "[IDL-GEN]: Option 'intf-type sa' only support language option 'gen-cpp', 'gen-rust' or 'gen-ts'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --gen-c"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        return ret

    def run_hdi(self):
        # hdi not support cmd
        expect_result = "[IDL-GEN]: Option 'intf-type hdi' not support option 'dump-metadata'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --dump-metadata"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-s" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -s"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = \
            "[IDL-GEN]: Option 'intf-type hdi' only support language option 'gen-c', 'gen-cpp' or 'gen-java'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --gen-ts"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        command = f"{self.cmd} --intf-type hdi --gen-rust"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        return ret

    def run(self):
        ret = self.run_sa()
        ret &= self.run_hdi()
        return ret


# set not support value
class TestCmdNotSupportValue(Test):
    def run(self):
        expect_result = "[IDL-GEN]: The Option \"-abc\" is illegal.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} -abc"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)
        return ret

class TestCmdModeTypeTest(Test):
    def run(self):
        expect_result = ""
        command = f"{self.cmd} --intf-type hdi --mode low"
        ret = exec_command_check(command, expect_result, RetStatus.OK)

        command = f"{self.cmd} --intf-type hdi -m passthrough"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        command = f"{self.cmd} --intf-type hdi --mode ipc"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        command = f"{self.cmd} --intf-type hdi -m kernel"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        expect_result = "[IDL-GEN]: invalid generate mode set: 'abc', please input low/passthrough/ipc/kernel/sa."
        command = f"{self.cmd} --intf-type hdi --mode abc"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)
        return ret


class TestCmdPackageTypeTest(Test):
    def run(self):
        expect_result = ""
        command = f"{self.cmd} --intf-type hdi -p abc"
        ret = exec_command_check(command, expect_result, RetStatus.OK)

        return ret


class TestCmdDumpAstTypeTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: no idl files."""
        command = f"{self.cmd} --intf-type hdi --dump-ast"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --dump-ast"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        command = f"{self.cmd} --dump-ast"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdDumpMetaDataTest(Test):
    def run(self):
        expect_result = "[IDL-GEN]: Option 'intf-type hdi' not support option 'dump-metadata'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --dump-metadata"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --dump-metadata"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdHashTest(Test):
    def run(self):
        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi --hash"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'hash'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --hash"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdRootPackageTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-r" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -r"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: invalid path 'xx'."
        command = f"{self.cmd} --intf-type sa -r x:xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' not support option 'r'.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -r x:./"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-r" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -r"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: invalid path 'xx'."
        command = f"{self.cmd} --intf-type hdi -r x:xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = ""
        command = f"{self.cmd} --intf-type hdi -r x:./"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        return ret


class TestCmdDirectoryTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-D" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -D"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'D'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -D ./"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: failed to open './x/', errno:2\n"
        expect_result += """[IDL-GEN]: Option 'intf-type sa' not support option 'D'.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -D ./x"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-D" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -D"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = ""
        command = f"{self.cmd} --intf-type hdi -D {os.path.dirname(os.path.abspath(__file__))}"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        expect_result = f"[IDL-GEN]: failed to open '{os.path.dirname(os.path.abspath(__file__))}/x/', errno:2"
        command = f"{self.cmd} --intf-type hdi -D {os.path.dirname(os.path.abspath(__file__))}/x"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        return ret


class TestCmdPlaceGenerateDirectoryTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-d" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -d"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa -d ./"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa -d x"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-d" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -d"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi -d ./"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi -d x"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdPlaceOutputFileTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-o" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -o"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'o'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -o xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-o" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -o"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = ""
        command = f"{self.cmd} --intf-type hdi -o xx"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        return ret


class TestCmdPlaceMetadataInfoTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-s" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -s"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type hdi' not support option '-s'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -s aaa"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenCTest(Test):
    def run(self):
        expect_result = \
            "[IDL-GEN]: Option 'intf-type sa' only support language option 'gen-cpp', 'gen-rust' or 'gen-ts'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa --gen-c"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi --gen-c"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenCppTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --gen-cpp"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi --gen-cpp"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenJavaTest(Test):
    def run(self):
        expect_result = \
            "[IDL-GEN]: Option 'intf-type sa' only support language option 'gen-cpp', 'gen-rust' or 'gen-ts'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa --gen-java"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: no idl files."
        command = f"{self.cmd} --intf-type hdi --gen-java"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret

class TestCmdGenRustTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --gen-rust"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = \
            """[IDL-GEN]: Option 'intf-type hdi' only support language option 'gen-c', 'gen-cpp' or 'gen-java'.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --gen-rust"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenTsTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --gen-ts"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = \
            "[IDL-GEN]: Option 'intf-type hdi' only support language option 'gen-c', 'gen-cpp' or 'gen-java'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --gen-ts"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenLogDomainIdTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --log-domainid xx"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type hdi' not support option '-t', '-log-domainid' or '-log-tag'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --log-domainid xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdGenLogDomainTagTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "--log-tag" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa --log-tag"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --log-tag xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa --log-domainid xx --log-tag xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "---log-tag" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi ---log-tag xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret


class TestCmdHiTraceTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "-t" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa -t"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa -t xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "-t" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -t"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type hdi' not support option '-t', '-log-domainid' or '-log-tag'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi -t xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret

class TestCmdSystemTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: The Option "--system" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa --system"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: invalid system level set: 'xx', please input mini/lite/full"
        command = f"{self.cmd} --intf-type sa --system xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: Option 'intf-type sa' not support option 'system'.\n"
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type sa --system mini"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "--system" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type hdi --system"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = "[IDL-GEN]: invalid system level set: 'xx', please input mini/lite/full"
        command = f"{self.cmd} --intf-type hdi --system xx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = ""
        command = f"{self.cmd} --intf-type hdi --system mini"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        command = f"{self.cmd} --intf-type hdi --system lite"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        command = f"{self.cmd} --intf-type hdi --system full"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)
        return ret

class TestCmdIntfTypeTest(Test):
    def run(self):
        expect_result = """[IDL-GEN]: Option 'intf-type sa' must set idl file."""
        command = f"{self.cmd} --intf-type sa"
        ret = exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = ""
        command = f"{self.cmd} --intf-type hdi"
        ret &= exec_command_check(command, expect_result, RetStatus.OK)

        expect_result = "[IDL-GEN]: invalid interface type set: 'xxx', please input hdi/sa."
        command = f"{self.cmd} --intf-type xxx"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        expect_result = """[IDL-GEN]: The Option "--intf-type" is illegal.\n"""
        expect_result += """Use "-h, --help" to show usage."""
        command = f"{self.cmd} --intf-type"
        ret &= exec_command_check(command, expect_result, RetStatus.ERR)

        return ret

def get_subclasses(cls):
    return cls.__subclasses__()


class Tests:
    test_cases = get_subclasses(Test)
    pass

    @staticmethod
    def set_up_test_case():
        hdi_gen_file = get_idl()
        ret = file_exists(hdi_gen_file)
        if not ret:
            print_failure("[===========] no such file:{}".format(hdi_gen_file))
        return ret

    @staticmethod
    def set_down_test_case():
        pass

    @staticmethod
    def test():
        test_case_num = len(Tests.test_cases)
        success_case_num = 0
        print_success("[===========] start {} test".format(test_case_num))
        for test_case in Tests.test_cases:
            obj = test_case()
            if obj.test():
                success_case_num += 1
        print_success("[    PASSED ] {} test".format(success_case_num))
        failure_case_num = test_case_num - success_case_num
        if failure_case_num > 0:
            print_failure("[    FAILED ] {} test".format(failure_case_num))


if __name__ == "__main__":
    if not Tests.set_up_test_case():
        exit(-1)
    Tests.test()
    Tests.set_down_test_case()
