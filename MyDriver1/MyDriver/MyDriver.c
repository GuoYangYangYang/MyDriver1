#include <ntifs.h> 
#include "MyDriver.h"

// �ڱ���DDK�ṩ������ʱ������ָ��ĳ�����̺�ĳ��ȫ�ֱ�����
// �����ҳ�ڴ滹�ǷǷ�ҳ�ڴ棬��Ҫ�����¶��壺
#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath);
VOID DriverUnload(IN PDRIVER_OBJECT objDriver);

/*++

����NTʽ������˵����Ҫ�ĺ�����DriverEntry���̡�ж�����̼�����
IRP����ǲ���̡�

--*/

/*++

�������ع�����������ں�����DriverEntry����

�ͱ�д��ͨӦ�ó���һ�������������и���ں�����Ҳ��������
��ִ�еĺ������������ͨ��������ΪDriverEntry��������һ����
DriverEntry��Ҫ�Ƕ�����������г�ʼ��������������ϵͳ
���������õġ���Windows���и�����Ľ��̽���ϵͳ���̣���
���̹������������и���ΪSystem�Ľ��̾���ϵͳ���̡�ϵͳ������
ϵͳ������ʱ�򣬾��Ѿ��������ˡ�
�������ص�ʱ��ϵͳ���������µ��̣߳�����ִ��������е�
���������������һ�����������������������һ��DRIVER_OBJECT
�Ľṹ�塣���⣬ϵͳ���̵���ִ��������е����ù�����򣬲�ѯ��
���������Ӧ��ע����е��
ϵͳ�̵߳������������DriverEntry����ʱ��ͬʱ����������
�����ֱ���pDriverObject��pRegistryPath������һ����ָ��ղ�
���������������ָ�룬����һ����ָ���豸������ļ����ַ�����
ָ�롣��DriverEntry�У���Ҫ�����Ƕ�ϵͳ���̴��������������
�г�ʼ�������⣬�豸������ļ�����ʱ����Ҫ�����£���Ϊ�����
�������ǳ��ڴ��ڵģ��������غ������ʧ��������Ժ���ʹ�����
UNICODE�ַ����ͱ����Ȱ������Ƶ���ȫ�ĵط���
����ַ���������һ����\REGISTRY\MACHINE\SYSTEM\ControlSet
\Services\[������]�������������У��ַ�����UNICODE�ַ�����
��ʾ��UNICODE�ǿ��ַ�����ÿ���ַ���16λ��ʾ��

UNICODE�����ݽṹUNICODE_STRING��ʾ��

typedef struct _LSA_UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

���У�
1.  Length:
��¼����ַ����ö����ֽڼ�¼������ַ�����N���ַ���
��ôLength������N��2����
2.  MaximumLength:
��¼buffer�Ĵ�С��Ҳ��������ṹ����ܼ�¼���ֽ�����
MaximumLengthҪ���ڻ����Length��
3.  Buffer:
��¼�ַ�����ָ�롣��ASCII�ַ�����ͬ��������ַ���
ÿ���ַ�����16λ��

--*/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pRegistryPath);

	// NTSTATUS status;

	// ��ӡһ���ַ���
	KdPrint(("Enter DriverEntry\n"));

	// ע�������������ú������
	pDriverObject->DriverUnload = DriverUnload;

	DisplayItsProcessName();

	// ʵ�麯��1�������������ڴ�
	Test01();

	// ʵ�麯��2������������
	Test02();

	// ʵ�麯��3��������Lookaside
	Test03();

	// ʵ�麯��4������������ʱ����
	Test04();

	// ʵ�麯��5��������try-except
	Test05();

	// ʵ�麯��6��������try-finally
	Test06();

	// ʵ�麯��7���������ַ����ĳ�ʼ��
	Test07();

	// ʵ�麯��8���������ַ����ĳ�ʼ��
	Test08();

	// ʵ�麯��9���������ַ����ĸ���
	Test09();

	// ʵ�麯��10���������ַ����ıȽ�
	Test10();

	// ʵ�麯��11���������ַ����Ĵ�д
	Test11();

	// ʵ�麯��12���������ַ��������ֵ�ת��
	Test12();

	// ʵ�麯��13�������������ַ���֮���ת��
	Test13();

	// ʵ�麯��14���������ļ��Ĵ���
	Test14();

	// ʵ�麯��15���������ļ��Ĵ�
	Test15();

	// ʵ�麯��16���������ļ��Ĵ�2
	Test16();

	// ʵ�麯��17���������ļ��Ĳ�ѯ���޸��ļ�����
	Test17();

	// ʵ�麯��18���������ļ���д����
	Test18();

	// ʵ�麯��19���������ļ��Ķ�����
	Test19();

	// ʵ�麯��20�������������ر�ע���
	Test20();

	// ������ǲ����
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MyDriverDispatchRoutin;
	// pDriverObject->MajorFunction[IRP_MJ_CLOSE];
	// pDriverObject->MajorFunction[IRP_MJ_WRITE];
	// pDriverObject->MajorFunction[IRP_MJ_READ];

	// ���������豸����
	// status = CreateDevice(pDriverObject);

	KdPrint(("DriverEntry end\n"));

	/*++
	DriverEntry�ķ���ֵ��NTSTATUS�����ݣ�NTSTATUS�Ǳ�
	����Ϊ32λ���޷��ų����͡����������򿪷��У�����ϰ����
	NTSTATUS����״̬������0~0X7FFFFFFF������Ϊ����ȷ��״̬��
	��0X80000000~0XFFFFFFFF������Ϊ�Ǵ����״̬�������и��ǳ�
	���õĺ�NT_SUCCESS�����������״̬�Ƿ���ȷ��
	������NTSTATUSֵ�У�
	#define STATUS_SUCCESS           ((NTSTATUS)0x00000000L)
	#define STATUS_BUFFER_OVERFLOW   ((NTSTATUS)0x80000005L)
	#define STATUS_UNSUCCESSFUL      ((NTSTATUS)0xC0000001L)
	#define STATUS_NOT_IMPLEMENTED   ((NTSTATUS)0xC0000002L)
	#define STATUS_ACCESS_VIOLATION  ((NTSTATUS)0xC0000005L)
	#define STATUS_INVALID_HANDLE    ((NTSTATUS)0xC0000008L)
	#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
	DriverEntry�ķ���ֵ�����ʾ�ɹ�������ζ�ż���������
	����������ζ�ż�������ʧ�ܣ����ö���������������������
	--*/
	return STATUS_SUCCESS;
}
/*++

�����Ҫ˵������DriverEntry����������"In"��"In","OUT",
"INOUT"��DDK�ж�������Ϊ�մ������ǵĹ��������ڳ���ע�ͣ�
������һ��"In"����ʱ��Ӧ���϶��ò����Ǵ�����������Ŀ�ġ�
"OUT"��������������������ں��������������"INOUT"���ڼȿ�
�������ֿ�������Ĳ���������DriverEntry���̣�����
DriverObjectָ����IN��������ʹ���߲��ܸı����ָ�뱾��
����ȫ���Ըı���ָ��Ķ���
��DriverEntry�����У�һ������ж����������IRP����ǲ������
���⻹��һ���ִ��븺�𴴽��豸��������ж�����̺�������ǲ����
���Ƕ�������������á��豸�����е�MajorFunction��һ������ָ
�����飬IRP_MJ_CREATE��IRP_MJ_CLOSE��IRP_MJ_WRITE��������
�ĵڼ���Ԫ�ء�

--*/


