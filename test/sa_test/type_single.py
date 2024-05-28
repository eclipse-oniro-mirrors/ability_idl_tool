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


class TypeSingle(Test):
    def get_file_name(self):
        return __file__

    def run_cpp(self):
        self.set_gen_cpp_env()
        return self.run_choose(True)

    def run_rust(self):
        self.set_gen_rust_env()
        return self.run_choose(True)

    def run_ts(self):
        self.set_gen_ts_env()
        return self.run_choose(True)

    def run(self):
        return self.run_cpp() and self.run_rust() and self.run_ts()


if __name__ == "__main__":
    TypeSingle().test()