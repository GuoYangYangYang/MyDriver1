#include <ntifs.h> 

// 2018-9-3 11:53
// 在编译DDK提供的例程时，可以指定某个例程和某个全局变量是
// 载入分页内存还是非分页内存，需要做如下定义：
#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

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

驱动对象（DRIVER_OBJECT）
每个驱动程序会有唯一的驱动对象与之对应，并且这个驱动对
象是在驱动加载的时候，被内核中的对象管理程序所创建的。
驱动对象用DRIVER_OBJECT数据结构表示，它作为驱动的一个
实例被内核加载，并且对一个驱动只加载一个实例。确切的说，是
由内核中的I/O管理器负责加载的。驱动程序需要在DriverEntry中
初始化。
DRIVER_OBJECT数据结构如下：

typedef struct _DRIVER_OBJECT {
CSHORT Type;
CSHORT Size;
PDEVICE_OBJECT DeviceObject;
ULONG Flags;
PVOID DriverStart;
ULONG DriverSize;
PVOID DriverSection;
PDRIVER_EXTENSION DriverExtension;
UNICODE_STRING DriverName;
PUNICODE_STRING HardwareDatabases;
PFAST_IO_DISPATCH FastIoDispatch;
PDRIVER_INITIALIZE DriverInit;
PDRIVER_STARTIO DriverStartIo;
PDRIVER_UNLOAD DriverUnload;
PDRIVER_DISPATCH MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
} DRIVER_OBJECT;
typedef struct _DRIVER_OBJECT *PDRIVER_OBJECT;

其中：
1.  DeviceObject:
每个驱动程序会有一个或多个设备对象。其中，每个设备
对象都有一个指针指向下一个驱动对象，最后一个设备对象指
向空。此处的DeviceObject指向驱动对象的第一个设备对象。
通过DeviceObject，就可以遍历驱动对象里的所有设备对象。
设备对象是由程序员自己创建的，而非操作系统完成，在驱动
被卸载的时候，遍历每个设备对象，并将其删除。
2.  DriverName:
顾名思义，DriverName记录的是驱动程序的名字。这里用
UNICODE字符创记录，该字符串一般为\Driver\[驱动程序名称]。
3.  HardwareDatabase:
这里记录的是设备的硬件数据库键名，这里同样用UNICODE
字符串记录。该字符串一般为\REGISTRY\MACHINE\HARDWARE\
DESCRIPTION\SYSTEM。
4.  FastIoDispatch:
文件驱动中用到的派遣函数。
5.  DriverStartIo:
记录StartIO例程的函数地址，用于串行化操作。
6.  DriverUnload:
指定驱动卸载时所用的回调函数地址。
7.  MajorFunction:
MajorFunction域记录的是一个函数指针数组，也就是
MajorFunction是一个数组，数组中的每个成员记录着一个指针，
每一个指针指向的是一个函数。这个函数就是处理IRP的派遣函数。

--*/

/*++

设备对象（DEVICE_OBJECT）
每个驱动程序会创建一个或多个设备对象，用DEVICE_OBJECT数
据结构表示。每个设备对象都会有一个指针指向下一个设备对象，因此
就形成一个设备链。设备链的第一个设备是由DRIVER_OBJECT结构体
中指明的。设备对象保存设备特征和状态的信息。
DEVICE_OBJECT数据结构如下：

typedef struct _DEVICE_OBJECT {
struct _DRIVER_OBJECT *DriverObject;
struct _DEVICE_OBJECT *NextDevice;
struct _DEVICE_OBJECT *AttachedDevice;
struct _IRP *CurrentIrp;
ULONG Flags;
struct _DEVOBJ_EXTENSION *DeviceObjectExtension;
ULONG DeviceType;
int StackSize;
} DEVICE_OBJECT;
typedef struct _DEVICE_OBJECT *PDEVICE_OBJECT;

其中：
1.  DriverObject:
指向驱动程序中的驱动对象。同属于一个驱动程序的驱动
对象指向的是统一驱动对象。
2.  NextDevice:
指向下一个设备对象。这里指的下一个设备对象是同属于一
个驱动对象的设备，也就是同一个驱动程序创建的若干设备对象。
每个设备对象根据NextDevice域形成链表，从而可以枚举每个
设备对象。
3.  AttachedDevice:
指向下一个设备对象。这里指的是。如果有更高一层的驱动
附加到这个驱动的时候，AttachedDevice指向的就是那个更高
一层的驱动。
4.  CurrentIrp:
在使用StartIO例程的时候，此域指向的是当前IRP结构。
5.  Flags:
此域是一个32位的无符号整型。每一个位有具体的含义：
DO_BUFFERED_IO：读写操作使用缓冲方式（系统复制缓冲
区）访问用户模式数据。
DO_EXCLUSIVE：一次只允许一个线程打开设备句柄。
DO_DIRECT_IO：读写操作使用直接方式（内存描述符号）
访问用户模式数据。
DO_DEVICE_INITIALIZING：设备对象正在初始化。
DO_POWER_PAGABLE：必须在PASSIVE_LEVEL级上处理
IRP_MJ_PNP请求。
DO_POWER_INRUSH：设备上电期间需要大电流。
6.  DeviceExtension:
指向的是设备的扩展对象。每个设备都会指定一个设备扩展
对象，设备扩展对象记录的是设备自己特殊定义的结构体，也就是
由程序员自己定义的结构体。另外，在驱动程序中，应该尽量避免
全局变量的使用，因为全局变量涉及不容易同步的问题。解决的
方法是将全局变量存在设备扩展里。
7.  DeviceType:
指明设备的类型。FILE_DEVICE_FILE_SYSTEM指明为：
文件系统设备对象。当制作虚拟设备时，应选择FILE_DEVICE_UNKOWN
类型的设备。
8.  StackSize:
在多层驱动情况下，驱动与驱动之间会形成类似堆栈的结构。
IRP会依次从最高层传递到最底层。StackSize描述的就是这个
层数。

--*/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath);
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);
PDRIVER_DISPATCH MyDriverDispatchCreate(); // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchClose(); // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchWrite(); // 派遣函数
PDRIVER_DISPATCH MyDriverDispatchRead(); // 派遣函数
VOID DriverUnload(IN PDRIVER_OBJECT objDriver);
VOID DisplayItsProcessName();
VOID SomeFunction1();
VOID SomeFunction2();
VOID Test1();
VOID Test2();
VOID Test3();
VOID Test4();
VOID Test5();
NTSTATUS Test6();
VOID Test7();
VOID Test8();
NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
);
VOID Test9();
VOID Test10();
VOID Test11();
VOID Test12();
VOID Test13();
VOID Test14();
VOID Test15();
VOID Test16();
VOID Test17();

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
	Test1();

	// 实验函数2————链表
	Test2();

	// 实验函数3————Lookaside
	Test3();

	// 实验函数4————运行时函数
	Test4();

	// 实验函数5————try-except
	Test5();

	// 实验函数6————try-finally
	Test6();

	// 实验函数7————字符串的初始化
	Test7();

	// 实验函数8————字符串的初始化
	Test8();

	// 实验函数9————字符串的复制
	Test9();

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