/*++

DriverUnload����

�����������л�����DriverUnload���̣���������������ж��
��ʱ����á���NTʽ�����У�DriverUnloadһ�㸺��ɾ����DriverEntry
�д������豸���󣬲��ҽ��豸�����������ķ�������ɾ�������⣬
DriverUnload�������һЩ��Դ���л��ա�

--*/

VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pDriverObject);

	PDEVICE_OBJECT pNextObj;

	// ʲôҲ������ֻ��ӡһ���ַ���
	KdPrint(("Enter DriverUnload\n"));

	// �õ���һ���豸����
	pNextObj = pDriverObject->DeviceObject;

	// ö�������豸����
	while (pNextObj != NULL)
	{
		// �õ��豸��չ
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		// ɾ����������
		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;

		// ɾ���豸
		IoDeleteDevice(pDevExt->pDevice);
	}

	// ʲôҲ������ֻ��ӡһ���ַ���
	KdPrint(("Finish DriverUnload\n"));
}

/*++

��DriverUnload�У��������������󡣸����������󣬾Ϳ��Ա���
�����ɸ��������󴴽����豸����ͨ�����������DeviceObject��
���Ϳ����ҵ������豸���������ϴ����У�����������ʵֻ������
һ���豸������ˣ���DriverUnload�ĵ����У�Ҳֻ��ɾ��һ���豸
����
ɾ���豸����ĺ�����IoDeleteDevice������ԭ���ǣ�

VOID IoDeleteDevice(IN PDEVICE_OBJECT DeviceObject);

���������Ҫ��ɾ�����豸����ָ�롣��DriverUnload�У�����
Ҫɾ���豸����ͬʱ��Ҫ���豸��������ķ������ӽ���ɾ����ɾ��
�������ӵĺ�����IoDeleteSymbolicLink������ԭ���ǣ�

NTSTATUS IoDeleteSymbolicLink(
IN PUNICODE_STRING SymbolibLinkName
);

���У�
1.  SymbolicLinkName:
��ʾ�Ѿ���ע���˵ķ������ӡ�
2.  ����ֵ��
��ʾɾ�����������Ƿ�ɹ���

--*/