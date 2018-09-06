#include "MyDriver.h"

/*++

ʵ�麯��1�������������ڴ�

--*/

/*++

��ҳ��Ƿ�ҳ�ڴ�

Windows�涨��Щ�����ڴ�ҳ���ǿ��Խ������ļ��У������ڴ�
����Ϊ��ҳ�ڴ档����Щ�����ڴ�ҳ��Զ���ύ�����ļ��У���Щ��
�汻��Ϊ�Ƿ�ҳ�ڴ档
��������ж�������DISPATCH_LEVEL֮��ʱ������DISPATCH_LEVEL
�㣩������ֻ��ʹ�÷Ƿ�ҳ�ڴ棬���򽫵�������������


--*/

/*--

�ڱ���DDK�ṩ������ʱ������ָ��ĳ�����̺�ĳ��ȫ�ֱ�����
�����ҳ�ڴ滹�ǷǷ�ҳ�ڴ棬��Ҫ�����¶��壺

#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

--*/

/*++

�����ĳ���������뵽��ҳ�ڴ��У�������Ҫ�ں�����ʵ����
�������´��룺

#pragma PAGEDCODE
VOID SomeFunction1()
{
PAGED_CODE();
// do somethings
}

���У�PAGED_CODE()��DDK�ṩ�ĺ꣬��ֻ��check�汾����Ч��
���������������Ƿ����е���DISPATCH_LEVEL���ж����󼶣����
���ڻ��������ж����󼶣�������һ�����ԡ�

--*/

#pragma PAGEDCODE
VOID SomeFunction1()
{
	PAGED_CODE();
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

/*++

�����ĳ���������뵽�Ƿ�ҳ�ڴ��У�������Ҫ�ں�����ʵ���м�
�����´��룺

#pragma LOCKEDCODE
VOID SomeFunction2()
{
// do somethings
}

--*/

#pragma LOCKEDCODE
VOID SomeFunction2()
{
	KdPrint(("IN PAGEDCODE\n"));
	// do somethings
}

VOID Test01()
{
	KdPrint(("Test01��������beginning\n"));
	SomeFunction1();
	SomeFunction2();
	KdPrint(("Test01��������finishing\n"));
}

/*++

����һ���������������ĳ��������Ҫ�ڳ�ʼ����ʱ�������ڴ棬
Ȼ��Ϳ��Դ��ڴ���ж�ص����������ָ��������DriverEntry���
�£�������NTʽ��������DriverEntry��ܳ���ռ�ݺܴ�Ŀռ䣬Ϊ
�˽�ʡ�ڴ棬��Ҫ��ʱ�ش��ڴ���ж�ص����������£�

#pragma INITCODE
extern "C" NTSTATUS DriverEntry(
IN PDRIVER_OBJECT pDriverObject,
IN PUNICODE_STRING RegistryPath)
{
// do somethings
}

--*/