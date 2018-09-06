#include "MyDriver.h"

/*++

实验函数13————两种字符串之间的转换

--*/

/*++

ANSI_STRING字符串与UNICODE_STRING字符串相互转换

DDK提供了ANSI_STRING字符串与UNICODE_STRING字符串相互
转换的相关函数。

1. 将UNICODE_STRING字符串转换成ANSI_STRING字符串。

DDK对于这种转换提供的函数是RtlUnicodeStringToAnsiString,
其声明是：

NTSTATUS RtlUnicodeStringToAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PUNICODE_STRING SourceString,
IN BOOLEAN AllocateDestinationString
);

DestinationString: 需要被转换的字符串
SourceString: 需要转换的源字符串
AllocateDestinationString: 是否需要对被转换的字符串分
配内存。
返回值: 指明是否转换成功。

2. 将ANSI_STRING字符串转化成UNICODE_STRING字符串

DDK对于这种转换提供的函数是RtlAnsiStringToUnicodeString,
其声明是：

NTSTATUS RtlAnsiStringToUnicodeString (
IN OUT PUNICODE_STRING DestinationString,
IN PUNICODE_STRING SourceString,
IN BOOLEAN AllocateDestinationString
);

DestinationString: 需要被转换的字符串
SourceString: 需要转换的源字符串
AllocateDestinationString: 是否需要对被转换的字符串分
配内存。
返回值: 指明是否转换成功。

如下所示：

--*/

VOID Test13()
{
	KdPrint(("Test13————beginning\n"));

	// 将UNICODE_STRING字符串转换成ANSI_STRING字符串
	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	ANSI_STRING AnsiString1;
	NTSTATUS nStatus = RtlUnicodeStringToAnsiString(
		&AnsiString1, &UnicodeString1, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%Z\n", &AnsiString1));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// 销毁AnsiString1
	RtlFreeAnsiString(&AnsiString1);

	// 将ANSI_STRING字符串转换成UNICODE_STRING字符串
	// 初始化AnsiString2
	ANSI_STRING AnsiString2;
	RtlInitString(&AnsiString2, "Hello world");

	UNICODE_STRING UnicodeString2;
	nStatus = RtlAnsiStringToUnicodeString(
		&UnicodeString2, &AnsiString2, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// 销毁UnicodeString2
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test13————finishing\n"));
}

