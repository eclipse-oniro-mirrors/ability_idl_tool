#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# Copyright (c) 2025 Huawei Device Co., Ltd.
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
from test_base import Test
from pathlib import Path


class SupportOptionUsingHooks(Test):
    def __init__(self):
        super().__init__()
        self.idl_file = os.path.join(self.working_dir, "idl_file", "foo", "IFoo.idl")

    def get_file_name(self):
        return __file__

    def set_gen_cpp_import_env(self):
        # 使用 pathlib 处理路径
        base_output = Path("out_file") / "out_cpp"
        base_target = Path("target_file") / "target_cpp"

        self.set_command_gen_langauge("--gen-cpp")
        self.set_output_dir(str(base_output))
        self.set_target_dir(str(base_target))
        self.update_command()

    def run_cpp(self):
        self.set_gen_cpp_import_env()
        return self.run_choose(True)

    def run(self):
        return self.run_cpp()


if __name__ == "__main__":
    SupportOptionUsingHooks().test()
