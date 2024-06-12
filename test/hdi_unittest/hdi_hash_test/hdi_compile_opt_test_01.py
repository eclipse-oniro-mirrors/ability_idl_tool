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
from base import Test


class hdi_compile_opt_test_01(Test):
    def get_file_name(self):
        return __file__

    def set_idl_attr(self):
        work_dir = self.get_working_dir()
        package_path = os.path.join(work_dir, "foo")
        idl_dir = os.path.join(package_path, "v1_0")
        self.set_not_use_default_add_idl_file(True)
        output_path = os.path.join(self.working_dir, "hash_out")
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        self.set_output_path(os.path.join(output_path, "hash.txt"))
        self.set_output_dir(output_path)
        self.set_target_dir(os.path.join(self.working_dir, "hash_target"))
        command = f"{self._idl} --intf-type hdi --hash -c "\
            f"-r ohos.hdi:{self.get_working_dir()} -o {self._output_path} {idl_dir}/IFoo.idl"
        self.set_command(command)

    def run(self):
        self.set_idl_attr()
        return self.run_choose(True)


if __name__ == "__main__":
    hdi_compile_opt_test_01().test()

