# IDL语法

## 接口定义

使用 interface 定义

```cpp
package OHOS.One;

option_stub_hooks on;     // 开启钩子功能
interface OHOS.Two.Test {

}
```

注意：
    1.文件名必须与接口名称一致，如这里的文件应该命名为Test.idl;
    2.可以使用 package 或 接口名前定义 两种方式设置接口的命名空间；当同时定义时，后者优先级更高。如上面实例 OHOS.Two 命名空间将会生效。

## 数据类型

### 基础数据类型
| IDL文件写法 | 对应cpp的类型 | 功能和规格限制 | 生成的序列化代码 | 生成的反序列化代码 |
|-----------------|--------------|--------|-----------------------------------|-------------------------------------|
| void           | 默认为IPC错误码 |        | 无                            | 无                                 |
| boolean        | bool         |        | WriteInt32                      | ReadInt32                          |
| byte           | int8_t       |        | WriteInt32                      | ReadInt32                          |
| short          | short        |        | WriteInt32                      | ReadInt32                          |
| int            | int32_t      |        | WriteInt32                      | ReadInt32                          |
| long           | int64_t      |        | WriteInt64                      | ReadInt64                          |
| float          | float        |        | WriteFloat                      | ReadFloat                          |
| double         | double       |        | WriteDouble                     | ReadDouble                         |
| char           | char         |        | WriteInt32                      | ReadInt32                          |
| String         | std::string  |        | WriteString16(Str8ToStr16(inParam)) | Str16ToStr8(data.ReadString16()) |
| unsigned char  | uint8_t      |        | WriteUint8                      | ReadUint8                          |
| unsigned short | uint16_t     |        | WriteUint16                     | ReadUint16                         |
| unsigned int   | uint32_t     |        | WriteUint32                     | ReadUint32                         |
| unsigned long  | uint64_t     |        | WriteUint64                     | ReadUint64                         |
| FileDescriptor | int          |        | WriteFileDescriptor             | ReadFileDescriptor                 |
| T[] ，其中T表示其他数据类型 | std::vector<T> | VECTOR_MAX_SIZE = 102400 | WriteInt32 (数据的大小)，<br>然后通过 for 循环逐项序列化     | ReadInt32 (数据的大小)，<br>然后通过 for 循环逐项反序列化       |
| List<T> <br>其中T表示其他数据类型 | std::vector<T> | LIST_MAX_SIZE = 102400 | WriteInt32 (数据的大小)，<br>然后通过 for 循环逐项序列化   | ReadInt32 (数据的大小)，<br>然后通过 for 循环逐项反序列化       |
| Map<T1, T2> <br>其中T1和T2表示其他数据类型   | std::unordered_map<T1, T2>  | MAP_MAX_SIZE = 102400 | WriteInt32 (数据的大小)，<br>然后逐项序列化 map 的 key 和 value | ReadInt32 (数据的大小)，<br>然后逐项反序列化 map 的 key 和 value，并组成新的 map |
| IRemoteObkect <br>需要在接口类上方声明：<br>sequenceable OHOS.IRemoteObject;   | sptr<IRemoteObject>  |  | WriteRemoteObject | ReadRemoteObject |



### 自定义序列化对象：用sequenceable声明

业务方在实现自定义对象时需要继承Parcelable，实现Marshalling和Unmarshalling函数。

| IDL文件写法                                                  | 对应cpp的类型 | 生成的.h中对头文件、命名空间的处理                    | 生成的序列化代码          | 生成的反序列化代码      |
| ------------------------------------------------------------ | ------------- | ----------------------------------------------------- | ------------------------- | ----------------------- |
| sequencealbe test.MySeq1;<br />头文件与类名一致              | MySeq1        | #include “my_seq1.h”<br />using test::MySeq1;         | WriteParcelable(&inParam) | ReadParcelable<myseq>() |
| sequencealbe <br />FooMyseq..test.MySeq2;<br />指定头文件    | MySeq2        | #include “foo_my_seq2.h”<br />using test::MySeq2;     | WriteParcelable(&inParam) | ReadParcelable<myseq>() |
| sequencealbe <br />FooTest.MySeq..test.MySeq3;<br />指定头文件及其路径 | MySeq2        | #include “foo_test/my_seq.h”<br />using test::MySeq3; | WriteParcelable(&inParam) | ReadParcelable<myseq>() |



