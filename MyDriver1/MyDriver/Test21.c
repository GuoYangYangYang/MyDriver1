#include "MyDriver.h"

/*++

实验函数21————打开注册表

--*/

/*++

打开注册表

	ZwCreateKey函数既可以创建注册表项，也可以打开
注册表项。为了简化打开注册表项，DDK提供了内核函数
ZwOpenKey，以简化打开操作。如果ZwOpenKey指定的项
不存在，不会创建这个项目，而是返回一个错误状态。该函数
声明如下：

NTSTATUS ZwOpenKey (
	OUT PHANDLE KeyHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

	KeyHandle: 返回被打开的句柄。
	DesiredAccess: 打开的权限，一般设为KEY_ALL_ACCESS。
	ObjectAttributes: OBJECT_ATTRIBUTES数据结构，
指示打开的状态。
	返回值: 返回是否打开成功。

	ZwOpenKey的参数比ZwCreateKey的参数简化，程序员
使用该函数会很方便。代码如下：

--*/

VOID Test21()
{
	KdPrint(("Test21————beginning\n"));

	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&RegUnicodeString,
		MY_REG_SOFTWARE_KEY_NAME
	);

	OBJECT_ATTRIBUTES objectAttributes;
	// 初始化objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&RegUnicodeString,
		OBJ_CASE_INSENSITIVE, // 对大小写敏感
		NULL,
		NULL
	);

	// 打开注册表
	NTSTATUS ntStatus = ZwOpenKey(
		&hRegister,
		KEY_ALL_ACCESS,
		&objectAttributes
	);

	// 判断操作是否成功
	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Open register successfully\n"));
	}
	
	// 关闭句柄
	ZwClose(hRegister);

	KdPrint(("Test21————finishing\n"));
}