#include "MyDriver.h"

/*++

实验函数03————Lookaside

--*/


/*++

使用Lookaside

如果驱动程序需要频繁地从内存中申请、回收固定大小的内存，
则应该使用Lookaside对象。
Lookaside是一个自动的内存分配容器。通过对Lookaside对象
申请内存，效率要高于直接向Windows申请内存。一般在如下使用：
1. 程序员每次申请固定大小的内存。
2. 申请和回收的操作十分频繁。
使用Lookaside对象首先要初始化Lookaside对象。有如下函数：
VOID ExInitializeNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PEREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
和
VOID ExInitializePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PFREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
这两个函数分别是对非分页和分页Lookaside对象进行初始化。

在初始化完Lookaside对象后，可以进行申请内存的操作了：
PVOID ExAllocateFromNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
和
PVOID ExAllocateFromPagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
这两个函数分别是对非分页内存和分页内存的申请。

对Lookaside对象回收内存的操作，有以下两个函数：
VOID ExFreeToNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
和
VOID ExFreeToPagedLookaside (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
这两个函数分别是对非分页内存和分页内存的回收。

在使用完Lookaside对象后，需要删除Lookaside对象，有以下
两个函数：
VOID ExDeleteNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
和
VOID ExDeletePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
这两个函数分别是对非分页和分页Lookaside对象的删除。

--*/

VOID Test03()
{
	KdPrint(("Test03————beginning\n"));

	// 初始化Lookaside对象
	PAGED_LOOKASIDE_LIST pageList;
	ExInitializePagedLookasideList(&pageList, NULL,
		NULL, 0, sizeof(MYDATASTRUCT), '1234', 0);
	INT ARRAY_NUMBER = 50;
	PMYDATASTRUCT MyObjectArray[50];

	// 模拟频繁申请内存
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		MyObjectArray[i] = (PMYDATASTRUCT)
			ExAllocateFromPagedLookasideList(&pageList);
	}

	// 模拟频繁回收内存
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		ExFreeToPagedLookasideList(&pageList,
			MyObjectArray[i]);
		MyObjectArray[i] = NULL;
	}

	// 删除Lookaside对象
	ExDeletePagedLookasideList(&pageList);

	KdPrint(("Test03————finishing\n"));
}

