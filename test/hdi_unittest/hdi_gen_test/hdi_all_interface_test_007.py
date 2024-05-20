
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


class hdi_all_interface_test_007(Test):
    def get_file_name(self):
        return __file__

    def set_idl_attr(self):
        self.set_mode_attr("ipc")
        self.set_system_attr("full")
        self.set_command_ext_attr("-m ipc --system full")
        self.set_gen_c_env()
        self.update_command()

    def run_c(self):
        self.set_gen_c_env()
        self.update_command()
        ret = self.run_choose(True)
        self.remove_output()
        return ret

    def run_cpp(self):
        self.set_gen_cpp_env()
        self.update_command()
        ret = self.run_choose(True)
        self.remove_output()
        return ret

    def run(self):
        self.set_idl_attr()
        ret = self.run_c()
        ret &= self.run_cpp()
        return ret


if __name__ == "__main__":
    hdi_all_interface_test_007().test()

