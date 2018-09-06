#include "MyDriver.h"

/*++

实验函数18————文件的写操作

--*/

/*

文件的写操作

DDK提供了文件写操作的内核函数，其函数声明如下：

NTSTATUS ZwWriteFile (
IN HANDLE FileHandle,
IN HANDLE Event OPTIONAL,
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
IN PVOID ApcContext OPTIONAL,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PVOID Buffer,
IN ULONG Length,
IN PLARGE_INTEGER ByteOffset OPTIONAL,
IN PULONG Key OPTIONAL
);

FileHandle: 文件打开的句柄。
Event: 很少用到，一般设置为NULL。
ApcRoutine: 很少用到，一般设置为NULL。
ApcContext: 很少用到，一般设置为NULL。
IoStatusBlock: 记录写操作的状态。其中，IoStatusBlock.
Information记录实际写了多少字节。
Buffer: 从这个缓冲区开始往文件里写。
Length: 准备写多少字节。
BytesOffset: 从文件的多少偏移地址开始写。
Key: 很少用到，一般设置为NULL。

代码如下所示：

*/

VOID Test18()
{
	KdPrint(("Test18————beginning\n"));

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
		GENERIC_WRITE,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF, // 即使存在该文件也创建
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);

	// 构造要填充的数据
	RtlFillMemory(pBuffer, BUFFER_SIZE, 0xAA);

	KdPrint(("The program will write %d bytes\n", BUFFER_SIZE));

	// 写文件
	ZwWriteFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		BUFFER_SIZE,
		NULL,
		NULL
	);

	KdPrint(("The program really wrote %d bytes\n", iostatus.Information));

	// 构造要填充的数据
	RtlFillMemory(pBuffer, BUFFER_SIZE, 0xBB);

	KdPrint(("The program will append %d bytes\n", BUFFER_SIZE));

	// 追加数据
	LARGE_INTEGER number;
	number.QuadPart = 1024i64; // 设置文件指针

							   // 对文件进行附加写
	ZwWriteFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		BUFFER_SIZE,
		&number,
		NULL
	);

	KdPrint(("The program really appended %d bytes\n", iostatus.Information));

	// 关闭文件句柄
	ZwClose(hfile);

	ExFreePool(pBuffer);

	KdPrint(("Test18————finishing\n"));
}
