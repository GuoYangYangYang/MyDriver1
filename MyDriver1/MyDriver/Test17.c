#include "MyDriver.h"

/*++

实验函数17————文件的查询、修改文件属性

--*/

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