### 自定义回调

业务方在实现自定义回调时需要继承 IRemoteBroker

| IDL文件写法                                            | 对应cpp的类型 | 生成的.h中对头文件、命名空间的处理                 | 生成的序列化代码                       | 生成的反序列化代码                            |
| ------------------------------------------------------ | ------------- | -------------------------------------------------- | -------------------------------------- | --------------------------------------------- |
| interface test.MyIntf1;<br />头文件与类名一致          | sptr<MyIntf1> | #include “my_Intf1.h”<br />using test::MyIntf1;    | WriteRemoteObject(inParam->AsObject()) | iface_cast<MyIntf1>(reply.ReadRemoteObject()) |
| interface FooMyIntf..test.MyIntf2;<br />指定头文件路径 | MySeq2        | #include “foo_my_intf.h”<br />using test::MyIntf2; | WriteRemoteObject(inParam->AsObject()) | iface_cast<MyIntf1>(reply.ReadRemoteObject()) |



### 复合数据类型

复合数据类型定义在独立的idl文件中，通过import进行调用，在复杂类型idl文件开头定义命名空间，如：package OHOS.custom.idlFile;

注：不支持在复合类型的idl文件中使用sequenceable

| IDL文件写法                                                  | 对应cpp的类型 | 生成的.h中对头文件、命名空间的处理                           | 生成的序列化代码 | 生成的反序列化代码 |
| ------------------------------------------------------------ | ------------- | ------------------------------------------------------------ | -------------------------------------- | --------------------------------------------- |
| enum FooEnum {<br/>    ENUM_ONE = 1,<br/>    ENUM_TWO,<br/>    }; | enum          | enum class FooEnum : int32_t {<br/>    ENUM_ONE = 1,<br/>    ENUM_TWO,<br/>}; | WriteUint64 | ReadUint64 |
| struct FooStruct {<br/>    int id;<br/>    String name;<br/>    enum FooEnum type;<br/>}; | struct   | struct FooStruct {<br/>    int32_t id;<br/>    std::string name;<br/>    FooEnum type;<br/>    sptr<IRemoteObject> caption;<br/>}; | 根据包含的类型对应生成Marshalling函数 | 根据包含的类型对应生成Unmarshalling函数 |
| union FooUnion {<br/>    enum FooEnum enumType;<br/>    int unionInfo;<br/>}; | union | union FooUnion {<br/>    FooEnum enumType;<br/>    int32_t unionInfo;<br/>} __attribute__ ((aligned(8))); | WriteUnpadBuffer | ReadUnpadBuffer |



### callback

在接口前使用[callback]表示回调接口;

| IDL文件写法                                                  | 生成的代码示例      |
| ------------------------------------------------------------ | ------------------- |
| [callback] interface OHOS.fcallback.ICallback {<br/>    void void_test_func();<br/>} | 生成代码同interface |



## import

import关键字用于idl文件关联解析其他idl文件。

| IDL文件写法                                                  | 功能和规格限制                                               |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| import相对路径、支持平级目录、上级目录和下级目录。如：<br /><br />import ../Types;<br />关联父目录下的Types.idl文件。<br /><br />import Callback/ICallback；<br />关联同级目录下的Callback目录下的ICallback.idl文件。 | 1.import的idl文件无需带“.idl”后缀<br />2.被import的idl文件只能是复杂类型idl文件回调idl文件；<br />3.只支持在接口idl文件或回调idl文件中使用import，不支持在复杂类型idl文件中使用import。 |



## 方法修饰符

在方法面前使用[]，表示修饰方法的属性。可以多个方法修饰符同时使用，使用逗号分隔

