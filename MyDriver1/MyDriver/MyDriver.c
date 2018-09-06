#include <ntifs.h> 
#include "MyDriver.h"

// 在编译DDK提供的例程时，可以指定某个例程和某个全局变量是
// 载入分页内存还是非分页内存，需要做如下定义：
#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath);
VOID DriverUnload(IN PDRIVER_OBJECT objDriver);

/*++

对于NT式驱动来说，主要的函数是DriverEntry例程、卸载例程及各个
IRP的派遣例程。

--*/

/*++

驱动加载过程与驱动入口函数（DriverEntry）：

和编写普通应用程序一样，驱动程序有个入口函数，也就是首先
被执行的函数。这个函数通常被命名为DriverEntry，但不是一定。
DriverEntry主要是对驱动程序进行初始化工作，它是由系统
进程所调用的。在Windows中有个特殊的进程叫做系统进程，打开
进程管理器，里面有个名为System的进程就是系统进程。系统进程在
系统启动的时候，就已经被创建了。
驱动加载的时候，系统进程启动新的线程，调用执行体组件中的
对象管理器，创建一个驱动对象。这个驱动对象是一个DRIVER_OBJECT
的结构体。另外，系统进程调用执行体组件中的配置管理程序，查询此
驱动程序对应的注册表中的项。
系统线程调用驱动程序的DriverEntry例程时，同时传进两个参
数，分别是pDriverObject和pRegistryPath。其中一个是指向刚才
被创建驱动对象的指针，另外一个是指向设备服务键的键名字符串的
指针。在DriverEntry中，主要功能是对系统进程创建的驱动对象进
行初始化。另外，设备服务键的键名有时候需要保存下，因为这个字
符串不是长期存在的（函数返回后可能消失）。如果以后想使用这个
UNICODE字符串就必须先把它复制到安全的地方。
这个字符串的内容一般是\REGISTRY\MACHINE\SYSTEM\ControlSet
\Services\[服务名]。在驱动程序中，字符串用UNICODE字符串来
表示。UNICODE是宽字符集，每个字符用16位表示。

UNICODE用数据结构UNICODE_STRING表示：

typedef struct _LSA_UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

其中：
1.  Length:
记录这个字符串用多少字节记录。如果字符串由N个字符，
那么Length将会是N的2倍。
2.  MaximumLength:
记录buffer的大小，也就是这个结构最大能记录的字节数。
MaximumLength要大于或等于Length。
3.  Buffer:
记录字符串的指针。与ASCII字符串不同，这里的字符串
每个字符都是16位。

--*/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath)
{
	// 避免编译器关于未引用参数的警告
	UNREFERENCED_PARAMETER(pRegistryPath);

	// NTSTATUS status;

	// 打印一行字符串
	KdPrint(("Enter DriverEntry\n"));

	// 注册其他驱动调用函数入口
	pDriverObject->DriverUnload = DriverUnload;

	DisplayItsProcessName();

	// 实验函数1————申请内存
	Test01();

	// 实验函数2————链表
	Test02();

	// 实验函数3————Lookaside
	Test03();

	// 实验函数4————运行时函数
	Test04();

	// 实验函数5————try-except
	Test05();

	// 实验函数6————try-finally
	Test06();

	// 实验函数7————字符串的初始化
	Test07();

	// 实验函数8————字符串的初始化
	Test08();

	// 实验函数9————字符串的复制
	Test09();

	// 实验函数10————字符串的比较
	Test10();

	// 实验函数11————字符串的大写
	Test11();

	// 实验函数12————字符串与数字的转换
	Test12();

	// 实验函数13————两种字符串之间的转换
	Test13();

	// 实验函数14————文件的创建
	Test14();

	// 实验函数15————文件的打开
	Test15();

	// 实验函数16————文件的打开2
	Test16();

	// 实验函数17————文件的查询、修改文件属性
	Test17();

	// 实验函数18————文件的写操作
	Test18();

	// 实验函数19————文件的读操作
	Test19();

	// 实验函数20————创建关闭注册表
	Test20();

	// 设置派遣函数
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyDriverDispatchRoutin;
	// pDriverObject->MajorFunction[IRP_MJ_CLOSE];
	// pDriverObject->MajorFunction[IRP_MJ_WRITE];
	// pDriverObject->MajorFunction[IRP_MJ_READ];

	// 创建驱动设备对象
	// status = CreateDevice(pDriverObject);

	KdPrint(("DriverEntry end\n"));

	/*++
	DriverEntry的返回值是NTSTATUS的数据，NTSTATUS是被
	定义为32位的无符号长整型。在驱动程序开发中，人们习惯用
	NTSTATUS返回状态。其中0~0X7FFFFFFF，被认为是正确的状态，
	而0X80000000~0XFFFFFFFF，被认为是错误的状态。其中有个非常
	有用的宏NT_SUCCESS，被用来检测状态是否正确。
	常见的NTSTATUS值有：
	#define STATUS_SUCCESS           ((NTSTATUS)0x00000000L)
	#define STATUS_BUFFER_OVERFLOW   ((NTSTATUS)0x80000005L)
	#define STATUS_UNSUCCESSFUL      ((NTSTATUS)0xC0000001L)
	#define STATUS_NOT_IMPLEMENTED   ((NTSTATUS)0xC0000002L)
	#define STATUS_ACCESS_VIOLATION  ((NTSTATUS)0xC0000005L)
	#define STATUS_INVALID_HANDLE    ((NTSTATUS)0xC0000008L)
	#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
	DriverEntry的返回值如果表示成功，则意味着加载驱动成
	功，否则意味着加载驱动失败，调用对象管理程序销毁驱动对象。
	--*/
	return STATUS_SUCCESS;
}
/*++

最后需要说明的是DriverEntry参数的修饰"In"。"In","OUT",
"INOUT"在DDK中都被定义为空串，它们的功能类似于程序注释，
当看到一个"In"参数时，应该认定该参数是纯粹用于输入目的。
"OUT"参数代表这个参数仅用于函数的输出参数。"INOUT"用于既可
以输入又可以输出的参数。例如DriverEntry例程，它的
DriverObject指针是IN参数，即使用者不能改变这个指针本身，
但完全可以改变它指向的对象。
在DriverEntry函数中，一般设置卸载例程数和IRP的派遣函数，
另外还有一部分代码负责创建设备对象。设置卸载例程和设置派遣函数
都是对驱动对象的设置。设备对象中的MajorFunction是一个函数指
针数组，IRP_MJ_CREATE、IRP_MJ_CLOSE、IRP_MJ_WRITE代表数组
的第几个元素。

--*/


