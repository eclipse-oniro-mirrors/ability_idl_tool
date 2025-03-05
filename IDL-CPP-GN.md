# 业务侧进行工具集成

## 导入IDL工具gni

  必须引用下面这个idl.gni

```gn
import("//build/config/components/idl_tool/idl.gni")
```

## idl_gen_interface内部配置选项


|        变量名         | 作用                                                         | 使用示例                                                     |
| :-------------------: | :----------------------------------------------------------- | :----------------------------------------------------------- |
|        src_idl        | 【必选项，与sources二选一】字符串类型。业务定义的idl文件，推荐和BUILD.gn在同一个路径；支持相对路径；<br/>配置后external_deps需要增加"**c_utils:utils**"、"**ipc:ipc_core**"依赖。不支持生成so。 | src_idl = "IThermalSrv.idl"                                  |
|        sources        | 【必选项，与src_idl二选一】列表类型。业务定义的idl文件，支持相对路径，支持多个idl文件。功能同src_idl，可用sources代替src_idl。支持生成so。 | sources = [<br/>  "IThermalSrv.idl",<br/>]                   |
|   sources_callback    | 【可选项】列表类型。业务定义的回调idl文件，为接口idl文件的相对路径。<br />当使用相对路径时，需要在对应include_dir加上<br />include_dirs = [<br />        "${target_gen_dir}",<br />        "${target_gen_dir}/ + 相对路径"<br />] | sources_callback = [<br/> "cb/IThermalActionCallback.idl", <br/>"cb/IThermalLevelCallback.idl",<br/>"cb/IThermalTempCallback.idl", <br/>] |
|    sources_common     | 【可选项】列表类型。业务定义的复杂类型的idl文件，为接口idl文件的相对路径。<br />当使用相对路径时，需要在对应include_dir加上<br />include_dirs = [<br />        "${target_gen_dir}",<br />        "${target_gen_dir}/ + 相对路径"<br />] | source_common = [<br/> "type/IThermalSensorInfo.idl",<br/>]  |
|       dst_file        | 【已作废】配置生成的文件名称；目前配置后不生效，相关的文件名称可自动生成； |                                                              |
|        hitrace        | 【可选项】开启hitrace，值是hitrace_meter.h文件中定义的uint64_t类型标识，需要填入常量的变量名；<br/>配置后external_deps需要增加“**hitrace:hitrace_meter**”依赖 | HITRACE_TAG_ABILITY_MANAGER                                  |
|     log_domainid      | 【可选项】开启hilog，Domain ID 使用16进制的整数；<br/>配置后external_deps需要增加“**hilog:libhilog**”依赖；<br/>**注：需要和log_tag成对配置** | log_domainid = "0xD001718"                                   |
|        log_tag        | 【可选项】开启hilog，字符串类型tag名、一般为子系统名称；<br/>配置后external_deps需要增加“**hilog:libhilog**”依赖；<br/>**注：需要和log_tag成对配置** | log_tag = "idlTest"                                          |
|     client_enable     | 【可选项】开启死亡监听，生成死亡监听对应的文件。             | client_enbale = true                                         |
|      source_cpp       | 根据idl文件中对自定义对象的使用，编译为so时需要增加自定义对应使用的cpp的编译，默认为空 |                                                              |
|        configs        | 编译so时增加configs配置                                      |                                                              |
| sequenceable_pub_deps | 编译so时增加public_deps配置                                  |                                                              |
| sequenceable_ext_deps | 编译so时增加external_desp配置                                |                                                              |
|     innerapi_tags     | 编译so时增加innerapi_tags                                    |                                                              |
|       sanitize        | 编译so时增加sanitize，<br/><br/>so使用方式的默认值为：<br/>  sanitize = {<br/>  cfi = true<br/>  cfi_cross_dso = true<br/>  debug = false<br/>} |                                                              |
|        cflags         | 编译so时增加cflags配置                                       |                                                              |
|       cflags_cc       | 编译so时增加cflags_cc配置                                    |                                                              |
|    subsystem_name     | 【必选项】编译so时增加subsystem_name，与业务保持一致         | subsystem_name = "powermgr"                                  |
|       part_name       | 【必选项】编译so时增加part_name，与业务保持一致              | part_name = "thermal_manager"                                |

示例：

