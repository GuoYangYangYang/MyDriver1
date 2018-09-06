#include "MyDriver.h"

/*++

实验函数19————文件的读操作

--*/

/*++

文件的读操作

DDK提供了文件读操作的内核函数，其函数声明如下：

NTSTATUS ZwReadFile (
IN HANDLE FileHandle,
IN HANDLE Event OPTIONAL,
IN PIO_APC_BOUTINE ApcPoutine OPTIONAL,
IN PVOID ApcContext OPTIONAL,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PVOID Buffer,
IN ULONG Length,
IN PLAYER_INTEGER ByteOffset OPTIONAL,
IN PULONG Key OPTIONAL
);

FileHandle: 文件打开的句柄。
Event: 很少用到，一般设置为NULL。
ApcRoutine: 很少用到，一般设置为NULL。
ApcContext: 很少用到，一般设置为NULL。
IoStatusBlock: 记录写操作的状态，其中，IoStatusBlock.
Information记录实际写了多少字节。
Buffer: 从这个缓冲区开始从文件里读。
Length: 准备写多少字节。
ByteOffset: 从文件的多少偏移地址开始写。
Key: 很少用到，一般设置为NULL。

如果是读取整个文件，需要知道文件的大小，该功能可以通过内核
函数ZwQueryInformationFile来实现。

代码如下所示：

--*/

VOID Test19()
{
	KdPrint(("Test19————beginning\n"));

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
		FILE_OPEN, // 即使存在该文件，也创建
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (!NT_SUCCESS(ntStatus))
	{
		KdPrint(("The file is not exist!\n"));
		return;
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

	KdPrint(("The program want to read %d bytes\n",
		fsi.EndOfFile.QuadPart));

	// 为读取的文件分配缓冲区
	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(
		PagedPool, (LONG)fsi.EndOfFile.QuadPart
	);

	// 读取文件
	ZwReadFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		(LONG)fsi.EndOfFile.QuadPart,
		NULL,
		NULL
	);

	KdPrint(("The program really read %d bytes\n",
		iostatus.Information));

	// 关闭文件句柄
	ZwClose(hfile);

	// 释放缓冲区
	ExFreePool(pBuffer);

	KdPrint(("Test19————finishing\n"));
}