创建设备对象
在NT式的驱动中，创建设备对象是由IoCreateDevice内核函数
完成的。

NTSTATUS IoCreateDevice (
IN PDRIVER_OBJECT DriverObject,
IN ULONG DeviceExtensionSize,
IN PUNICODE_STRING DeviceName OPTIONAL,
IN DEVICE_TYPE DeviceType,
IN ULONG DeviceCharacteristics,
IN BOOLEAN Exclusive,
OUT PDEVICE_OBJECT *DeviceObject
);

其中：
1.  DriverObject:
输入参数，每个驱动程序中。会有唯一的驱动对象与之对应，但
每个驱动对象会有若干个设备对象。DriverObject指向的就是驱动
对象的指针。
2.  DeviceExtensionSize:
输入参数，指定设备扩展的大小，I/O管理器会根据这个大小，在
内存中创建设备扩展，并与驱动对象关联。
3.  DeviceName:
输入参数，设置设备对象的名字。
4.  DeviceCharacteristics:
输入参数，设置设备对象的名字。
5.  Exclusive:
输入参数，设置设备对象是否为内核模式下使用，一般为TRUE。
6.  DeviceObject:
输入参数，I/O管理器负责创建这个设备对象，并返回设备对象
的地址。
7.  返回值:
返回此函数的调用状态。

设备名称用UNICODE字符串指定，并且字符串必须是"\Device\
[设备名]"的形式。在Windows下的所有设备都是以类似名字命名的，
例如，磁盘分区的C盘、D盘、E盘、F盘就是被命名为"\Device\
HarddiskVolume1"、"\Device\HarddiskVolume2"、"\Device\
HarddiskVolume3"、"\Device\HarddiskVolume4"。
当然也可以不指定设备名字，如果在IoCreateDevice中没有指
定设备对象的名字，I/O管理器会自动分配一个数字作为设备的设备
名，例如，"\Device\00000001"、"\Deivece\00000002"、
"\Device\00000003"。
如果指定了设备名，只能被内核模式下的其他驱动所识别。但是
在用户模式下的应用程序无法识别这个设备。让用户模式下的应用能
识别设备有两种方法，第一种是通过符号链接找到设备，第二种是通
过设备接口找到设备。
符号链接可以理解为设备对象起了一个“别名”。设备对象的名称
只能被内核模式的驱动识别，而别名也可以被用户模式下的应用程序
识别。

--*/

/*++

创建符号链接的函数IoCreateSymBolicLink，其函数申明如下：

NTSTATUS IoCreateSymbolicLink(
IN PUNICODE_STRING SymbolicLinkName,
IN PUNICODE_STRING DeviceName
);

其中：
1.  SymbolicLinkName:
输入参数，符号链接的字符串，用UNICODE字符串表示。
2.  DeviceName:
输入参数，设备对象名的字符串，用UNICODE字符串表示。
3.  返回值:
返回创建符号链接是否成功。

在内核模式下，符号链接是以“\??\”开头的（或者是“\DosDevices”
开头的），如C盘就是“\??\C:”（或者“\DosDevices\C:”）。而在
用户模式下，则是以“\\.\”开头的，如C盘就是“\\.\C:”。

--*/

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

/*++

设备的创建函数

--*/
NTSTATUS CreateDevice(
	IN PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	// 创建设备名称
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevice");

	// 创建设备
	status = IoCreateDevice(pDriverObject,
		sizeof(DEVICE_EXTENSION),
		&devName,
		FILE_DEVICE_UNKNOWN,
		0, TRUE,
		&pDevObj);

	// 判断设备对象是否创建成功
	if (!NT_SUCCESS(status))
		return status;

	// 将设备设置为缓冲区设备
	pDevObj->Flags |= DO_BUFFERED_IO;

	// 得到设备扩展
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	// 设置设备扩展的设备对象
	pDevExt->ustrDeviceName = devName;

	// 创建符号链接
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\MyDriver");
	pDevExt->ustrSymLinkName = symLinkName;

	// 创建符号链接
	status = IoCreateSymbolicLink(&symLinkName, &devName);

	// 判断是否成功创建符号链接
	if (!NT_SUCCESS(status))
	{
		// 删除符号链接
		IoDeleteDevice(pDevObj);
		return status;
	}

	return STATUS_SUCCESS;
}

/*++

其中在创建设备对象的时候，指定的设备类型为FILE_DEVICE_UNKNOWN，
说明此设备是常用设备之外的设备，一般虚拟设备常使用这个作为设备
类型。
在创建设备对象后，对Flags的DO_BUFFERED_IO位进行设置，
这里是将设备设置成“缓冲区设备”。关于将设备设置成“缓冲设备”
或者“直接设备”。随后设定设备的设备扩展，设备扩展是程序员
自定义的。

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

/*++

Windows驱动程序和进程的关系

驱动程序可以看成是一个特殊的DLL文件被应用程序加载到虚拟
内存中，只不过加载地址是内核模式地址，而不是用户模式地址。它
能访问的只是这个进程的虚拟内存，而不是其他进程的虚拟地址。需
要指出的是，Windows驱动程序里的不同例程运行在不同的进程中。
DriverEntry例程和AddDevice例程是运行在系统（System）进程
中。这个进程是Windows中非常的进程，也是Windows第一个运行的
进程。当需要加载的时候，这个进程中会有一个线程将驱动加载到内
核模式地址空间内，并调用DriverEntry例程。
而其他的一些例程，例如，IRP_MJ_READ和IRP_MJ_WRITE的派
遣函数会运行于应用程序的“上下文”中。所谓运行在进程的“上下文”
中，指的是运行于某个进程的环境中，所能访问的虚拟地址是这个进
程的虚拟地址。
在代码中打印一行log信息，这行信息打印出当前进程的进程名。
如果当前进程是发起I/O请求的进程，则说明在进程的“上下文”中。
下面函数可以显示当前进程的进程名。

--*/

