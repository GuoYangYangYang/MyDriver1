#pragma once
#include <ntifs.h> 
#define BUFFER_SIZE 1024

// �û��Զ�������ṹ
typedef struct _MYDATASTRUCT {
	// List Entry��Ҫ��Ϊ_MYDATASTRUCT�ṹ���һ����
	LIST_ENTRY ListEntry;
	// �������Զ��������
	ULONG x;
	ULONG y;
	ULONG number;
} MYDATASTRUCT, *PMYDATASTRUCT;

/*++

�豸��չ

--*/
typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice; // ͨ��pDeviceָ���豸����
	UNICODE_STRING ustrDeviceName; // �豸����
	UNICODE_STRING ustrSymLinkName; // ����������
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/*++

���У�
1.  pDevice:
�豸�����е�DeviceExtensionָ���豸��չ��pDevice����
ָ���豸����
2.  ustrDeviceName:
�豸����
3.  ustrSymLinkName:
�豸�ķ�����������

���豸��չ�У���¼���ϼ�����Ϣ���Ա������ص�����������ǲ
����ʹ�á�ʹ�õ�ʱ��ֻ��������豸�л�ȡ�����������´��룺
pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

--*/

NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject);

VOID DisplayItsProcessName();
VOID Test01();     // ʵ�麯��01�������������ڴ�
VOID Test02();     // ʵ�麯��02������������
VOID Test03();     // ʵ�麯��03��������Lookaside
VOID Test04();     // ʵ�麯��04������������ʱ����
VOID Test05();     // ʵ�麯��05��������try-except
NTSTATUS Test06(); // ʵ�麯��06��������try-finally
VOID Test07();     // ʵ�麯��07���������ַ����ĳ�ʼ��
VOID Test08();     // ʵ�麯��08���������ַ����ĳ�ʼ��
VOID Test09();     // ʵ�麯��09���������ַ����ĸ���
VOID Test10();     // ʵ�麯��10���������ַ����ıȽ�
VOID Test11();     // ʵ�麯��11���������ַ����Ĵ�д
VOID Test12();     // ʵ�麯��12���������ַ��������ֵ�ת��
VOID Test13();     // ʵ�麯��13�������������ַ���֮���ת��
VOID Test14();     // ʵ�麯��14���������ļ��Ĵ���
VOID Test15();     // ʵ�麯��15���������ļ��Ĵ�
VOID Test16();     // ʵ�麯��16���������ļ��Ĵ�2
VOID Test17();     // ʵ�麯��17���������ļ��Ĳ�ѯ���޸��ļ�����
VOID Test18();     // ʵ�麯��18���������ļ���д����
VOID Test19();     // ʵ�麯��19���������ļ��Ķ�����
VOID Test20();     // ʵ�麯��20�������������ر�ע���

PDRIVER_DISPATCH MyDriverDispatchCreate(); // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchClose();  // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchWrite();  // ��ǲ����
PDRIVER_DISPATCH MyDriverDispatchRead();   // ��ǲ����
NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
);
