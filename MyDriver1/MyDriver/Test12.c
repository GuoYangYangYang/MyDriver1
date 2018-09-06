#include "MyDriver.h"

/*++

实验函数12————字符串与数字的转换

--*/

/*++

字符串与整形数字相互转换

DDK提供了UNICODE_STRING字符串与整形相互转换的内核函数。

1. 将UNICODE_STRING字符串转换成整形。

这个函数是RtlUnicodeStringToInteger，其声明是：

NTSTATUS RtlUnicodeStringToInteger (
IN PUNICODE_STRING String,
IN ULONG Base OPTIONAL,
OUT PULONG Value
);

String: 需要转换的字符串
Base: 转换的数的进制（如2，8，10，16）
Value: 需要转换的数字
返回值: 指明是否转换成功

2. 将整数转换成UNICODE_STRING字符串

这个函数RtlIntegerToUnicodeString，其声明是：

NTSTATUS RtlIntegerToUnicodeString (
IN ULONG Value,
IN ULONG Base OPTIONAL,
IN OUT PUNICODE_STRING String
);

Value: 需要转换的数字
Base: 转换的数的进制（如2，8，10，16）
String: 需要转换的字符串
返回值: 指明是否转换成功

如下所示：

--*/

VOID Test12()
{
	KdPrint(("Test12————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"-100");

	ULONG lNumber;
	NTSTATUS nStatus = RtlUnicodeStringToInteger(
		&UnicodeString1, 10, &lNumber);
	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to integer successfully!\n"));
		KdPrint(("Result:%d\n", lNumber));
	}
	else
	{
		KdPrint(("Conver to integer unsuccessfully!\n"));
	}

	// 数字转换成字符串
	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;
	nStatus = RtlIntegerToUnicodeString(200, 10, &UnicodeString2);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to string successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver to string unsuccessfully\n"));
	}

	// 销毁UnicodeString2，注意UnicodeString1不用销毁
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test12————finishing\n"));
}
