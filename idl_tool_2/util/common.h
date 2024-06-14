/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDL_COMMON_H
#define OHOS_IDL_COMMON_H

namespace OHOS {
namespace Idl {
    constexpr const char *TAB = "    ";
    constexpr const char *TAG = "IDL-GEN";

#ifndef __MINGW32__
    constexpr char SEPARATOR = '/';
#else
    constexpr char SEPARATOR = '\\';
#endif

    enum class SystemLevel {
        INIT,
        /** mini system */
        MINI,
        /** lite system */
        LITE,
        /** std system */
        FULL,
    };

    enum class GenMode {
        INIT,
        /** generate hdi code of low mode, it is only supported by 'MINI' SystemLevel */
        LOW,
        /** generate hdi code of pass through mode, it is only supported by 'LITE' or 'std' SystemLevel */
        PASSTHROUGH,
        /** generate hdi code of ipc mode, it is only supported by 'std' SystemLevel */
        IPC,
        /** generate hdi code of kernel mode, it is only supported by 'LITE' or 'std' SystemLevel */
        KERNEL
    };

    enum class Language {
        C,
        CPP,
        JAVA,
        RUST,
        TS,
    };

    enum class InterfaceType {
        HDI,
        SA,
        SM,
    };

} // namespace Idl
} // namespace OHOS
#endif // OHOS_IDL_COMMON_H
