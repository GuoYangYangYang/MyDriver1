#include "MyDriver.h"

/*++

实验函数22————添加、修改注册表键值

--*/

/*++

添加、修改注册表键值

	打开注册表的句柄后，就可以对该项进行设置和修改了。
注册表是以二元形式存储的，即“键名”和“键值”。通过键名
设置键值，而键值可以划分几个类，如下所示：

REG_BINARY    键值用二进制存储
REG_SZ        键值用宽字符串，字符串以\0的结尾
REG_EXPAND_SZ 键值用宽字符串，字符串以\0的结尾，该字符串是扩展的字符
REG_MULTI_SZ  键值存储多个字符串，每个字符串以\0隔开
REG_DWORD     键值用4字节整型存储
REG_QWORD     键值用8字节存储

	在添加和修改注册表键值的时候，要分类进行添加和修改
。DDK提供了ZwSetValueKey函数来完成这个任务，其函数声明
是：

NTSTATUS ZwSetValueKey (
	IN HANDLE KeyHandle,



--*/