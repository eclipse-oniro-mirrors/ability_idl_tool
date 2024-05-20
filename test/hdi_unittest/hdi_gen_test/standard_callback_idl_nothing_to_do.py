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


class empty_file(Test):
    def get_file_name(self):
        return __file__

    def set_idl_attr(self):
        idl_dir = os.path.join(self.get_working_dir(), "foo", "v1_0")
        self.set_not_use_default_add_idl_file(True)
        self.set_target_dir("cpp_target")
        self.set_command(f"{self._idl} --intf-type hdi -D {idl_dir}")

    def run_cpp(self):
        self.set_idl_attr()
        return self.run_choose(False)

    def run(self):
        return self.run_cpp()


if __name__ == "__main__":
    empty_file().test()

