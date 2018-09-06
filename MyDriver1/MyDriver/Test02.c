#include "MyDriver.h"

/*++

实验函数2————链表

--*/


/*++

分配内核内存

Windows驱动程序使用的内存资源十分珍贵，分配内存时要尽量
节约。和应用程序一样，局部变量是存放在栈（Stack）空间中的。
但栈空间不会像应用程序那么大，所以驱动程序不适合递归调用或者
局部变量是大型结构体。如果需要大型结构体，则需要在堆（Heap）
中申请。
堆中申请内存的函数有以下几个，原型如下：

PVOID ExAllocatePool (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

PVOID ExAllocatePoolWithQuota (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes
);

PVOID ExAllocatePoolWithQuotaTag (
IN POOL_TYPE PoolType,
IN SIZE_T NumberOfBytes,
IN ULONG Tag
);

其中PoolType是个枚举变量，如果此值为NonPagedPool，则分
配非分页内存。如果此值为PagedPool，则分配内存为分页内存。
NumberOfBytes是分配内存的大小，最好为4的倍数。
返回值分配的内存地址，一定是内核模式地址。如果返回0，则
代表分配失败。

--*/

/*++

链表结构

DDK提供了标准的双向链表。双向链表可以将链表形成一个环。BLINK
指针指向前一个元素，FLINK指针指向下一个元素。
以下是DDK提供的双向链表的数据结构。

typedef struct _LIST_ENTRY {
struct _LIST_ENTRY *Flink;
struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

--*/

/*++

链表初始化

每个双向链表都是以一个链表头作为链表的第一个元素。初次使用
链表头需要进行初始化，主要将链表头的Flink和Blink两个指针都指向
自己。这意味着链表头所代表的连是空链，初始化链表头用InitializeListHead
宏实现。
如何判断链表是否为空，可以判断链表头的Flink和Blink是否指
向自己。DDK为程序员提供了一个宏简化这种检查（IsLIstEmpty）
程序员需要自己定义链表中每个元素的数据类型，并将LIST_ENTRY
结构作为自定义结构的一个子域。LIST_ENTRY的作用是将自定义的
数据结构串成一个链表。

typedef struct _MYDATASTRUCT {
// List Entry需要作为_MYDATASTRUCT结构体的一部分
LIST_ENTRY ListEntry;
// 下面是自定义的数据
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;

--*/

/*++

从首部插入链表

对链表的插入有两种方式，一种是在链表的头部插入，一种是在
链表的尾部插入。
在头部插入链表使用语句InsertHeadList，用法如下：
InsertHeadList(&head, &mydata->ListEntry);
其中，head是LIST_ENTRY结构的链表头，mydata是用户定义的
数据结构，而它的子域ListEntry是包含其中的LIST_ENTRY数据结构

--*/

/*++

从尾部插入链表

另外一种插入方法是在链表的尾部进行插入。在尾部插入链表使
用InsertTailList，用法如下：
InsertTailList(&head, &mydata->ListEntry);
其中，head是LIST_ENTRY结构的链表头，mydata是用户定义的
数据结构，而它的子域ListEntry是包含其中的LIST_ENTRY数据结构

--*/

/*++

从链表删除

和插入链表一样，删除也有两种对应的方法。一种是从链表头部
删除，一种是从链表尾部删除。分别对应RemoveHeadList和
RemoveTailList函数，其使用方法如下：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
及
PLIST_ENTRY pEntry = RemoveTailList(&head);
其中，head是链表头，pEntry是从链表删除下来的元素中的
ListEntry。这里有两种情况：
1. 当自定一的数据结构的第一个字段是LIST_ENTRY时，如：
typedef struct _MYDATASTRUCT {
LIST_ENTRY ListEntry;
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;
此时，RemoveHeadList返回的指针可以被当作用户自定义的
指针，即：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PMYDATASTRUCT pMyData = (PMYDATASTRUCT) pEntry;
2. 当自定义的数据结构的第一个字段不是LIST_ENTRY时，即：
typedef struct _MYDATASTRUCT {
ULONG x;
ULONG y;
LIST_ENTRY ListEntry;
} MYDATASTRUCT, *PMYDATASTRUCT;
此时，RemoveHeadList返回的指针不可以当作用户自定义的
指针，此时需要通过pEntry的地址逆向算出自定义数据的指针。一般
通过pEntry在自定义数据中的偏移量，用pEntry减去这个偏移量，就
会得到用户自定义结构的指针的地址。DDK中提供了宏
CONTEAINING_RECORD，其用法如下：
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PIRP pIrp = CONTAINING_RECORD(pEntry,
MYDATASTRUCT,
ListEntry);
DDK建议自定义数据结构无论第一个字段是否为ListEntry，最
好都使用CONTAINING_RECORD宏得到自定义数据结构的指针。

--*/

VOID Test02()
{
	KdPrint(("Test02————beginning\n"));
	LIST_ENTRY linkListHead;
	// 初始化链表
	InitializeListHead(&linkListHead);

	PMYDATASTRUCT pData;
	ULONG i = 0;
	// 在链表中插入10个元素
	KdPrint(("Begin insert to link list"));
	for (i = 0; i < 10; i++)
	{
		// 分配分页内存
		pData = (PMYDATASTRUCT)
			ExAllocatePool(PagedPool, sizeof(MYDATASTRUCT));
		pData->number = i;
		// 从头部插入链表
		InsertHeadList(&linkListHead, &pData->ListEntry);
	}

	// 从链表中取出，并显示
	KdPrint(("Begin remove from link list\n"));
	while (!IsListEmpty(&linkListHead))
	{
		// 从尾部删除一个元素
		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry,
			MYDATASTRUCT,
			ListEntry);
		// KdPrint(("%d\n", pData->number));
		ExFreePool(pData);
	}
	KdPrint(("Test02————finishing\n"));
}

