#include "MyDriver.h"

/*++

实验函数1————申请内存

--*/

/*++

分页与非分页内存

Windows规定有些虚拟内存页面是可以交换到文件中，这类内存
被称为分页内存。而有些虚拟内存页永远不会交换到文件中，这些内
存被称为非分页内存。
当程序的中断请求级在DISPATCH_LEVEL之上时（包括DISPATCH_LEVEL
层），程序只能使用非分页内存，否则将导致蓝屏死机。


--*/

/*--

在编译DDK提供的例程时，可以指定某个例程和某个全局变量是
载入分页内存还是非分页内存，需要做如下定义：

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

--*/

/*++

如果将某个函数载入到分页内存中，我们需要在函数的实现中
加入如下代码：

#pragma PAGEDCODE
VOID SomeFunction1()
{
PAGED_CODE();
// do somethings
}

其中，PAGED_CODE()是DDK提供的宏，它只在check版本中生效。
它会检验这个函数是否运行低于DISPATCH_LEVEL的中断请求级，如果
等于或高于这个中断请求级，将产生一个断言。

--*/

#pragma PAGEDCODE
VOID SomeFunction1()
{
	PAGED_CODE();
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

/*++

如果将某个函数载入到非分页内存中，我们需要在函数的实现中加
入如下代码：

#pragma LOCKEDCODE
VOID SomeFunction2()
{
// do somethings
}

--*/

#pragma LOCKEDCODE
VOID SomeFunction2()
{
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

VOID Test01()
{
	KdPrint(("Test01————beginning\n"));
	SomeFunction1();
	SomeFunction2();
	KdPrint(("Test01————finishing\n"));
}

/*++

还有一种特殊情况，就是某个例程需要在初始化的时候载入内存，
然后就可以从内存中卸载掉。这种情况指出现扎起DriverEntry情况
下，尤其是NT式的驱动，DriverEntry会很长，占据很大的空间，为
了节省内存，需要及时地从内存中卸载掉。代码如下：

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
IN PDRIVER_OBJECT pDriverObject,
IN PUNICODE_STRING RegistryPath)
{
// do somethings
}

--*/