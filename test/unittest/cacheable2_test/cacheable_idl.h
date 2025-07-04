/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_CACHEABLE_IDL_FILE_H
#define OHOS_CACHEABLE_IDL_FILE_H

#include <string>

namespace OHOS {
namespace Idl {
namespace TestCommon {

const std::string CACHE_TIME_001_IDL_NAME = "CacheTimeTest001.idl";

const std::string CACHEABLE_TIME_TEST001_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest001 {\n"
"   [cacheable 100]void test001([in] short param1);\n"
"	[cacheable 0]void test002([in] short param1);\n"
"	[cacheable 00200]void test003([in] short param1);\n"
"	[cacheable 2147483647]void test004([in] short param1);\n"
"}";

const std::string CACHE_TIME_002_IDL_NAME = "CacheTimeTest002.idl";

const std::string CACHEABLE_TIME_TEST002_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest002 {\n"
"   [cacheable 2147483648]void test001([in] short param1);\n"
"}";

const std::string CACHE_TIME_003_IDL_NAME = "CacheTimeTest003.idl";

const std::string CACHEABLE_TIME_TEST003_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest003 {\n"
"   [cacheable 100]void test001([in] short param1);\n"
"	[cacheable 00200]void test002([in] short param1);\n"
"	[cacheable 200, ipccode 0X823]void test003([in] short param1);\n"
"	[cacheable 200, ipccode 0x90]void test004([in] short param1);\n"
"}";

const std::string CACHE_TIME_004_IDL_NAME = "CacheTimeTest004.idl";

const std::string CACHEABLE_TIME_TEST004_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest004 {\n"
"   [cacheable 200]void test001([in] short param1, [in] List<boolean> param2);\n"
"}";

const std::string CACHE_TIME_005_IDL_NAME = "CacheTimeTest005.idl";

const std::string CACHEABLE_TIME_TEST005_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest005 {\n"
"   [cacheable -900]void test001([in] short param1);\n"
"   [cacheable +900]void test002([in] short param1);\n"
"   [cacheable 100, ipccode 0B110]void test003([in] short param1);\n"
"   [cacheable 100, ipccode 0b10]void test004([in] short param1);\n"
"}";

const std::string CACHE_TIME_006_IDL_NAME = "CacheTimeTest006.idl";

const std::string CACHEABLE_TIME_TEST006_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest006 {\n"
"   [cacheable --900]void test001([in] short param1);\n"
"   [cacheable ++900]void test002([in] short param1);\n"
"   [cacheable 9<<1]void test003([in] short param1);\n"
"   [cacheable 9>>1]void test004([in] short param1);\n"
"   [cacheable 2 100]void test005([in] short param1);\n"
"   [cacheable 200 void test006([in] short param1);\n"
"   [cacheable 0823]void test007([in] short param1);\n"
"   [cacheable 0h82]void test008([in] short param1);\n"
"   /comment test009\n"
"}";

const std::string CACHE_TIME_007_IDL_NAME = "CacheTimeTest007.idl";

const std::string CACHEABLE_TIME_TEST007_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest007 {\n"
"   [cacheable";

const std::string CACHE_TIME_008_IDL_NAME = "CacheTimeTest008.idl";

const std::string CACHEABLE_TIME_TEST008_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest008 {\n"
"   //comment test001\n"
"   [cacheable 200]void test001([in] short param1);\n"
"   /*comment test002*/\n"
"   [cacheable 200]void test002([in] short param1);\n"
"   /**\n"
"    * @brief comment test003\n"
"    *\n"
"    * @param [in] param1 short\n"
"    */\n"
"   [cacheable 200]void test003([in] short param1);\n"
"}";

const std::string CACHE_TIME_009_IDL_NAME = "CacheTimeTest009.idl";

const std::string CACHEABLE_TIME_TEST009_IDL_CONTENT =
"interface idl.OHOS.CacheTimeTest009 {\n"
"   [cacheable 100]void test001([in] short param1);\n"
"	[cacheable 100]void test002([in] short param1);\n"
"	[cacheable 200]void test003([in] long param1);\n"
"	[cacheable 200]void test004([in] short param1);\n"
"}";

} // namespace TestCommon
} // namespace Idl
} // namespace OHOS

#endif  // OHOS_CACHEABLE_IDL_FILE_H