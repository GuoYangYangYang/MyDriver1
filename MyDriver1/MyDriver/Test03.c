#include "MyDriver.h"

/*++

ʵ�麯��03��������Lookaside

--*/


/*++

ʹ��Lookaside

�������������ҪƵ���ش��ڴ������롢���չ̶���С���ڴ棬
��Ӧ��ʹ��Lookaside����
Lookaside��һ���Զ����ڴ����������ͨ����Lookaside����
�����ڴ棬Ч��Ҫ����ֱ����Windows�����ڴ档һ��������ʹ�ã�
1. ����Աÿ������̶���С���ڴ档
2. ����ͻ��յĲ���ʮ��Ƶ����
ʹ��Lookaside��������Ҫ��ʼ��Lookaside���������º�����
VOID ExInitializeNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PEREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
��
VOID ExInitializePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PALLOCATE_FUNCTION Allocate OPTIONAL,
IN PFREE_FUNCTION Free OPTIONAL,
IN ULONG Flags,
IN SIZE_T Size,
IN ULONG Tag,
IN USHORT Depth
);
�����������ֱ��ǶԷǷ�ҳ�ͷ�ҳLookaside������г�ʼ����

�ڳ�ʼ����Lookaside����󣬿��Խ��������ڴ�Ĳ����ˣ�
PVOID ExAllocateFromNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
��
PVOID ExAllocateFromPagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
�����������ֱ��ǶԷǷ�ҳ�ڴ�ͷ�ҳ�ڴ�����롣

��Lookaside��������ڴ�Ĳ���������������������
VOID ExFreeToNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
��
VOID ExFreeToPagedLookaside (
IN PPAGED_LOOKASIDE_LIST Lookaside,
IN PVOID Entry
);
�����������ֱ��ǶԷǷ�ҳ�ڴ�ͷ�ҳ�ڴ�Ļ��ա�

��ʹ����Lookaside�������Ҫɾ��Lookaside����������
����������
VOID ExDeleteNPagedLookasideList (
IN PNPAGED_LOOKASIDE_LIST Lookaside
);
��
VOID ExDeletePagedLookasideList (
IN PPAGED_LOOKASIDE_LIST Lookaside
);
�����������ֱ��ǶԷǷ�ҳ�ͷ�ҳLookaside�����ɾ����

--*/

VOID Test03()
{
	KdPrint(("Test03��������beginning\n"));

	// ��ʼ��Lookaside����
	PAGED_LOOKASIDE_LIST pageList;
	ExInitializePagedLookasideList(&pageList, NULL,
		NULL, 0, sizeof(MYDATASTRUCT), '1234', 0);
	INT ARRAY_NUMBER = 50;
	PMYDATASTRUCT MyObjectArray[50];

	// ģ��Ƶ�������ڴ�
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		MyObjectArray[i] = (PMYDATASTRUCT)
			ExAllocateFromPagedLookasideList(&pageList);
	}

	// ģ��Ƶ�������ڴ�
	for (int i = 0; i < ARRAY_NUMBER; i++)
	{
		ExFreeToPagedLookasideList(&pageList,
			MyObjectArray[i]);
		MyObjectArray[i] = NULL;
	}

	// ɾ��Lookaside����
	ExDeletePagedLookasideList(&pageList);

	KdPrint(("Test03��������finishing\n"));
}