/*++

DriverUnload例程

在驱动对象中会设置DriverUnload例程，此例程在驱动被卸载
的时候调用。在NT式驱动中，DriverUnload一般负责删除在DriverEntry
中创建的设备对象，并且将设备对象所关联的符号链接删除。另外，
DriverUnload还负责对一些资源进行回收。

--*/

VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	// 避免编译器关于未引用参数的警告
	UNREFERENCED_PARAMETER(pDriverObject);

	PDEVICE_OBJECT pNextObj;

	// 什么也不做，只打印一行字符串
	KdPrint(("Enter DriverUnload\n"));

	// 得到下一个设备对象
	pNextObj = pDriverObject->DeviceObject;

	// 枚举所有设备对象
	while (pNextObj != NULL)
	{
		// 得到设备扩展
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		// 删除符号链接
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;

		// 删除设备
		IoDeleteDevice(pDevExt->pDevice);
	}

	// 什么也不做，只打印一行字符串
	KdPrint(("Finish DriverUnload\n"));
}

/*++

在DriverUnload中，传进来驱动对象。根据驱动对象，就可以遍历
所有由该驱动对象创建的设备对象。通过驱动对象的DeviceObject域
，就可以找到随后的设备对象。在以上代码中，驱动对象其实只创建了
一个设备对象。因此，在DriverUnload的调用中，也只会删除一个设备
对象。
删除设备对象的函数是IoDeleteDevice，函数原型是：

VOID IoDeleteDevice(IN PDEVICE_OBJECT DeviceObject);

其参数就是要被删除的设备对象指针。在DriverUnload中，除了
要删除设备对象，同时还要对设备对象关联的符号链接进行删除。删除
符号链接的函数是IoDeleteSymbolicLink，函数原型是：

NTSTATUS IoDeleteSymbolicLink(
IN PUNICODE_STRING SymbolibLinkName
);

其中：
1.  SymbolicLinkName:
表示已经被注册了的符号链接。
2.  返回值：
表示删除符号链接是否成功。

--*/