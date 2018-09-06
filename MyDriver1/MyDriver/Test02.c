#include "MyDriver.h"

/*++

ʵ�麯��2������������

--*/


/*++

�����ں��ڴ�

Windows��������ʹ�õ��ڴ���Դʮ����󣬷����ڴ�ʱҪ����
��Լ����Ӧ�ó���һ�����ֲ������Ǵ����ջ��Stack���ռ��еġ�
��ջ�ռ䲻����Ӧ�ó�����ô���������������ʺϵݹ���û���
�ֲ������Ǵ��ͽṹ�塣�����Ҫ���ͽṹ�壬����Ҫ�ڶѣ�Heap��
�����롣
���������ڴ�ĺ��������¼�����ԭ�����£�

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

����PoolType�Ǹ�ö�ٱ����������ֵΪNonPagedPool�����
��Ƿ�ҳ�ڴ档�����ֵΪPagedPool��������ڴ�Ϊ��ҳ�ڴ档
NumberOfBytes�Ƿ����ڴ�Ĵ�С�����Ϊ4�ı�����
����ֵ������ڴ��ַ��һ�����ں�ģʽ��ַ���������0����
�������ʧ�ܡ�

--*/

/*++

����ṹ

DDK�ṩ�˱�׼��˫������˫��������Խ������γ�һ������BLINK
ָ��ָ��ǰһ��Ԫ�أ�FLINKָ��ָ����һ��Ԫ�ء�
������DDK�ṩ��˫����������ݽṹ��

typedef struct _LIST_ENTRY {
struct _LIST_ENTRY *Flink;
struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

--*/

/*++

�����ʼ��

ÿ��˫����������һ������ͷ��Ϊ����ĵ�һ��Ԫ�ء�����ʹ��
����ͷ��Ҫ���г�ʼ������Ҫ������ͷ��Flink��Blink����ָ�붼ָ��
�Լ�������ζ������ͷ����������ǿ�������ʼ������ͷ��InitializeListHead
��ʵ�֡�
����ж������Ƿ�Ϊ�գ������ж�����ͷ��Flink��Blink�Ƿ�ָ
���Լ���DDKΪ����Ա�ṩ��һ��������ּ�飨IsLIstEmpty��
����Ա��Ҫ�Լ�����������ÿ��Ԫ�ص��������ͣ�����LIST_ENTRY
�ṹ��Ϊ�Զ���ṹ��һ������LIST_ENTRY�������ǽ��Զ����
���ݽṹ����һ������

typedef struct _MYDATASTRUCT {
// List Entry��Ҫ��Ϊ_MYDATASTRUCT�ṹ���һ����
LIST_ENTRY ListEntry;
// �������Զ��������
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;

--*/

/*++

���ײ���������

������Ĳ��������ַ�ʽ��һ�����������ͷ�����룬һ������
�����β�����롣
��ͷ����������ʹ�����InsertHeadList���÷����£�
InsertHeadList(&head, &mydata->ListEntry);
���У�head��LIST_ENTRY�ṹ������ͷ��mydata���û������
���ݽṹ������������ListEntry�ǰ������е�LIST_ENTRY���ݽṹ

--*/

/*++

��β����������

����һ�ֲ��뷽�����������β�����в��롣��β����������ʹ
��InsertTailList���÷����£�
InsertTailList(&head, &mydata->ListEntry);
���У�head��LIST_ENTRY�ṹ������ͷ��mydata���û������
���ݽṹ������������ListEntry�ǰ������е�LIST_ENTRY���ݽṹ

--*/

/*++

������ɾ��

�Ͳ�������һ����ɾ��Ҳ�����ֶ�Ӧ�ķ�����һ���Ǵ�����ͷ��
ɾ����һ���Ǵ�����β��ɾ�����ֱ��ӦRemoveHeadList��
RemoveTailList��������ʹ�÷������£�
PLIST_ENTRY pEntry = RemoveHeadList(&head);
��
PLIST_ENTRY pEntry = RemoveTailList(&head);
���У�head������ͷ��pEntry�Ǵ�����ɾ��������Ԫ���е�
ListEntry�����������������
1. ���Զ�һ�����ݽṹ�ĵ�һ���ֶ���LIST_ENTRYʱ���磺
typedef struct _MYDATASTRUCT {
LIST_ENTRY ListEntry;
ULONG x;
ULONG y;
} MYDATASTRUCT, *PMYDATASTRUCT;
��ʱ��RemoveHeadList���ص�ָ����Ա������û��Զ����
ָ�룬����
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PMYDATASTRUCT pMyData = (PMYDATASTRUCT) pEntry;
2. ���Զ�������ݽṹ�ĵ�һ���ֶβ���LIST_ENTRYʱ������
typedef struct _MYDATASTRUCT {
ULONG x;
ULONG y;
LIST_ENTRY ListEntry;
} MYDATASTRUCT, *PMYDATASTRUCT;
��ʱ��RemoveHeadList���ص�ָ�벻���Ե����û��Զ����
ָ�룬��ʱ��Ҫͨ��pEntry�ĵ�ַ��������Զ������ݵ�ָ�롣һ��
ͨ��pEntry���Զ��������е�ƫ��������pEntry��ȥ���ƫ��������
��õ��û��Զ���ṹ��ָ��ĵ�ַ��DDK���ṩ�˺�
CONTEAINING_RECORD�����÷����£�
PLIST_ENTRY pEntry = RemoveHeadList(&head);
PIRP pIrp = CONTAINING_RECORD(pEntry,
MYDATASTRUCT,
ListEntry);
DDK�����Զ������ݽṹ���۵�һ���ֶ��Ƿ�ΪListEntry����
�ö�ʹ��CONTAINING_RECORD��õ��Զ������ݽṹ��ָ�롣

--*/

VOID Test02()
{
	KdPrint(("Test02��������beginning\n"));
	LIST_ENTRY linkListHead;
	// ��ʼ������
	InitializeListHead(&linkListHead);

	PMYDATASTRUCT pData;
	ULONG i = 0;
	// �������в���10��Ԫ��
	KdPrint(("Begin insert to link list"));
	for (i = 0; i < 10; i++)
	{
		// �����ҳ�ڴ�
		pData = (PMYDATASTRUCT)
			ExAllocatePool(PagedPool, sizeof(MYDATASTRUCT));
		pData->number = i;
		// ��ͷ����������
		InsertHeadList(&linkListHead, &pData->ListEntry);
	}

	// ��������ȡ��������ʾ
	KdPrint(("Begin remove from link list\n"));
	while (!IsListEmpty(&linkListHead))
	{
		// ��β��ɾ��һ��Ԫ��
		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
		pData = CONTAINING_RECORD(pEntry,
			MYDATASTRUCT,
			ListEntry);
		// KdPrint(("%d\n", pData->number));
		ExFreePool(pData);
	}
	KdPrint(("Test02��������finishing\n"));
}

