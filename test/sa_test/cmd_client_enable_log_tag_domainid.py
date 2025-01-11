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

from test_base import Test


class CmdClientEnableLogTagDomainId(Test):
    def get_file_name(self):
        return __file__

    def run_cmd(self):
        self.set_command_attr(f"-client-enable -log-tag new_tag -log-domainid new_id -d {self.output_dir} --gen-cpp")
        self.set_cmd_test_env()
        return self.run_choose(True)

    def run(self):
        return self.run_cmd()


if __name__ == "__main__":
    CmdClientEnableLogTagDomainId().test()
