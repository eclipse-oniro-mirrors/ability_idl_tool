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

__all__ = ["test_base", "util"]

from .test_base import Test
from .util import get_time_stamp, print_success, print_failure, compare_target_files, exec_command, file_exists
from .util import make_binary_file, clean_binary_file, get_all_idl_files, get_idl, get_subclasses, dump_ast_compare
from .util import hdi_gen_fail_check_ignore_line
