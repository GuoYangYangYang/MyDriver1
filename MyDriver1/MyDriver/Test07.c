#include "MyDriver.h"

/*++

实验函数07————字符串的初始化

--*/

/*++

内核下的字符串操作

和应用程序一样，驱动程序需要经常和字符串打交道。其中包括
ASCII字符串、宽字符串、还有DDK定义的ANSI_STRING数据结构和
UNICODE_STRING数据结构。

--*/

/*++

ASCII字符串和宽字符串

在应用程序中，往往使用两种字符：一种是char型的字符串，
负责记录ANSI字符集，它是指向一个char数组的指针，每个char型
变量的大小为一个字节，字符串是以0标志字符串结束。还有一种是
wchar_t数组的指针，wchar_t字符大小为两个字节，字符串以0标志
字符串结束。
ANSI字符的构造如下：
char* str1 = "abc";
str1指针指向的内容是61 626 300
UNICODE字符的构造如下：
wchar_t *str2 = L"abc";
str2指针指向的内容是6100 620 063 000 000。在构造字符串
的时候使用一个关键字“L"。编译器会自动生成所需要的宽字符。
在驱动程序开发中，DDK将char和wchar_t类别，替换成CHAR和
WCHAR类别。
对于这两类的字符串，DDK提供相应的字符串操作函数，例如，
strpy、sprintf、strcat、strlen等。但微软公司不鼓励直接使用
这些函数，取而代之的是使用同样功能的宏。

--*/

/*++

ANSI_STRING字符串与UNICODE_STRING字符串

DDK不鼓励程序员使用C语言的字符串，主要是因为：标准C的字
符串处理函数容易导致缓冲区溢出等错误。如果程序员不对字符串的
长度进行检验，很容易导致这个错误，从而导致整个操作系统的崩溃。
DDK鼓励程序员使用DDK自定义的字符串，这种数据格式的定义如下：

typedef struct _STRING {
USHORT Length;
USHORT MaximumLength;
PCHAR Buffer;
} STRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

这个数据结构对ASCII字符串进行了封锁。
Length: 字符的长度。
MaximumLength: 整个字符串缓冲区的最大长度。
Buffer: 缓冲区的指针。

和标准的字符串不同，STRING字符串不是以0标志字符结束。字符
长度依靠Length字段。在标准C中的字符串中，如果缓冲区长度是N，
那么只能容纳N-1个字符的字符串，这里因为要留一个字节存储NULL。
而在STRING字符串中，缓冲区的大小MaximumLength，最大的字符串
长度可以是MaximumLength，而不是MaximumLength-1。
与ANSI_STRING相对应，DDK将宽字符串封装成UNICODE_STRING
数据结构。

typedef struct _UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} UNICODE_STRING;

Length: 字符的长度。单位是字节。如果是N个字符，那么Length
等于N的2倍。
MaximumLength: 整个字符串缓冲区的最大长度，单位也是字节。
Buffer: 缓冲区的指针。

和ANSI_STRING不同，UNICODE_STRING的缓冲区是记录宽字符
的缓冲区。每个元素是宽字符。和ANSI_STRING一样，字符串的结束
不是以NULL为标志，而是依靠字段Length。

ANSI_STRING字符串和UNICODE_STRING字符串，KdPrint同样
提供了打印log的方法。

ANSI_STRING ansiString;
// 省去初始化
KdPrint("%Z\n", &ansiString); // 注意是%Z

而当打印一段宽字符的时候，需要进行以下操作。

UNICODE_STRING uniString;
// 省去初始化
KdPrint("%wZ\n", &uniString); // 注意是%wZ

--*/

/*++

字符初始化和销毁

ANSI_STRING字符串和UNICODE_STRING字符串使用前需要进行
初始化。有两种办法构造这个数据结构。

1. 方法一就是使用DDK提供了相应的函数

初始化ANSI_STRING字符串

VOID RtlInitAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PCSZ SourceString
);

DestinationString: 要初始化的ANSI_STRING字符串。
SourceString: 字符串的内容。

初始化UNICODE_STRING字符串

VOID RtlInitUnicodeString (
IN OUT PUNCODE_STRING DestinationString,
IN PCWSTR SourceString
);

DestinationString: 要初始化的UNICODE_STRING字符串。
SourceString: 字符串的内容。

以RtlInitAnsiString为例，其使用方法是：

ANSI_STRING AnsiString1;
CHAR* string1 = "hello";
RtlInitAnsiString(&AnsiString, string1);

--*/


VOID Test07()
{
	KdPrint(("Test07————beginning\n"));

	ANSI_STRING AnsiString1;

	CHAR * string1 = "hello";
	// 初始化ANSI_STRING字符串
	RtlInitAnsiString(&AnsiString1, string1);
	KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // 打印hello

												 // 改变string1，此处会蓝屏，不能使用
												 //string1[0] = 'H';
												 //string1[1] = 'E';
												 //string1[2] = 'L';
												 //string1[3] = 'L';
												 //string1[4] = 'O';
												 // 改变string1，AnsiString同样会导致变化，此处会蓝屏。
												 // KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // 打印HELLO
	KdPrint(("Test07————finishing\n"));
}
