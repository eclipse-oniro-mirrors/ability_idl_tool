# Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
ifeq ($(findstring ;, $(PATH)), ;)
	export PWD:=$(shell cd)
	export TARGET_DIR := $(PWD)
	export BUILD_DIR:=$(TARGET_DIR)\build
	export TARGET:=$(TARGET_DIR)\idl-gen.exe
else
	export PWD := $(abspath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
	export TARGET_DIR := $(PWD)
	export BUILD_DIR = $(TARGET_DIR)/build
	export TARGET:=$(TARGET_DIR)/idl-gen
endif

export CXXFLAGS = -std=c++14 -O2 -Wall -fno-common -fno-strict-aliasing -s
export Q := @
export MAKEFLAGS += --no-print-directory

MAKE_FILE_NAME = MakefileLinux
ifeq ($(findstring ;, $(PATH)), ;)
	MAKE_FILE_NAME := MakefileWin
endif

all:
	$(Q) echo start build $(TARGET) ...
	$(Q) $(MAKE) -f $(MAKE_FILE_NAME)

clean:
	$(Q) $(MAKE) -f $(MAKE_FILE_NAME) clean

.PHONY: clean