#include "MyDriver.h"

/*++

实验函数10————字符串的比较

--*/


/*++

字符串比较

DDK提供了对ANSI_STRING字符串和UNICODE_STRING字符串的
相关字符串比较的命令，分别是ANSI_STRING字符串比较函数和
UNICODE_STRING字符串比较函数。

LONG RtlCompareString (
IN PSTRING String1,
IN PSTRING String2,
BOOLEAN CaseInSensitive
);

String1: 要比较的第一个字符串
String2: 要比较的第二个字符串
CaseInSensitive: 是否对大小写敏感
返回值: 比较结果

LONG RtlCompareUnicodeString(
IN PUNICODE_STRING String1,
IN PUNICODE_STRING String2,
IN BOOLEAN CaseInSensitive
);

String1: 要比较的第一个字符串
String2: 要比较的第二个字符串
CaseInSensitive: 是否对大小写敏感
返回值: 比较结果

这两个函数的参数形式相同，如果函数返回值为0，则表示两个
字符串相等。如果小于0，则表示第一个字符串小于第二个字符串；
如果大于0，则表示第一个字符串大于第二个字符串。
同时，DDK又提供了RtlEqualString和RtlEqualUnicodeString
函数，其使用方法和上面两个函数类似。只是返回为非零代表相等，零
代表不相等。

--*/

VOID Test10()
{
	KdPrint(("Test10————beginning\n"));

	// 初始化UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// 初始化UnicodeString2
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString2, L"Hello");

	// 判断字符串是否相等
	if (RtlEqualUnicodeString(&UnicodeString1, &UnicodeString2, TRUE))
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are equal\n"));
	}
	else
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are NOT equal"));
	}

	KdPrint(("Test10————finishing\n"));
}