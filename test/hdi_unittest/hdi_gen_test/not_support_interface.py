
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


class not_support_interface(Test):
    def get_file_name(self):
        return __file__

    def set_idl_attr(self):
        self.set_check_fail_ignore_line(True)
        work_dir = self.get_working_dir()
        package_path = os.path.join(work_dir)
        self.set_package_path(package_path)
        idl_dir = package_path
        self.set_idl_dir(idl_dir)
        self.set_system_attr("full")
        self.set_mode_attr("ipc")
        self.set_gen_cpp_env()
        self.update_command()

    def run(self):
        self.set_idl_attr()
        return self.run_choose(False)


if __name__ == "__main__":
    not_support_interface().test()

