#include "MyDriver.h"

/*++

实验函数08————字符串的初始化

--*/

/*++

另外一种方法是程序员自己申请内存。并初始化内存，当不用字
符串时，需要回收字符串占用的内存。

--*/

VOID Test08()
{
	KdPrint(("Test08————beginning\n"));
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

	KdPrint(("Test08————finishing\n"));
}

/*++

对于最后一步清理内存，DDK同样给出了简化函数，分别是
RtlFreeAnsiString和RtlFreeUnicodeString。这两个函数内部
调用了ExFreePool去回收内存。

--*/