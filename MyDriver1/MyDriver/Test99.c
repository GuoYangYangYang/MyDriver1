#include "MyDriver.h"

/*



*/

/*++

派遣函数

派遣函数是Windows驱动程序中的重要概念。驱动程序的主要功
能是负责处理I/O请求，其中大部分I/O请求是在派遣函数中处理的。
用户模式下所有对驱动程序的I/O请求，全部由操作系统转化为
一个叫做IRP的数据结构，不同的IRP数据会被“派遣”到不同的派遣
函数（Dispatch Function）中。

--*/

/*++

IRP与派遣函数

IRP的处理机制类似Windows应用程序中的“消息处理”机制，驱动
程序接受到不同类型的IRP后，会进入不同的派遣函数，在派遣函数中
IRP得到处理。

--*/

/*++

IRP

在Windows内核中，由一种数据结构叫做IRP（I/O Request
Package），即输入输出请求包。它是与输入输出相关的重要数据结构。
上层应用程序与底层驱动程序通信时，应用程序会发出I/O请求。操作
系统将I/O请求转化为相应的IRP数据，不同类型的IRP会根据类型传递
到不同的派遣函数内。
IRP具有两个基本的属性，一个是MajorFunction，另一个是
MinorFunction，分别记录IRP的主要类型和子类型。操作系统根据
MajorFunction将IRP“派遣”到不同的派遣函数中，在派遣函数中还
可以继续判断这个IRP属于哪种MinorFunction。
NT式驱动程序和WDM驱动程序都是在DriverEntry函数中注册
派遣函数的。

--*/

/*++

PDRIVER_DISPATCH MyDriverDispatchCreate()
{

}

PDRIVER_DISPATCH MyDriverDispatchClose()
{

}

PDRIVER_DISPATCH MyDriverDispatchWrite()
{

}

PDRIVER_DISPATCH MyDriverDispatchRead()
{

}

--*/

/*++

在DriverEntry的驱动对象pDriverObject中，有个函数指针
数组MajorFunctio。函数指针数组是个数组，每个元素都记录着一
个函数的地址。通过设置这个数组，可以将IRP的类型和派遣函数关
联起来。此处只对四种类型的IRP设置了派遣函数，而IRP的类型并
不只是这四种。对于其他没有设置的IRP类型，系统默认这些IRP类型
与_IopInvalidDeviceRequest函数关联。
在进入DriverEntry之前，操作系统会将_IopInvaildDeviceRequest
的地址填满整个MajorFunction数组。

--*/

/*++

IRP类型

IRP的概念类似Windows应用程序中“消息”的概念。在Win32编程
中，程序是由“消息”驱动的。不同的消息，会被分发到不同的消息处理
函数中。如果没有对应的处理函数，它会进入到系统默认的消息处理
函数中。
IRP的处理类似这种方式。文件I/O的相关函数，如CreateFile、
ReadFile、WriteFile、CloseHandle等函数会使操作系统产生出
IRP_MJ_CREATE、IRP_MJ_READ、IRP_MJ_WRITE、IRP_MJ_CLOSE
等不同的IRP，这些IRP会被传送到驱动程序的派遣函数中。
另外，内核中的文件I/O处理函数，如ZwCreateFile、ZwReadFile、
ZwWriteFile、ZwClose，它们同样会创建相应的IRP_MJ_CREATE、
IRP_MJ_READ、IRP_MJ_WRITE、IRP_MJ_CLOSE等不同的IRP，并将
IRP传送到相应驱动的相应派遣函数中。
还有些IRP是由系统的某个组件创建的，比如IRP_MJ_SHUTDOWN
是在Windows的即插即用组件在即将关闭系统的时候发出的。

--*/

/*++

对派遣函数的简单处理

大部分的IRP都源于文件I/O处理Win32API，如CreateFile、
ReadFile等。处理这些IRP简单的方法就是在相应派遣函数中，将IRP
的状态设置为成功，然后结束IRP的请求，并让派遣函数返回成功。结
束IRP的请求使用函数IoCompleteRequest。下面的代码演示了一种
最简单的处理IRP请求的派遣函数。

--*/

NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
)
{
	// 避免编译器关于未引用参数的警告
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter MyDriverDispatchRoutin\n"));
	// 对一般IRP的简单操作
	NTSTATUS status = STATUS_SUCCESS;
	// 设置IRP完成状态
	pIrp->IoStatus.Status = status;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 0; // btyes xfered
									// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave MyDriverDispatchRoutin\n"));
	return status;
}

/*++

派遣函数设置了IRP的完成状态为STATUS_SUCCESS。这样，发起
I/O请求的Win32 API（如WriteFile）将会返回TRUE。相反，如果将
IRP的完成状态设置为不成功，这时发起I/O请求的Win32 API（如
WriteFile）将会返回FALSE。这种情况时，可以使用GetLastError
Win32 API得到错误代码。所得的错误代码会和IRP设置的状态相一致。
除了设置IRP的完成状态，派遣函数还要设置这个IRP请求操作了
多少字节。在本例中，将操作字节数简单地设置为了0。如果是ReadFile
产生的IRP，这个字节数代表对设备写了多少字节。最后，派遣函数将
IRP请求结束，这是通过IoCompleteRequest函数完成的。
IoCompleteRequest的声明如下：

VOID IoCompleteRequest (
IN PIRP Irp,
IN CCHAR PriorityBoost
);

Irp: 代表需要被结束的IRP。
PriorityBoost: 代表线程恢复时的优先级别。

以ReadFile为例，ReadFile的内部操作大体是这样的:
1. ReadFile调用ntdll中的NtReadFile。其中ReadFile函数
是Win32 API，而NtReadFile函数是Native API。
2. ntdll中的NtReadFile进入到内核模式，并调用系统服务中
的NtReadFile函数。
3. 系统服务函数NtReadFile创建IRP_MJ_WRITE类型的IRP，
然后他将这个IRP发送到某个驱动程序的派遣函数中。NtReadFile
然后去当代一个事件，这时当前线程进入“睡眠”状态，也可以说当前
线程被阻塞住或者线程处于“Pending”状态。
4. 在派遣函数中一般会将IRP请求结束，结束IRP是通过
IoCompleteRequest函数内部会设置刚才等待的事件，“事件”的线程
被恢复运行。
例如，在读一个很大的文件（或者设备）时，ReadFile不会立刻
返回，而是等待一段时间。这段时间就是当前线程“睡眠”的那段时间。
IRP请求被结束，标志这个操作执行完毕，这时“睡眠”的线程被唤醒。
IoCompleteRequest函数中的第二个参数PriorityBoost代表
一种优先级，指的是被阻塞的线程以何种优先级恢复运行。一般情况下
优先级设置为IO_NO_INCREMENT。对某些特殊情况，需要将阻塞的
线程以“优先”的身份恢复运行。如键盘、鼠标等输入设备，它们需要
更快地反应。

--*/

/*++

通过设备链表打开设备

要打开设备，必须通过设备的名字才能得到该设备的句柄。每个
设备都有设备名称，如MyDriver1驱动程序的设备名为“\Device\MyDriver1Device”，
但是设备名无法被用户模式下的应用程序查询到，设备名只能被内核
模式下的程序查询到。
在应用程序中，设备可以通过符号链表进行访问。驱动程序通过
IoCreateSymbolicLink函数创建符号链表。MyDriver1驱动程序的
设备所对应的符号链表“\??\MyDriver1”。在编写程序时，符号链接
的写法就变成了“\\.\MyDriver1”，写成C语言的字符串就是
“\\\\.\\MyDriver1”。
下面的代码演示了如何利用CreateFile来打开设备句柄，以及
如何利用CloseHandle关闭设备句柄。在打开和关闭设备句柄的时候，
操作系统内部会创建IRP，并将IRP发送到相应的派遣函数中。
这种办法是将AnsiString1中的Buffer指针等于string1指针。
这种初始化的优点是操作简单，用完后不用清理内存。但带来另外一个
问题，如果修改string1，同时会导致AnsiString1字符发生变化。

--*/

