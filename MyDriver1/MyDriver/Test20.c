#include "MyDriver.h"

/*++ 

实验函数20————创建关闭注册表

--*/

/*++

内核模式下的注册表操作

在驱动程序的开发中，经常会用到对注册表的操作。DDK提供了
另外一套对注册表操作的相关函数。

注册表中的概念：

注册表项: 注册表中的一个项目，类似目录的概念。每个项中
存储多个二元结构，键名——键值。每个项中，可以有若干个子项。
注册表子项: 类似于目录中的子目录。
键名: 通过键名可以寻找到相应的键值。
键值类别: 每个键值存储的时候有不同的类别，可以是整形，
字符串等数据。
键值: 键名下对应存储的数据。

--*/

/*++

创建关闭注册表

和文件操作类似，对注册表操作首先要获取一个注册表句柄。对
注册表的操作都需要根据这个句柄进行操作。可以通过ZwCreateKey
函数获得打开的注册表句柄。这个函数打开注册表后，并返回一个操
作句柄。其函数声明如下:

NTSTATUS ZwCreateKey (
OUT PHANDLE KeyHandle,
IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttriburtes,
IN ULONG TitleIndex,
IN PUNICODE_STRING Class OPTIONAL,
IN ULONG CreateOptions,
OUT PULONG Disposition OPTIONAL
);

KeyHandle: 获得的注册表句柄。
DesiredAccess: 访问权限，一般设置为KEY_ALL_ACCESS。
ObjectAttributes: OBJECT_ATTRIBUTES数据结构。
TitleIndex: 很少用到，一般设置为0。
Class: 很少用到，一般设置为NULL。
CreateOptions: 创建时的选项，一般设置为REG_OPTION_NON_VOLATILE。
Disposition: 返回是创建成功，还是打开成功。返回值是
REG_CREATED_NEW_KEY或者是REG_OPENED_EXISTING_KEY。
返回值: 返回是否创建成功。

如果ZwCreateKey指定的项目不存在，则直接创建这个项目，并
利用Disposition参数返回REG_CREATED_NEW_KEY。如果该项目已经
存在了，Disposition参数返回REG_OPENED_EXISTING_KEY。代码
如下所示：

--*/

VOID Test20()
{
	KdPrint(("Test20————beginning\n"));

	// 创建或打开某注册表项目
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

	ULONG ulResult;
	// 创建或打开注册表项目
	NTSTATUS ntStatus = ZwCreateKey(
		&hRegister,
		KEY_ALL_ACCESS,
		&objectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&ulResult
	);

	if (NT_SUCCESS(ntStatus))
	{
		// 判断是被新创建，还是已经被创建
		if (ulResult == REG_CREATED_NEW_KEY)
		{
			KdPrint(("The register item is created\n"));
		}
		else if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The register item has been created, and now is opened\n"));
		}
	}

	// 创建或打开某注册表项目的子项
	UNICODE_STRING subRegUnicodeString;
	HANDLE hSubRegister;

	// 初始化UNICODE_STRING字符串
	RtlInitUnicodeString(
		&subRegUnicodeString,
		L"SubItem"
	);

	OBJECT_ATTRIBUTES subObjectAttributes;
	// 初始化subObjectAttributes
	InitializeObjectAttributes(
		&subObjectAttributes,
		&subRegUnicodeString,
		OBJ_CASE_INSENSITIVE, // 对大小写敏感
		hRegister,
		NULL
	);

	// 创建或打开注册表项目
	ntStatus = ZwCreateKey(
		&hSubRegister,
		KEY_ALL_ACCESS,
		&subObjectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&ulResult
	);

	if (NT_SUCCESS(ntStatus))
	{
		// 判断是被新创建，还是已经被创建
		if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The sub register item is created\n"));
		}
		else if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The sub register item has been created, and now is opened\n"));
		}
	}

	// 关闭注册表句柄
	ZwClose(hRegister);
	ZwClose(hSubRegister);

	KdPrint(("Test20————finishing\n"));
}