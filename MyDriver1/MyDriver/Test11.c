#include "MyDriver.h"

/*++

实验函数11————字符串的大写

--*/


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