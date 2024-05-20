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

from base import file_exists, make_binary_file, print_failure, print_success, get_subclasses, get_idl
from base import Test
from hdi_dump_ast_test import __all__
import importlib

for module_name in __all__:
    module = importlib.import_module(f'hdi_dump_ast_test.{module_name}')


class Tests:
    test_cases = get_subclasses(Test)
    test_objs = list()

    @staticmethod
    def set_up_test_case():
        hdi_gen_file = get_idl()
        ret = file_exists(hdi_gen_file)
        if not ret:
            hdi_gen_path = "../../"
            if make_binary_file(hdi_gen_path)[0] == 0:
                ret = True
        if not ret:
            print_failure("[===========] failed to make idl-gen")
        return ret

    @staticmethod
    def tear_down_test_case():
        for case in Tests.test_objs:
            case.remove_output()

    @staticmethod
    def test():
        test_case_num = len(Tests.test_cases)
        success_case_num = 0
        print_success("[===========] start {} test".format(test_case_num))
        for test_case in Tests.test_cases:
            obj = test_case()
            Tests.test_objs.append(obj)
            if obj.test():
                success_case_num += 1
        print_success("[    PASSED ] {} test".format(success_case_num))
        failure_case_num = test_case_num - success_case_num
        if failure_case_num > 0:
            print_failure("[    FAILED ] {} test".format(failure_case_num))


if __name__ == "__main__":
    if not Tests.set_up_test_case():
        print_failure("test case set up failed!")
        exit(-1)
    Tests.test()
    Tests.tear_down_test_case()
