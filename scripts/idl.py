#!/usr/bin/env python3
# coding: utf-8
#
# Copyright (c) 2023 Huawei Device Co., Ltd.
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

#!/usr/bin/env python3
# coding: utf-8
#
# Copyright (c) 2023 Huawei Device Co., Ltd.
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

import os
import subprocess
import platform
import argparse

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--src-idl', help='idl source file')
    parser.add_argument('--dst-path', help='the converted target path')
    parser.add_argument('--idl-tool-path', help='path of the idl conversion tool')
    parser.add_argument('--log-domainid', help='hilog domain id')
    parser.add_argument('--log-tag', help='hilog tag')
    parser.add_argument('--hitrace', help='hitrace switch, default off')
    arguments = parser.parse_args()
    return arguments

def run_command(cmd, execution_path, input_arguments):
    print(" ".join(cmd) + " | execution_path: " + execution_path)
    proc = subprocess.Popen(cmd, cwd=execution_path, stdout=subprocess.PIPE)
    proc.wait()

def idl_gen_interface(input_arguments):
    (path, name) = os.path.split(input_arguments.idl_tool_path)
    is_exists = os.path.exists(input_arguments.dst_path)
    if is_exists:
        for filename in os.listdir(input_arguments.dst_path):
            file_path = os.path.join(input_arguments.dst_path, filename)
            if os.path.isfile(file_path):
                os.remove(file_path)
    is_exists = os.path.exists(input_arguments.dst_path)

    if not is_exists:
        try:
            os.makedirs(input_arguments.dst_path, 0o750, exist_ok=True)
        except (OSError, TypeError) as excep:
            raise excep
        finally:
            pass

    cmd = [os.path.join("./", name, "idl"),
           "-gen-cpp", "-d", input_arguments.dst_path, "-c",
           input_arguments.src_idl]
    if input_arguments.log_domainid:
        cmd += ['-log-domainid', input_arguments.log_domainid]
    if input_arguments.log_tag:
        cmd += ['-log-tag', input_arguments.log_tag]
    if input_arguments.hitrace:
        cmd += ['-t', input_arguments.hitrace]
    run_command(cmd, path, input_arguments)

if __name__ == '__main__':
    idl_gen_interface(parse_args())