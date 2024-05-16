
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
from base import Test
import os

class hash_bad_04(Test):
    def gen_fail_output(self):
        work_dir = self.get_working_dir()
        file_path = os.path.join(work_dir, "foo", "v1_0", "IFoo.idl")
        if not os.path.exists(self.target_dir):
            os.makedirs(self.target_dir)
        content = f"[IDL-GEN]: IFoo.idl:16:9:package name 'ohos.hdi.foo.v1_0' does not match file path '{file_path}'"
        fail_output = os.path.join(self.target_dir, "fail_output.txt")
        with os.fdopen(os.open(fail_output, os.O_CREAT | os.O_WRONLY, 0o644), mode='w', encoding='utf-8') as fd:
            fd.write(content)

    def set_idl_attr(self):
        work_dir = self.get_working_dir()
        package_path = os.path.join(work_dir, "foo")
        idl_dir = os.path.join(package_path, "v1_0")
        self.set_hash_param(package_path, idl_dir)

    def run(self):
        self.set_idl_attr()
        self.gen_fail_output()
        return self.run_choose(False)

    def get_file_name(self):
        return __file__


if __name__ == "__main__":
    hash_bad_04().test()

