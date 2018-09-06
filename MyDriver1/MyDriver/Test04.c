#include "MyDriver.h"

/*++



--*/

/*++

运行时函数

一般编译器厂商，在发布其编译器的同时，会将运行时函数一起
发布给用户。运行时函数是程序运行的时候必不可少的，它由编译器
提供。针对不同的操作系统，运行时函数的实现方法不同，但接口
基本保持一致。

1. 内存间复制（非重叠）

在驱动程序开发中，经常用到内存的复制。DDK提供以下函数：

VOID RtlCopyMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: 表示要复制内存的目的地址。
Source: 表示要复制内存的源地址。
Length: 表示要复制内存的长度，单位是字节。
和该函数功能类似的还有RtlCopyBytes，这两个函数的参数完全
一样，功能也完全一样，只是在不用的平台下有不同的实现。

2. 内存间复制（可重叠）

用RtlCopyMemory可以复制内存，但其内部没有考虑内存重叠的
情况。有三段等长的内存，ABCD。如果需要将A到C段的内存复制到B
到D这段内存上，这时B到C段的内存就是重叠的部分。
RtlCopyMemory函数的内部实现方法是依靠memory函数实现的。
根据C99定义，memory没有考虑重叠的部分，因此它不能保证重叠部
分是否被复制。
为了保证重叠部分也被正确复制，C99规定memmove函数完成这
个任务。这个函数对两个内存是否重叠进行了判断，但却牺牲了速度。
如果程序员能确保复制的内存没有重叠，将选择使用memopy函数。
如果不能保证内存是否重叠，则使用memmove函数。同样，为了保证可
移植性，DDK用宏对memmove进行了封装，名称变为RtlMoveMemory。

VOID RtlMoveMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: 表示要复制内存的目的地址。
Source: 表示要复制内存的源地址。
Length: 表示要复制内存的长度，单位是字节。

3. 填充内存

驱动程序开发中，还经常用到对某段内存区域用固定字节填充。
DDK为程序员提供了函数RtlFillMemory。实际的函数时memset函数

VOID RtlFillMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length,
IN UCHAR Fill
);

Destination: 目的地址。
Length: 长度。
Fill: 需要填充的字节。

在驱动程序开发中，还经常要对某段内存填零，DDK提供的宏是
RtlZeroBytes和RtlZeroMemory。

VOID RtlZeroMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length
);

Destination: 目的地址。
Length: 长度。

4. 内存比较

驱动程序开发中，还会用到比较两块内存是否一致。该函数是
RtlCompareMemory，其声明是：

ULONG RtlEqualMemory (
CONST VOID *Source1,
CONST VOID *Source2,
SIZE_T Length
);

Source1: 比较的第一个内存地址。
Source2: 比较的第二个内存地址。
Length: 比较的长度，单位为字节。
返回值: 相等的字节数。

RtlEqualMemory通过判断返回值和Length是否相等，来判断两
块内存是否完全一致。在两段内存一致的情况下返回非零值，在不一
致的情况下返回零。

--*/

#define BUFFER_SIZE 1024
#pragma INITCODE
VOID Test04()
{
	KdPrint(("Test04————beginning\n"));

	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// 用零填充内存
	RtlZeroMemory(pBuffer, BUFFER_SIZE);
	PUCHAR pBuffer2 = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// 用固定字节填充内存
	RtlFillMemory(pBuffer2, BUFFER_SIZE, 0xAA);

	// 内存复制
	RtlCopyMemory(pBuffer, pBuffer2, BUFFER_SIZE);

	// 判断内存是否一致
	ULONG ulRet = RtlCompareMemory(pBuffer, pBuffer2, BUFFER_SIZE);
	if (ulRet == BUFFER_SIZE)
	{
		KdPrint(("The two blocks are same.\n"));
	}

	KdPrint(("Test04————finishing\n"));
}

/*++

使用C++特性分配内存

微软编译器没有提供内核模式下的new模式符。应该对new和
delete操作符进行重载。
重载new和delete操作符，有两种方法，一种是全局重载，一种
是在类中重载。

--*/

/*++

数据类型

在C语言中，整数类型有8位、16位、32位三种类型，而在DDK中
又新添了一种64位的长整数整数。64位的整数只有无符号形式，用
LONGLONG类型表示。64位形整数的常量前面是一个数字，后面加上
i64结尾。如：

LONGLONG llvalue = 100i64;

除了LONGLONG以外，DDK还提供了一种64位整数的表示方法，即
LARGE_INTEGER数据结构，其区别是LONGLONG是基本的数据，而
LARGE_INTEGER是数据结构。LARGE_INTEGER定义如下:

typedef union _LARGE_INTEGER {
struct {
ULONG LowPart;
LONG HighPart;
};
struct {
ULONG LowPart;
LONG HighPart;
} u;
LONGLONG QuadPart;
} LARGE_INTEGER;

这是个联合体，联合体中的三个元素可以认为是LARGE_INTEGER
的三个定义。

1. LARGE_INTEGER可以认为是由两个部分组成。一个是低32位的整
数HighPart，一个是高32位的整数HighPart。在little endian的
情况下，低32位数字在前，高32位数字在后。赋值100如下：

LARGE_INTEGER LargeValue;
LargeValue.LowPart = 100;
LargeValue.HighPart = 0;

2. LARGE_INTEGER可以认为是由两个部分组成，一个是低32位的整
数HighPart，一个是高32位的整数HighPart。在big endian的
情况下，高32位数字在前，低32位数字在后。赋值100如下：

LARGE_INTEGER LargeValue;
LargeValue.u.LowPart = 100;
LargeValue.u.HighPart = 0;

3. LARGE_INTEGER等价于LONGLONG数据。在这种情况下，如果赋值
100，如下：

LARGE_INTEGER LargeValue;
LargeValue.QuadPart = 100i64;

--*/

/*++

返回状态值

DDK大部分函数的返回值类型是NTSTATUS类型。参考DDK.h文件。
NTSTATUS类型与LONG等价。

--*/

/*++

检查内存可用性

在驱动程序开发中，对内存的操作要格外小心，如果某段内存是
只读的。而驱动程序试图去写操作，就会导致系统的崩溃。同样，当
某段内存是不可读的情况下，驱动程序试图去读，同样会导致系统的
崩溃。
DDK提供了两种函数，可在不知道某段内存是否可读写的情况下，
试探这段内存可读写性。这两个函数分别是ProbeForRead和
ProbeForWrite。

VOID ProbeForRead (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: 需要被检查的内存的地址。
Length: 需要被检查的内存的长度，单位是字节。
Alignment: 描述该段内存是以多少字节对齐的。

VOID ProbeForWrite (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: 需要被检查的内存的地址。
Length: 需要被检查的内存的长度，单位是字节。
Alignment: 描述该段内存是以多少字节对齐的。
这两个函数不是返回该段内存是否可读写，而是当不可读写的时候
会引发一个异常。这个异常需要用到微软编译器提供的“结构体异常”
处理办法。

--*/