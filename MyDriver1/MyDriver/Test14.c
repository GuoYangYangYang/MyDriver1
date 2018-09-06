#include "MyDriver.h"

/*++

实验函数14————文件的创建

--*/

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
