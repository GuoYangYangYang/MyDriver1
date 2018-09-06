#include "MyDriver.h"

/*++

实验函数15————文件的打开

--*/

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