```cpp
idl_gen_interface("thermalsrv_interface") {
    sources = [
        "IThermalSrc.idl",
    ]
    sources_callback = [
        "cb/IThermalActionCallback.idl",
        "cb/IThermalLevelCallback.idl",
        "cb/IThermalTempCallback.idl",
    ]
    sources_common = [
        "type/IThermalSensorInfo.idl",
    ]
    sub_include = [ "native/include" ]
    sequenceable_ext_deps = [ "bundle_framework:appexecfwk_base" ]
    sources_cpp = [rebase_path("${thermal_manager_path}/frameworks/native/thermal_srv_sensor_info.cpp") ]
    configs = [
        "${utils_path}:utils_config",
        ":thermalsrc_private_config",
        "${utils_path}:coverage_flags",
    ]
    log_domainid = "0xD003900"
    log_tag = "QuickFixManagerService"
    subsystem_name = "powermgr"
    part_name = "thermal_manager"
}
```

## include_dir配置

| 变量名               | 作用                    |
|:-----------------:| --------------------- |
| ${target_gen_dir} | 【必选项】该目录包含idl工具生成的头文件 |
| 业务方自己定义的头文件路径     | 业务方自己定义的头文件路径         |

示例：

```gn
config("thermalsrv_public_config") {
    include_dirs = [
        "${target_gen_dir}",
        "native/include",
    ]
}


// 若想在其他build.gn中依赖上面的config可以定义一个ohos_source_set，其中public_configs，接着在其他build.gn中依赖下面这个ohos_source_set


ohos_source_set("example") {
    public_configs = [ ":thermalsrv_public_config" ]
}
```



## so使用方式

只需在业务的 deps 中增加对生成的 so 的依赖， IDL工具就会生成对应 so。

- proxy so名字为："lib" + target_name + "_proxy"

- stub so 名字为："lib" + target_name + "_stub"

示例：

```gn
ohos_shared_library("thermalsrv_client") {
    sanitize = {
        cfi = true
        cfi_cross_dso = true
        debug = false
    }
    sources = [ "${thermal_frameworks}/native/thermal_mgr_client.cpp" ]
    configs = [
        "${utils_path}:utils_config",
        ":thermalsrv_private_config",
        "${utils_path}:coverage_flags",
    ]
    public_configs = [ ":thermalsrv_public_config" ]
    deps = [
        ":libthermalsrv_interface_proxy",
    ]
    external_deps = [
        "bundle_framework:appexecfwk_base",
        "c_utils:utils",
        "hilog:libhilog",
        "ipc:ipc_core",
        "samgr:smagr_proxy",
    ]
    subsystem_name = "powermgr"
    part_name = "thermal_manager"
}
```

## 源码使用方式

使用 get_target_outputs 获取IDL工具返回的生成文件路径，将其**过滤后**加入到 sources 中进行编译，同时需要在 deps 中增加对模板调用的依赖。

下例将生成的 stub 代码生成一个 source_set，业务方可依赖此 source_set 生成 so。

示例：

```gn
ohos_source_set("thermalsrv_stub") {
    configs = [
        "${utils_path}:utils_config",
        ":thermalsrv_private_config",
        "${utils_path}:coverage_flags",
    ]
    output_values = get_target_outputs(":thermalsrv_interface")
    sources = []
    sources += filterinclude(output_values, [ "*srv_stub.cpp" ] )
    sources += filterinclude(output_values, [ "*sensor_info.cpp" ] )
    sources += filterinclude(output_values, [ "*callback_proxy.cpp" ] )
    public_configs = [ ":thermalsrv_public_config" ]
    deps = [
        ":thermalsrv_interface",
    ]
    external_deps = [
        "bundle_framework:appexecfwk_base",
        "c_utils:utils",
        "hilog:libhilog",
        "ipc:ipc_core",
        "samgr:smagr_proxy",
    ]
    subsystem_name = "powermgr"
    part_name = "thermal_manager"
}
```