VOID DisplayItsProcessName()
{
	// 得到当前进程，PsGetCurrentProcess函数是得到当前运行的进程
	PEPROCESS pEProcess = PsGetCurrentProcess();
	// 得到当前进程名称
	PTSTR ProcessName = (PTSTR)((ULONG)pEProcess + 0x174);
	KdPrint(("%s\n", ProcessName));
}

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

VOID Test1()
{
	KdPrint(("Test1————beginning\n"));
	SomeFunction1();
	SomeFunction2();
	KdPrint(("Test1————finishing\n"));
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

/*++

分配内核内存

Windows驱动程序使用的内存资源十分珍贵，分配内存时要尽量
节约。和应用程序一样，局部变量是存放在栈（Stack）空间中的。
但栈空间不会像应用程序那么大，所以驱动程序不适合递归调用或者
局部变量是大型结构体。如果需要大型结构体，则需要在堆（Heap）
中申请。
堆中申请内存的函数有以下几个，原型如下：

PVOID ExAllocatePool (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

PVOID ExAllocatePoolWithQuota (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithQuotaTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

其中PoolType是个枚举变量，如果此值为NonPagedPool，则分
配非分页内存。如果此值为PagedPool，则分配内存为分页内存。
NumberOfBytes是分配内存的大小，最好为4的倍数。
返回值分配的内存地址，一定是内核模式地址。如果返回0，则
代表分配失败。

--*/

/*++

链表结构

DDK提供了标准的双向链表。双向链表可以将链表形成一个环。BLINK
指针指向前一个元素，FLINK指针指向下一个元素。
以下是DDK提供的双向链表的数据结构。

typedef struct _LIST_ENTRY {
struct _LIST_ENTRY *Flink;
struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

--*/

/*++

链表初始化

每个双向链表都是以一个链表头作为链表的第一个元素。初次使用
链表头需要进行初始化，主要将链表头的Flink和Blink两个指针都指向
自己。这意味着链表头所代表的连是空链，初始化链表头用InitializeListHead
宏实现。
如何判断链表是否为空，可以判断链表头的Flink和Blink是否指
向自己。DDK为程序员提供了一个宏简化这种检查（IsLIstEmpty）
程序员需要自己定义链表中每个元素的数据类型，并将LIST_ENTRY
结构作为自定义结构的一个子域。LIST_ENTRY的作用是将自定义的
数据结构串成一个链表。

typedef struct _MYDATASTRUCT {
// List Entry需要作为_MYDATASTRUCT结构体的一部分
LIST_ENTRY ListEntry;
// 下面是自定义的数据
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;

--*/

/*++

从首部插入链表

对链表的插入有两种方式，一种是在链表的头部插入，一种是在
链表的尾部插入。
在头部插入链表使用语句InsertHeadList，用法如下：
InsertHeadList(&head, &mydata->ListEntry);
其中，head是LIST_ENTRY结构的链表头，mydata是用户定义的
数据结构，而它的子域ListEntry是包含其中的LIST_ENTRY数据结构

--*/

/*++

从尾部插入链表

另外一种插入方法是在链表的尾部进行插入。在尾部插入链表使
用InsertTailList，用法如下：
InsertTailList(&head, &mydata->ListEntry);
其中，head是LIST_ENTRY结构的链表头，mydata是用户定义的
数据结构，而它的子域ListEntry是包含其中的LIST_ENTRY数据结构

--*/

/*++

从链表删除

和插入链表一样，删除也有两种对应的方法。一种是从链表头部
删除，一种是从链表尾部删除。分别对应RemoveHeadList和
RemoveTailList函数，其使用方法如下：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
及
PLIST_ENTRY pEntry = RemoveTailList(&head);
其中，head是链表头，pEntry是从链表删除下来的元素中的
ListEntry。这里有两种情况：
1. 当自定一的数据结构的第一个字段是LIST_ENTRY时，如：
typedef struct _MYDATASTRUCT {
LIST_ENTRY ListEntry;
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;
此时，RemoveHeadList返回的指针可以被当作用户自定义的
指针，即：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PMYDATASTRUCT pMyData = (PMYDATASTRUCT) pEntry;
2. 当自定义的数据结构的第一个字段不是LIST_ENTRY时，即：
typedef struct _MYDATASTRUCT {
ULONG x;
ULONG y;
LIST_ENTRY ListEntry;
} MYDATASTRUCT, *PMYDATASTRUCT;
此时，RemoveHeadList返回的指针不可以当作用户自定义的
指针，此时需要通过pEntry的地址逆向算出自定义数据的指针。一般
通过pEntry在自定义数据中的偏移量，用pEntry减去这个偏移量，就
会得到用户自定义结构的指针的地址。DDK中提供了宏
CONTEAINING_RECORD，其用法如下：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PIRP pIrp = CONTAINING_RECORD(pEntry,
MYDATASTRUCT,
ListEntry);
DDK建议自定义数据结构无论第一个字段是否为ListEntry，最
好都使用CONTAINING_RECORD宏得到自定义数据结构的指针。

--*/



VOID Test2()
{
	KdPrint(("Test2————beginning\n"));
	LIST_ENTRY linkListHead;
	// 初始化链表
	InitializeListHead(&linkListHead);

	PMYDATASTRUCT pData;
	ULONG i = 0;
	// 在链表中插入10个元素
	KdPrint(("Begin insert to link list"));
	for (i = 0; i < 10; i++)
	{
		// 分配分页内存
		pData = (PMYDATASTRUCT)
			ExAllocatePool(PagedPool, sizeof(MYDATASTRUCT));
		pData->number = i;
		// 从头部插入链表
		InsertHeadList(&linkListHead, &pData->ListEntry);
	}

	// 从链表中取出，并显示
	KdPrint(("Begin remove from link list\n"));
	while (!IsListEmpty(&linkListHead))
	{
		// 从尾部删除一个元素
		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry,
			MYDATASTRUCT,
			ListEntry);
		// KdPrint(("%d\n", pData->number));
		ExFreePool(pData);
	}
	KdPrint(("Test2————finishing\n"));
}

