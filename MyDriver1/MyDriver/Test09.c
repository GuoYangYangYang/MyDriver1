#include "MyDriver.h"

/*++

实验函数09————字符串的复制

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

VOID Test09()
{
	KdPrint(("Test09————beginning\n"));

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

	KdPrint(("Test09————finishing\n"));
}