| 方法装饰器名称 | 功能和规格限制                                               | idl文件中的写法     | 生成的代码示例                                               |
| -------------- | ------------------------------------------------------------ | ------------------- | ------------------------------------------------------------ |
| oneway         | 表示接口是异步                                               | [oneway]            | 有oneway装饰：MseeageOption option(MessageOption::TF_ASYNC)<br />无oneway装饰：MessageOption option(MessageOption::TF_SYNC) |
| cacheable      | 表示缓存                                                     | [cacheable 100]     | 增加对ApiCacheManager相关接口的调用                          |
| ipccode        | 用于指定IPC代码<br />最小值为1，最大值为16777215(0x00ffffff)<br />不能重复<br />支持十进制和十六进制<br />仅在SA模式生成cpp时生效，其他模式不支持 | [ipccode 20]        | remote->SendRequest(static_cast<uint32_t>(IFooIpcCode::COMMAND_BOOL_TEST_FUNC), data, reply, option); |
| ipcincapacity  | 用于指定Proxy类的IPC容量。<br />单位为KB;<br />最小值为1，最大值为1310071<br />只支持十进制<br />仅在SA模式生成cpp时生效，其他模式不支持 | [ipcincapacity 10]  | if (!data.SetMaxCapacity(CAPACITY_BOOL_TEST_FUNC_10)) {<br />        return ERR_INVALID_VALUE;<br />} |
| ipcoutcapacity | 用于指定Stub类的IPC容量,<br />规格限制同ipcincapacity        | [ipcoutcapacity 20] | if (!data.SetMaxCapacity(CAPACITY_BOOL_TEST_FUNC_10)) {<br />        return ERR_INVALID_VALUE;<br />} |



## 参数修饰符

在方法的参数前使用[]，表示修饰参数为入参、出参或既是入参又是出参。

| 方法装饰器名称 | 功能                 | idl文件中的写法 | 生成的代码示例                                               |
| -------------- | :------------------- | --------------- | ------------------------------------------------------------ |
| in             | 表示入参             | [in]            | 在Proxy端进行序列化，在Stub端进行反序列化                    |
| out            | 表示出参             | [out]           | 在Proxy端调用SendRequset需要反序列化，<br />在Stub端调用方法后需要序列化 |
| inout          | 表示既是入参又是出参 | [inout]         | 在Proxy端进行序列化，执行SendRequset后再做反序列化，<br />在Stub端进行反序列化，调用方法后在再进行序列化。 |

不同的参数修饰符会影响生成代码的参数有无 “const”  "&"  等，可在下面路径对应的参数下找到对应的生成规则。

比如seq类型对应路径为 code/foundation/ability/idl_tool/idl_tool_2/codegen/SA/type/sa_seq_type_emitter.cpp:
```cpp
switch (mode) {
    case TypeMode::PARAM_IN:
        return StringHelper::Format("const %s&", typeName_.c_str());
    case TypeMode::PARAM_INOUT:
    case TypeMode::PARAM_OUT:
        return StringHelper::Format("%s&", typeName_.c_str());
    case TypeMode::NO_MODE:
    case TypeMode::LOCAL_VAR:
        return typeName_;
    default:
        return "unknown type";
}
```



## 综合使用示例

IFooCustom.idl:
```cpp
enum FooEnum {
    ENUM_ONE = 1,
    ENUM_TWO = 2,
    ENUM_NESTING = (ENUM_ONE << ENUM_TWO)
};

struct FooStruct {
    int id;
    String name;
    enum FooEnum type;
    IRemoteObject caption;
};

union FooUnion {
    enum FooEnum enumType;
    int unionInfo;
};

struct RequestInfo {
    unsigned char[] initData;
    Map<String, String> optionalData;
};
```

Callback/ICallback.idl:
```cpp
package OHOS.fcallback;

[callback] interface OHOS.fcallback.ICallback {
    void void_test_func();
}
```

Test.idl:
```cpp
import ../IFooCustom;
import ../fcallback/ICallback;
import callback/IFoocallback;

interface OHOS.test.IFoo {
    FooEnum enum_test_func([in] FooEnum inParam, [out] FooEnum outParam, [inout] FooEnum inoutParam);
    FooStruct struct_test_func([in] FooStruct inParam, [out] FooStruct outParam, [inout] RequestInfo inoutParam);
    FooUnion union_test_func([in] FooUnion inParam, [out] FooUnion outParam, [inout] FooUnion inoutParam);
    IFoocallback callback_test_func([in] IFoocallback inParam, [out] IFoocallback outParam, [inout] IFoocallback inoutParam);
    ICallback callback_test_func2([in] ICallback inParam, [out] ICallback outParam, [inout] ICallback inoutParam);
    void ApplyQuickFix([in] String[] quickFixFiles, [in] boolean isDebug);
    void GetAllAppSuspendState([in] Map<int, FooStruct> inApp, [out] Map<int, FooStruct> outApp);
}
```