#pragma once
#include <ntifs.h> 
#define BUFFER_SIZE 1024

// 用户自定义链表结构
typedef struct _MYDATASTRUCT {
	// List Entry需要作为_MYDATASTRUCT结构体的一部分
	LIST_ENTRY ListEntry;
	// 下面是自定义的数据
	ULONG x;
	ULONG y;
	ULONG number;
} MYDATASTRUCT, *PMYDATASTRUCT;

/*++

设备扩展

--*/
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice; // 通过pDevice指向设备对象
	UNICODE_STRING ustrDeviceName; // 设备名称
	UNICODE_STRING ustrSymLinkName; // 符号链接名
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/*++

其中：
1.  pDevice:
设备对象中的DeviceExtension指向设备扩展，pDevice可以
指向设备对象。
2.  ustrDeviceName:
设备名。
3.  ustrSymLinkName:
设备的符号链接名。

在设备扩展中，记录以上几个信息，以备其他回调函数或者派遣
函数使用。使用的时候，只需从驱动设备中获取，类似于以下代码：
pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

--*/

NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);

VOID DisplayItsProcessName();
VOID Test01();     // 实验函数01————申请内存
VOID Test02();     // 实验函数02————链表
VOID Test03();     // 实验函数03————Lookaside
VOID Test04();     // 实验函数04————运行时函数
VOID Test05();     // 实验函数05————try-except
NTSTATUS Test06(); // 实验函数06————try-finally
VOID Test07();     // 实验函数07————字符串的初始化
VOID Test08();     // 实验函数08————字符串的初始化
VOID Test09();     // 实验函数09————字符串的复制
VOID Test10();     // 实验函数10————字符串的比较
VOID Test11();     // 实验函数11————字符串的大写
VOID Test12();     // 实验函数12————字符串与数字的转换
VOID Test13();     // 实验函数13————两种字符串之间的转换
VOID Test14();     // 实验函数14————文件的创建
VOID Test15();     // 实验函数15————文件的打开
VOID Test16();     // 实验函数16————文件的打开2
VOID Test17();     // 实验函数17————文件的查询、修改文件属性
VOID Test18();     // 实验函数18————文件的写操作
VOID Test19();     // 实验函数19————文件的读操作
VOID Test20();     // 实验函数20————创建关闭注册表

PDRIVER_DISPATCH MyDriverDispatchCreate(); // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchClose();  // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchWrite();  // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchRead();   // 派遣函数
NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
);
