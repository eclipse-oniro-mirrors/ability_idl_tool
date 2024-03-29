# Copyright (c) 2021 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

export PWD := $(abspath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
export TARGET_DIR := $(PWD)
export BUILD_DIR = $(TARGET_DIR)/build
export TARGET:=$(TARGET_DIR)/idl-gen

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