/*++

使用Lookaside

如果驱动程序需要频繁地从内存中申请、回收固定大小的内存，
则应该使用Lookaside对象。
Lookaside是一个自动的内存分配容器。通过对Lookaside对象
申请内存，效率要高于直接向Windows申请内存。一般在如下使用：
1. 程序员每次申请固定大小的内存。
2. 申请和回收的操作十分频繁。
使用Lookaside对象首先要初始化Lookaside对象。有如下函数：
VOID ExInitializeNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PEREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
和
VOID ExInitializePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PFREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
这两个函数分别是对非分页和分页Lookaside对象进行初始化。

在初始化完Lookaside对象后，可以进行申请内存的操作了：
PVOID ExAllocateFromNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
和
PVOID ExAllocateFromPagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
这两个函数分别是对非分页内存和分页内存的申请。

对Lookaside对象回收内存的操作，有以下两个函数：
VOID ExFreeToNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
和
VOID ExFreeToPagedLookaside (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
这两个函数分别是对非分页内存和分页内存的回收。

在使用完Lookaside对象后，需要删除Lookaside对象，有以下
两个函数：
VOID ExDeleteNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
和
VOID ExDeletePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
这两个函数分别是对非分页和分页Lookaside对象的删除。

--*/

VOID Test3()
{
	KdPrint(("Test3————beginning\n"));

	// 初始化Lookaside对象
	PAGED_LOOKASIDE_LIST pageList;
	ExInitializePagedLookasideList(&pageList, NULL,
		NULL, 0, sizeof(MYDATASTRUCT), '1234', 0);
	INT ARRAY_NUMBER = 50;
	PMYDATASTRUCT MyObjectArray[50];

	// 模拟频繁申请内存
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		MyObjectArray[i] = (PMYDATASTRUCT)
			ExAllocateFromPagedLookasideList(&pageList);
	}

	// 模拟频繁回收内存
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		ExFreeToPagedLookasideList(&pageList,
			MyObjectArray[i]);
		MyObjectArray[i] = NULL;
	}

	// 删除Lookaside对象
	ExDeletePagedLookasideList(&pageList);

	KdPrint(("Test3————finishing\n"));
}

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
VOID Test4()
{
	KdPrint(("Test4————beginning\n"));

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

	KdPrint(("Test4————finishing\n"));
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

/*++

结构体异常处理（try-except块）

结构体异常处理是微软编译器提供的独特处理机制，这种处理方式
能在一定程序上在出现错误的情况下，免于程序崩溃。

1. 异常: 异常的概念类似于中断的概念。当程序中某段错误触发一个
异常，操作系统会寻找处理这个异常的处理函数。如果程序提供错误
处理函数，则进入错误处理函数，如果没有提供处理函数，则由操作
系统的默认错误处理函数处理。在内核模式下，操作系统默认处理错
误的方法往往很简单，直接让系统蓝屏，并在蓝屏上简单描述出错的
信息，之后系统就进入死机状态。

2. 回卷: 程序执行到某个地方出现异常错误时，系统会寻找出错点
是否处于一个try{}中，并进入try提供的异常处理代码。如果当前
try块没有提供异常处理，则会向更外一层的try块，寻找异常处理
代码。直到最外层try{}块也没有提供异常处理代码，则交由从操作
系统处理。

这种向更外一层寻找异常处理的机制，被称为回卷。一般处理
异常，是通过try-except块来处理的。

__try
{
}
__except(filter_value)
{
}

在被__try{}包围的块中，如果出现异常，会根据filter_value
的数值，判断是否需要在__except{}中处理。filter_value的数值
会有三种可能。

1. EXCEPTION_EXECUTE_HANDLER，该数值为1。进入到__except
进行错误处理，处理完不再回到__try{}块中，转为继续进行。

2. EXCEPTION_CONTINUE_SEARCH，该数值为0。不使用__except
块中的异常处理，转而向上一层回卷。如果已经是最外层，则向操作
系统请求异常处理函数。

3. EXCEPTION_CONTINUE_EXECUTION，该数值为-1.重复先前错误
的指令，这个很少用到。

ProbeForRead和ProbeForWrite函数可以和try-except块
配合，用来检查某段内存是否可读写。

--*/

VOID Test5()
{
	PVOID badPointer = NULL;

	KdPrint(("Test5————beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));

		// 判断空指针是否可读，显然会导致异常
		ProbeForWrite(badPointer, 100, 4);

		// 由于在上面引发异常，所以以后语句不会被执行
		KdPrint(("Leave __try block\n"));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Catch the exception\n"));
		KdPrint(("The program will keep going\n"));
	}

	// 该语句会被执行
	KdPrint(("Test5————finishing\n"));

}

/*++

结构体异常处理（try-finally块）

结构体异常处理还有另外一种使用方法，就是利用try-finally
块，强迫函数在退出前执行一段代码。

--*/

NTSTATUS Test6()
{
	KdPrint(("Test6————beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));
		// do somethings
		KdPrint(("Leave __try block\n"));
		return STATUS_SUCCESS;
	}

	__finally
	{
		KdPrint(("Enter __finally block\n"));
		// do somethings
		KdPrint(("Leave __fianlly block\n"));
		KdPrint(("Test6————finishing\n"));
	}

}

/*++

上面代码的__try{}块中，无论运行什么代码（即使是return
语句或者触发异常），在程序退出前都会运行__finally{}块中的
代码、这样的目的是，在退出前需要运行一些资源回收的工作，而
资源回收代码的最佳位置就是放在这个块中。
除此之外，使用try-finally块还可以某种程度上简化代码。

--*/

/*++

使用宏需要注意的地方

DDK提供了大量的宏，在使用这些宏的时候需要避免“侧效”。

1. 对于if、while、for这样的语句，不省略{}。这样就能完全保证
不出现“侧效”错误。

2. 在编写多行宏的时候，在宏的前后加上{}。如：
#define PRINT(mag ) {\
KdPrint (("===================\n"));\
KdPrint (mag);\
KdPrint (("===================\n"));\
}

--*/

/*++

内核下的字符串操作

和应用程序一样，驱动程序需要经常和字符串打交道。其中包括
ASCII字符串、宽字符串、还有DDK定义的ANSI_STRING数据结构和
UNICODE_STRING数据结构。

--*/

/*++

ASCII字符串和宽字符串

在应用程序中，往往使用两种字符：一种是char型的字符串，
负责记录ANSI字符集，它是指向一个char数组的指针，每个char型
变量的大小为一个字节，字符串是以0标志字符串结束。还有一种是
wchar_t数组的指针，wchar_t字符大小为两个字节，字符串以0标志
字符串结束。
ANSI字符的构造如下：
char* str1 = "abc";
str1指针指向的内容是61 626 300
UNICODE字符的构造如下：
wchar_t *str2 = L"abc";
str2指针指向的内容是6100 620 063 000 000。在构造字符串
的时候使用一个关键字“L"。编译器会自动生成所需要的宽字符。
在驱动程序开发中，DDK将char和wchar_t类别，替换成CHAR和
WCHAR类别。
对于这两类的字符串，DDK提供相应的字符串操作函数，例如，
strpy、sprintf、strcat、strlen等。但微软公司不鼓励直接使用
这些函数，取而代之的是使用同样功能的宏。

--*/

/*++

ANSI_STRING字符串与UNICODE_STRING字符串

DDK不鼓励程序员使用C语言的字符串，主要是因为：标准C的字
符串处理函数容易导致缓冲区溢出等错误。如果程序员不对字符串的
长度进行检验，很容易导致这个错误，从而导致整个操作系统的崩溃。
DDK鼓励程序员使用DDK自定义的字符串，这种数据格式的定义如下：

typedef struct _STRING {
USHORT Length;
USHORT MaximumLength;
PCHAR Buffer;
} STRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

这个数据结构对ASCII字符串进行了封锁。
Length: 字符的长度。
MaximumLength: 整个字符串缓冲区的最大长度。
Buffer: 缓冲区的指针。

和标准的字符串不同，STRING字符串不是以0标志字符结束。字符
长度依靠Length字段。在标准C中的字符串中，如果缓冲区长度是N，
那么只能容纳N-1个字符的字符串，这里因为要留一个字节存储NULL。
而在STRING字符串中，缓冲区的大小MaximumLength，最大的字符串
长度可以是MaximumLength，而不是MaximumLength-1。
与ANSI_STRING相对应，DDK将宽字符串封装成UNICODE_STRING
数据结构。

typedef struct _UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} UNICODE_STRING;

Length: 字符的长度。单位是字节。如果是N个字符，那么Length
等于N的2倍。
MaximumLength: 整个字符串缓冲区的最大长度，单位也是字节。
Buffer: 缓冲区的指针。

和ANSI_STRING不同，UNICODE_STRING的缓冲区是记录宽字符
的缓冲区。每个元素是宽字符。和ANSI_STRING一样，字符串的结束
不是以NULL为标志，而是依靠字段Length。

ANSI_STRING字符串和UNICODE_STRING字符串，KdPrint同样
提供了打印log的方法。

ANSI_STRING ansiString;
// 省去初始化
KdPrint("%Z\n", &ansiString); // 注意是%Z

而当打印一段宽字符的时候，需要进行以下操作。

UNICODE_STRING uniString;
// 省去初始化
KdPrint("%wZ\n", &uniString); // 注意是%wZ

--*/

/*++

字符初始化和销毁

ANSI_STRING字符串和UNICODE_STRING字符串使用前需要进行
初始化。有两种办法构造这个数据结构。

1. 方法一就是使用DDK提供了相应的函数

初始化ANSI_STRING字符串

VOID RtlInitAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PCSZ SourceString
);

DestinationString: 要初始化的ANSI_STRING字符串。
SourceString: 字符串的内容。

初始化UNICODE_STRING字符串

VOID RtlInitUnicodeString (
IN OUT PUNCODE_STRING DestinationString,
IN PCWSTR SourceString
);

DestinationString: 要初始化的UNICODE_STRING字符串。
SourceString: 字符串的内容。

以RtlInitAnsiString为例，其使用方法是：

ANSI_STRING AnsiString1;
CHAR* string1 = "hello";
RtlInitAnsiString(&AnsiString, string1);

--*/


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

VOID Test7()
{
	KdPrint(("Test7————beginning\n"));

	ANSI_STRING AnsiString1;
	
	CHAR * string1 = "hello";
	// 初始化ANSI_STRING字符串
	RtlInitAnsiString(&AnsiString1, string1);
	KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // 打印hello

	// 改变string1，此处会蓝屏，不能使用
	//string1[0] = 'H';
	//string1[1] = 'E';
	//string1[2] = 'L';
	//string1[3] = 'L';
	//string1[4] = 'O';
	// 改变string1，AnsiString同样会导致变化，此处会蓝屏。
	// KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // 打印HELLO
	KdPrint(("Test7————finishing\n"));
}

/*++

	另外一种方法是程序员自己申请内存。并初始化内存，当不用字
符串时，需要回收字符串占用的内存。

--*/

VOID Test8()
{
	KdPrint(("Test8————beginning\n"));
	UNICODE_STRING UnicodeString1 = { 0 };

	// 设置缓冲区大小
	UnicodeString1.MaximumLength = BUFFER_SIZE;

	// 分配内存
	UnicodeString1.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	WCHAR* wideString = L"hello";

	// 设置字符长度，因为是宽字符，所以是字符长度的2倍
	UnicodeString1.Length = 2 * wcslen(wideString);

	// 保证缓冲区足够大，否则程序终止
	ASSERT(UnicodeString1.MaximumLength >=
		UnicodeString1.Length);

	// 内存复制
	RtlCopyMemory(
		UnicodeString1.Buffer,
		wideString,
		UnicodeString1.Length);

	// 设置字符长度
	UnicodeString1.Length = 2 * wcslen(wideString);

	KdPrint(("UnicodeString:%wZ\n", &UnicodeString1));

	// 清理内存
	ExFreePool(UnicodeString1.Buffer);
	UnicodeString1.Buffer = NULL;
	UnicodeString1.Length = UnicodeString1.MaximumLength = 0;

	KdPrint(("Test8————finishing\n"));
}

/*++

	对于最后一步清理内存，DDK同样给出了简化函数，分别是
RtlFreeAnsiString和RtlFreeUnicodeString。这两个函数内部
调用了ExFreePool去回收内存。

--*/


/*++

字符串复制

	DDK提供针对ANSI_STRING字符串和UNICODE_STRING字符串的
复制字符串命令，分别是：
	ANSI_STRING字符串复制函数

VOID RtlCopyString (
	IN OUT PSTRING DestinationString,
	IN PSTRING SourceString OPTIONAL
);

	DestinationString: 目的字符串
	SourceString: 源字符串

	UNICODE_STRING字符串复制函数

VOID RtlCopyUnicodeString (
	IN OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString
);

	DestinationString: 目的字符串
	SourceString: 源字符串

	下面的代码演示了如何使用RtlCopyUnicodeString函数：

--*/

VOID Test9()
{
	KdPrint(("Test9————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;

	// 将初始化UnicodeString2复制到UnicodeString1
	RtlCopyUnicodeString(&UnicodeString2, &UnicodeString1);

	// 分别显示UnicodeString1和UnicodeString2
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	// 销毁UnicodeString2
	// 注意UnicodeString1不用销毁
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test9————finishing\n"));
}

/*++

字符串比较

	DDK提供了对ANSI_STRING字符串和UNICODE_STRING字符串的
相关字符串比较的命令，分别是ANSI_STRING字符串比较函数和
UNICODE_STRING字符串比较函数。

LONG RtlCompareString (
	IN PSTRING String1,
	IN PSTRING String2,
	BOOLEAN CaseInSensitive
);

	String1: 要比较的第一个字符串
	String2: 要比较的第二个字符串
	CaseInSensitive: 是否对大小写敏感
	返回值: 比较结果

LONG RtlCompareUnicodeString(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInSensitive
);

	String1: 要比较的第一个字符串
	String2: 要比较的第二个字符串
	CaseInSensitive: 是否对大小写敏感
	返回值: 比较结果

	这两个函数的参数形式相同，如果函数返回值为0，则表示两个
字符串相等。如果小于0，则表示第一个字符串小于第二个字符串；
如果大于0，则表示第一个字符串大于第二个字符串。
	同时，DDK又提供了RtlEqualString和RtlEqualUnicodeString
函数，其使用方法和上面两个函数类似。只是返回为非零代表相等，零
代表不相等。

--*/

VOID Test10()
{
	KdPrint(("Test10————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString2, L"Hello");

	// 判断字符串是否相等
	if (RtlEqualUnicodeString(&UnicodeString1, &UnicodeString2, TRUE))
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are equal\n"));
	}
	else
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are NOT equal"));
	}

	KdPrint(("Test10————finishing\n"));
}

/*++

字符串转化成大写

	DDK提供了对ANSI_STRING字符串和UNICODE_STRING字符串的
相关字符串大小写转化的函数。

1. ANSI_STRING字符串转化成大写

VOID RtlUpperString (
	IN OUT PSTRING DestinationString,
	IN PSTRING SourceString
);

	DestinationString: 目的字符串。
	SourceString: 源字符串。

2. UNICODE_STRING字符串转化成大写

NTSTATUS RtlUpcaseUnicodeString (
	IN OUT PUNICODE_STRING DestinationString OPTIONAL,
	IN PCUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: 目的字符串。
	SourceString: 源字符串。
	AllocateDestinationString: 是否为目的字符串分配内存。
	返回值: 返回转化是否成功。

	RtlUpcaseUnicodeString函数比RtlUpperString函数多一个
参数AllocateDestinationString。这个参数指定是否为目的字符串
申请内存。目的字符串和源字符串可以是同一个字符串。
	DDK虽然提供了转化成大写的函数，但却没有提供转化成小写的
函数。如下所示：

--*/

VOID Test11()
{
	KdPrint(("Test11————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2;
	// RtlInitUnicodeString(&UnicodeString2, L" ");

	// 变化前
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));

	// 转化成大写
	RtlUpcaseUnicodeString(&UnicodeString2, &UnicodeString1, TRUE);

	// 变化后
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	KdPrint(("Test11————finishing\n"));
}

/*++

字符串与整形数字相互转换

	DDK提供了UNICODE_STRING字符串与整形相互转换的内核函数。

1. 将UNICODE_STRING字符串转换成整形。

	这个函数是RtlUnicodeStringToInteger，其声明是：

NTSTATUS RtlUnicodeStringToInteger (
	IN PUNICODE_STRING String,
	IN ULONG Base OPTIONAL,
	OUT PULONG Value
);

	String: 需要转换的字符串
	Base: 转换的数的进制（如2，8，10，16）
	Value: 需要转换的数字
	返回值: 指明是否转换成功

2. 将整数转换成UNICODE_STRING字符串

	这个函数RtlIntegerToUnicodeString，其声明是：

NTSTATUS RtlIntegerToUnicodeString (
	IN ULONG Value,
	IN ULONG Base OPTIONAL,
	IN OUT PUNICODE_STRING String
);

	Value: 需要转换的数字
	Base: 转换的数的进制（如2，8，10，16）
	String: 需要转换的字符串
	返回值: 指明是否转换成功

	如下所示：

--*/

VOID Test12()
{
	KdPrint(("Test12————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"-100");

	ULONG lNumber;
	NTSTATUS nStatus = RtlUnicodeStringToInteger(
		&UnicodeString1, 10, &lNumber);
	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to integer successfully!\n"));
		KdPrint(("Result:%d\n", lNumber));
	}
	else
	{
		KdPrint(("Conver to integer unsuccessfully!\n"));
	}

	// 数字转换成字符串
	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;
	nStatus = RtlIntegerToUnicodeString(200, 10, &UnicodeString2);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to string successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver to string unsuccessfully\n"));
	}

	// 销毁UnicodeString2，注意UnicodeString1不用销毁
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test12————finishing\n"));
}

/*++

ANSI_STRING字符串与UNICODE_STRING字符串相互转换

	DDK提供了ANSI_STRING字符串与UNICODE_STRING字符串相互
转换的相关函数。
	
1. 将UNICODE_STRING字符串转换成ANSI_STRING字符串。

	DDK对于这种转换提供的函数是RtlUnicodeStringToAnsiString,
其声明是：

NTSTATUS RtlUnicodeStringToAnsiString (
	IN OUT PANSI_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: 需要被转换的字符串
	SourceString: 需要转换的源字符串
	AllocateDestinationString: 是否需要对被转换的字符串分
配内存。
	返回值: 指明是否转换成功。

2. 将ANSI_STRING字符串转化成UNICODE_STRING字符串

	DDK对于这种转换提供的函数是RtlAnsiStringToUnicodeString,
其声明是：
	
NTSTATUS RtlAnsiStringToUnicodeString (
	IN OUT PUNICODE_STRING DestinationString,
	IN PUNICODE_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
);

	DestinationString: 需要被转换的字符串
	SourceString: 需要转换的源字符串
	AllocateDestinationString: 是否需要对被转换的字符串分
配内存。
	返回值: 指明是否转换成功。

	如下所示：
	
--*/

VOID Test13()
{
	KdPrint(("Test13————beginning\n"));

	// 将UNICODE_STRING字符串转换成ANSI_STRING字符串
	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	ANSI_STRING AnsiString1;
	NTSTATUS nStatus = RtlUnicodeStringToAnsiString(
		&AnsiString1, &UnicodeString1, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%Z\n", &AnsiString1));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// 销毁AnsiString1
	RtlFreeAnsiString(&AnsiString1);

	// 将ANSI_STRING字符串转换成UNICODE_STRING字符串
	// 初始化AnsiString2
	ANSI_STRING AnsiString2;
	RtlInitString(&AnsiString2, "Hello world");
	
	UNICODE_STRING UnicodeString2;
	nStatus = RtlAnsiStringToUnicodeString(
		&UnicodeString2, &AnsiString2, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// 销毁UnicodeString2
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test13————finishing\n"));
}

/*++

内核模式下的文件操作

	在驱动程序开发中，经常会对文件进行操作。与Win32API不同，
DDK提供另外一套对文件的操作函数。

--*/

/*++

文件的创建

	对文件的创建或者打开都是通过内核函数ZwCreateFile实现的。
和Window API类似，这个内核函数返回一个文件句柄，文件的所有操
作都是依靠这个句柄进行操作的。在文件操作完毕后，需要关闭这个
句柄。

NTSTATUS ZwCreateFile (
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOption,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
);

	FileHandle: 返回打开文件的句柄。
	DesiredAccess: 对打开文件操作的描述，读，写或是其他。
一般指定为GENERIC_READ或GENERIC_WRITE。
	ObjectAttributes: 是OBJECT_ATTRIBUTES结构的地址，该
结构包含要打开i的文件名。
	IoStatusBlock: 指向一个IO_STATUS_BLOCK结构，该结构接收
ZwCreateFile操作的结果状态。
	AllocationSize: 是一个指针，指向一个64位整数，该数指定
文件初始分配时的大小。该参数仅关系到创建或重写文件操作，如果
忽略它，那么文件长度将从0开始，并随着写入而增长。
	FileAttributes: 0或者FILE_ATTRIBUTE_NORMAL，指定新
创建文件的属性。
	ShareAccess: FILE_SHARE_READ或0，指定文件的共享方式。
如果仅为读数据而打开文件，则可以与其它线程同时读取该文件。如果
为写数据而打开文件，可能不希望其他线程访问该文件。
	CreateDisposition: FILE_OPEN或FILE_OVERWRITE_IF，
表明当指定文件存在或不存在时应如何处理。
	CreateOption: FILE_SYNCHRONOUS_IO_NONALERT，指定控制
打开操作和句柄使用的附加标志位。
	EaBuffer: 一个指针，指向可选的扩展属性区。 
	EaLength: 扩展属性区的长度。

	这个函数需要填写CreateDisposition参数。如果想打开文件，
CreateDisposition参数设置为FILE_OPEN。如果想创建文件，
CreateDisposition参数设置成FILE_OVERWRITE_IF。此时，无论
文件是否存在，都会创建新文件。
	文件名的指定是通过设定第三个参数ObjectAttributes。这个
参数是一个OBJECT_ATTRIBUTES结构。DDK提供对OBJECT_ATTRIBUTES
结构初始化的宏InitializeObjectAttributes。

VOID InitializeObjectAttributes (
	OUT POBJECT_ATTRIBUTES InitializeAttributes,
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN HANDLE RootDirectory,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor
);

	InitializeAttributes: 返回的OBJECT_ATTRIBUTES结构。
	ObjectName: 对象名称，用UNICODE_STRING描述，这里设置
的是文件名。
	Attributes: 一般设为OBJ_CASE_INSENSITIVE，对大小写敏感。
	RootDirectory: 一般设为NULL。
	SecurityDescriptor: 一般设为NULL。

	另外，文件名必须是符号链接或者是设备名。

	如下所示：

--*/

VOID Test14()
{
	KdPrint(("Test14————beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// 可写成"\\Device\\HarddiskVolume1\\1.LOG"

	// 初始化objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	// 创建文件
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_WRITE,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!\n"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!\n"));
	}

	// 文件操作
	// do sometihings
	KdPrint(("do somethings\n"));

	// 关闭文件句柄
	ZwClose(hfile);

	KdPrint(("Test14————finishing\n"));
}



/*++

文件复制

--*/

VOID Test15()
{
	KdPrint(("Test15————beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// 或者写成"\\Device\\HarddiskVolume1\\1.LOG"

	// 初始化objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE, // 对大小写敏感
		NULL,
		NULL
	);

	// 创建文件
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_READ,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN, // 打开文件，如果不存在，则返回错误
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Open file successfully!\n"));
	}
	else
	{
		KdPrint(("Open file unsuccessfully!\n"));
	}

	// 文件操作
	KdPrint(("do somethings\n"));

	// 关闭文件句柄
	ZwClose(hfile);
	
	KdPrint(("Test15————finishing\n"));
}

/*++

文件的打开

	除了使用ZwCreateFile函数可以打开文件，DDK还提供了一个
内核函数ZwOpenFile。ZwOpenFile内核函数的参数比ZwCreateFile
的参数简化，方便程序员打开文件。该函数的声明如下：

NTSTATUS ZwOpenFile (
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_NLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
);

	FileHandle: 返回打开的文件句柄
	DesiredAccess: 打开的权限，一般设为GENERIC_ALL
	ObjectAttributes: objectAttributes结构
	IoStatusBlock: 指向一个结构体的指针。该结构体指明打开
文件的状态
	ShareAccess: 共享的权限。可以是FILE_SHARE_READ或者
FILE_SHARE_WRITE。
	OpenOption: 打开选项，一般设为FILE_SYNCHRONOUS_IO_NONALERT
	返回值: 指明文件是否被成打开

	代码如下所示：

--*/

VOID Test16()
{
	KdPrint(("Test16————beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&logFileUnicodeString, L"\\??\\C:\\1.log"
	);
	// 或者写成"\\Device\\HarddiskVolume1\\1.LOG"

	// 初始化objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	// 创建文件
	NTSTATUS ntStatus = ZwOpenFile(
		&hfile,
		GENERIC_ALL,
		&objectAttributes,
		&iostatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_SYNCHRONOUS_IO_NONALERT
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!"));
	}

	// 文件操作
	KdPrint(("do somethings"));

	// 关闭文件句柄
	ZwClose(hfile);

	KdPrint(("Test16————finishing\n"));
}

/*++

获取或修改文件属性

	获取和修改文件属性，包括获取文件大小、获取或修改文件指针
位置、获取或修改文件名、获取或修改文属性（只读属性、隐藏属性）
、获取或修改文件创建、修改日期等。DDK提供了内核函数ZwSetInformationFile
和ZwQueryInformation函数来进行获取和修改文件属性。

NTSTATUS ZwSetInformationFile (
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);

	FileHandle: 文件句柄
	IoStatusBlocks: 返回设置的状态
	FileInformation: 依据FileInformationClass不同而不同。
作为输入信息。
	Length: FileInformation数据的长度
	FileInformationClass: 描述修改属性的类型

NTSTATUS ZwQueryInformationFile (
	IN HANDLE FileHandle,
	OUT PIO_STATUS)BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass
);

	FileHandle: 文件句柄
	IoStatusBlock: 返回设置的状态
	FileInformation: 依据FileInformationClass不同而不同。
作为输入信息。
	Length: FileInformation数据的长度
	FileInformationClass: 描述修改属性的类型

	这两个函数基本相同。其中FileInformationClass指定修改或者
查询的类别。

1. 当FileInformationClass是FileStandardInformation时，
输入和输出的数据是FILE_STANDARD_INFORMATION结构体，描述文件
的基本信息。

typedef struct FILE_STANDARD_INFORMATION {
	LARGE_INTEGER AllocationSize; // 为文件分配的大小
	LARGE_INTEGER EndOfFile; // 距离文件结尾还有多少字节
	ULONG NumberOfLinks; // 有多少个链接文件
	BOOLEAN DeletePending; // 是否准备删除
	BOOLEAN Directory; // 是否为目录
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

2. 当FileInformationClass是FileBasicInformation时，输入
和输出的数据是FILE_BASIC_INFORMATION结构体，描述文件的基本
信息。

typedef struct FILE_BASIC_INFORMATION {
	LARGE_INTEGER CreationTime; // 文件创建时间
	LARGE_INTEGER LastAccessTime; // 最后访问时间
	LARGE_INTEGER LastWriteTime; // 最后写时间
	LARGE_INTEGER ChangeTime; // 修改修改时间
	ULONG FileAttributes; // 文件属性
} FILE_BASIC_INPORMATION, *PFILE_BASIC_INFORMATION;

	其中，时间参数是从一个LARGE_INTEGER的整数，该整数代表从
1601年经过多少100ns。
FileAttributes描述文件属性。
FILE_ATTRIBUTE_NORMAL描述一般文件。
FILE_ATTRIBUTE_DIRECTORY描述是目录。
FILE_ATTRIBUTE_READONLY描述该文件为只读。
FILE_ATTRIBUTE_HIDDEN代表隐含文件。
FILE_ATTRIBUTE_SYSTEM代表系统文件。

3. 当FileInformationClass是FileNameInformation时，输入
和输出的数据是FILE_NAME_INFORMATION结构体，描述文件名信息。

typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength; // 文件名长度
	WCHAR FileName[1]; // 文件名
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

4. 当FileInformationClass是FilePositionInformation时，输
入和输出的数据是FILE_POSITION_INFORMATION结构体，描述文件名
信息。

typedef struct FILE_POSITION_INFORMATION {
	LARGE_INTEGER CurrentByteOffset; // 代表当前文件指针位置
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

	代码如下所示：

--*/

VOID Test17()
{
	KdPrint(("Test17————beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// 或者写成"\\Device\\HarddiskVolume1\\1.LOG"

	// 初始化objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE, // 对大小写敏感
		NULL,
		NULL
	);

	// 创建文件
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_READ,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN, // 打开文件，如果不存则返回错误
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("open file successfully\n"));
	}

	FILE_STANDARD_INFORMATION fsi;

	// 读取文件长度
	ntStatus = ZwQueryInformationFile(
		hfile,
		&iostatus,
		&fsi,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("file length:%u\n", fsi.EndOfFile.QuadPart));
	}

	// 修改当前文件指针
	FILE_POSITION_INFORMATION fpi;
	fpi.CurrentByteOffset.QuadPart = 100i64;

	ntStatus = ZwSetInformationFile(
		hfile,
		&iostatus,
		&fpi,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("update the file pointer successfully!\n"));
	}

	// 关闭文件句柄
	ZwClose(hfile);

	KdPrint(("Test17————finishing\n"));
}

/*




*/