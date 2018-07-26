#include <ntifs.h> 

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
VOID DriverUnload(PDRIVER_OBJECT objDriver);

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