| 步骤  | 配置说明                                                     |
| :---: | ------------------------------------------------------------ |
| 第①步 | 业务client端编译目标，内部编译的sources由业务实现，deps依赖了proxy的编译目标； |
| 第②步 | proxy编译目标，内部编译的sources由IDL工具根据.idl文件生成，deps依赖了idl_gen_interface配置的编译目标； |
| 第③步 | 工具生成代码的action执行目标，目标名称由业务自定义，idl_gen_interface模板由idl.gni(//build/config/components/idl_tool/idl.gni)提供； |
| 第④步 | 调用到工具生成代码的action执行目标，可配置相关变量传递到idl.gni模板中； |
| 第⑤步 | 调用get_target_outputs获取生成的代码文件的完整路径，添加到proxy编译目标的sources中； |



# 自测试native用例

## 获取ondemand工具

        编译目标：ondemand；

        源码路径：//foundation/systemabilitymgr/samgr/services/samgr/native/test/unittest/src/ondemand_helper.cpp

        使用方法：

```batch
# 测试SA的SAID为1494

# 加载 SA
./ondemand sa load 1494

# 卸载 SA
./ondemand sa unload 1494
```

        推工具脚本：**推送后需要给二进制增加执行权限**

```batch
hdc target mount
hdc file send "%cd%\ondemand" /data/ondemand
hdc shell chmod 777 /data/ondemand
pause
```



## SA的配置文件

### listen_test.cfg文件，推送到设备的 /system/etc/init 目录

```json
{
    "service" :[{
            "name" : "listen_test",
            "path" : ["/system/bin/sa_main", "/system/profile/listen_test.json"],
            "ondemand" : true,
            "secon" : "u:r:listen_test:s0",
            "permission" : ["ohos.permission.DISTRIBUTED_DATASYNC"],
            "uid" : "system",
            "gid" : ["system", "shell"]
        }
    ]
}
```



### listen_test.json文件，推送到设备的 /system/profile 目录

```json
{
    "process" : "listen_test",
    "systemability" :[
        {
            "name" : "1494",
            "libpath" : "libidl_sa_test.z.so",
            "run-o-create" : false,
            "distributed" : false,
            "dump-level" : 1
        }
    ]
}
```



### 推送配置文件的脚本

```batch
hdc target mount
hdc file send "%cd%\listen_test.cfg" /system/etc/init
hdc file send "%cd%\listen_test.json" /system/profile
pause
```



## 编译"测试SA"服务端so

    注：先在 code/foundation/ability/idl_tool/test/unittest/BUILD.gn 中增加 "../native/idl_sa_test:unittest" 依赖

    编译目标：idl_sa_test

    源码路径：//foundation/ability/idl_tool/test/unittest/idl_sa_test/listen_ability/BUILD.gn

    产物路径：//out/generic_generic_arm_64only/general_all_phone_standard/ability/idl_tool/libidl_sa_test.z.so

    推 so 脚本：

```batch
hdc target mount
hdc file send "%cd%\libidl_sa_test.z.so" /system/lib64/libidl_sa_test.z.so
pause
```



## 编译测试用例

    注：先在 code/foundation/ability/idl_tool/test/unittest/BUILD.gn 中增加 "../native/idl_sa_test:unittest" 依赖

    编译目标：idl_sa_client_test

    源码路径：//foundation/ability/idl_tool/test/unittest/idl_sa_test/BUILD.gn

    产物路径：//out/generic_generic_arm_64only/general_all_phone_standard/tests/unittest/idl_sa_client_test

    推测试用例脚本：**推送后需要给二进制增加执行权限**

```batch
hdc target mount
hdc file send "%cd%\idl_sa_client_test" /data/idl_sa_client_test
pause
```



## 执行步骤

### 验证同步func

```batch
# 执行后抓日志验证功能调用
./idl_sa_client_test --gtest_filter="IdlSaUnitTest.IdlSaProxyTest001"
```

### 验证异步func

```batch
# 执行后抓日志验证功能调用
./idl_sa_client_test --gtest_filter="IdlSaUnitTest.IdlSaProxyTest002"
```

### 验证死亡回调

```batch
# 执行后抓日志验证功能调用
./idl_sa_client_test --gtest_filter="IdlSaUnitTest.IdlSaRemoveDeathTest001"

# 执行后，再在另一个命令窗口执行 SA卸载
./data/ondemand sa unload 1494
```

### 同步加载SA

```batch
# 执行后抓日志验证功能调用
./idl_sa_client_test --gtest_filter="IdlSaUnitTest.IdlSaLoadTest001"
```

### 异步加载SA

```batch
# 执行后抓日志验证功能调用
./idl_sa_client_test --gtest_filter="IdlSaUnitTest.IdlSaLoadTest002"
```
