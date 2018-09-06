#include "MyDriver.h"

/*++

实验函数16————文件的打开2

--*/

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
