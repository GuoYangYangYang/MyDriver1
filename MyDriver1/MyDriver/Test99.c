#include "MyDriver.h"

/*



*/

/*++

��ǲ����

��ǲ������Windows���������е���Ҫ��������������Ҫ��
���Ǹ�����I/O�������д󲿷�I/O����������ǲ�����д���ġ�
�û�ģʽ�����ж����������I/O����ȫ���ɲ���ϵͳת��Ϊ
һ������IRP�����ݽṹ����ͬ��IRP���ݻᱻ����ǲ������ͬ����ǲ
������Dispatch Function���С�

--*/

/*++

IRP����ǲ����

IRP�Ĵ����������WindowsӦ�ó����еġ���Ϣ�������ƣ�����
������ܵ���ͬ���͵�IRP�󣬻���벻ͬ����ǲ����������ǲ������
IRP�õ�����

--*/

/*++

IRP

��Windows�ں��У���һ�����ݽṹ����IRP��I/O Request
Package�����������������������������������ص���Ҫ���ݽṹ��
�ϲ�Ӧ�ó�����ײ���������ͨ��ʱ��Ӧ�ó���ᷢ��I/O���󡣲���
ϵͳ��I/O����ת��Ϊ��Ӧ��IRP���ݣ���ͬ���͵�IRP��������ʹ���
����ͬ����ǲ�����ڡ�
IRP�����������������ԣ�һ����MajorFunction����һ����
MinorFunction���ֱ��¼IRP����Ҫ���ͺ������͡�����ϵͳ����
MajorFunction��IRP����ǲ������ͬ����ǲ�����У�����ǲ�����л�
���Լ����ж����IRP��������MinorFunction��
NTʽ���������WDM������������DriverEntry������ע��
��ǲ�����ġ�

--*/

/*++

PDRIVER_DISPATCH MyDriverDispatchCreate()
{

}

PDRIVER_DISPATCH MyDriverDispatchClose()
{

}

PDRIVER_DISPATCH MyDriverDispatchWrite()
{

}

PDRIVER_DISPATCH MyDriverDispatchRead()
{

}

--*/

/*++

��DriverEntry����������pDriverObject�У��и�����ָ��
����MajorFunctio������ָ�������Ǹ����飬ÿ��Ԫ�ض���¼��һ
�������ĵ�ַ��ͨ������������飬���Խ�IRP�����ͺ���ǲ������
���������˴�ֻ���������͵�IRP��������ǲ��������IRP�����Ͳ�
��ֻ�������֡���������û�����õ�IRP���ͣ�ϵͳĬ����ЩIRP����
��_IopInvalidDeviceRequest����������
�ڽ���DriverEntry֮ǰ������ϵͳ�Ὣ_IopInvaildDeviceRequest
�ĵ�ַ��������MajorFunction���顣

--*/

/*++

IRP����

IRP�ĸ�������WindowsӦ�ó����С���Ϣ���ĸ����Win32���
�У��������ɡ���Ϣ�������ġ���ͬ����Ϣ���ᱻ�ַ�����ͬ����Ϣ����
�����С����û�ж�Ӧ�Ĵ�������������뵽ϵͳĬ�ϵ���Ϣ����
�����С�
IRP�Ĵ����������ַ�ʽ���ļ�I/O����غ�������CreateFile��
ReadFile��WriteFile��CloseHandle�Ⱥ�����ʹ����ϵͳ������
IRP_MJ_CREATE��IRP_MJ_READ��IRP_MJ_WRITE��IRP_MJ_CLOSE
�Ȳ�ͬ��IRP����ЩIRP�ᱻ���͵������������ǲ�����С�
���⣬�ں��е��ļ�I/O����������ZwCreateFile��ZwReadFile��
ZwWriteFile��ZwClose������ͬ���ᴴ����Ӧ��IRP_MJ_CREATE��
IRP_MJ_READ��IRP_MJ_WRITE��IRP_MJ_CLOSE�Ȳ�ͬ��IRP������
IRP���͵���Ӧ��������Ӧ��ǲ�����С�
����ЩIRP����ϵͳ��ĳ����������ģ�����IRP_MJ_SHUTDOWN
����Windows�ļ��弴������ڼ����ر�ϵͳ��ʱ�򷢳��ġ�

--*/

/*++

����ǲ�����ļ򵥴���

�󲿷ֵ�IRP��Դ���ļ�I/O����Win32API����CreateFile��
ReadFile�ȡ�������ЩIRP�򵥵ķ�����������Ӧ��ǲ�����У���IRP
��״̬����Ϊ�ɹ���Ȼ�����IRP�����󣬲�����ǲ�������سɹ�����
��IRP������ʹ�ú���IoCompleteRequest������Ĵ�����ʾ��һ��
��򵥵Ĵ���IRP�������ǲ������

--*/

NTSTATUS MyDriverDispatchRoutin(
	IN PDEVICE_OBJECT pDevObj,
	IN PIRP pIrp
)
{
	// �������������δ���ò����ľ���
	UNREFERENCED_PARAMETER(pDevObj);
	KdPrint(("Enter MyDriverDispatchRoutin\n"));
	// ��һ��IRP�ļ򵥲���
	NTSTATUS status = STATUS_SUCCESS;
	// ����IRP���״̬
	pIrp->IoStatus.Status = status;
	// ����IRP�����˶����ֽ�
	pIrp->IoStatus.Information = 0; // btyes xfered
									// ����IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("Leave MyDriverDispatchRoutin\n"));
	return status;
}

/*++

��ǲ����������IRP�����״̬ΪSTATUS_SUCCESS������������
I/O�����Win32 API����WriteFile�����᷵��TRUE���෴�������
IRP�����״̬����Ϊ���ɹ�����ʱ����I/O�����Win32 API����
WriteFile�����᷵��FALSE���������ʱ������ʹ��GetLastError
Win32 API�õ�������롣���õĴ��������IRP���õ�״̬��һ�¡�
��������IRP�����״̬����ǲ������Ҫ�������IRP���������
�����ֽڡ��ڱ����У��������ֽ����򵥵�����Ϊ��0�������ReadFile
������IRP������ֽ���������豸д�˶����ֽڡ������ǲ������
IRP�������������ͨ��IoCompleteRequest������ɵġ�
IoCompleteRequest���������£�

VOID IoCompleteRequest (
IN PIRP Irp,
IN CCHAR PriorityBoost
);

Irp: ������Ҫ��������IRP��
PriorityBoost: �����ָ̻߳�ʱ�����ȼ���

��ReadFileΪ����ReadFile���ڲ�����������������:
1. ReadFile����ntdll�е�NtReadFile������ReadFile����
��Win32 API����NtReadFile������Native API��
2. ntdll�е�NtReadFile���뵽�ں�ģʽ��������ϵͳ������
��NtReadFile������
3. ϵͳ������NtReadFile����IRP_MJ_WRITE���͵�IRP��
Ȼ���������IRP���͵�ĳ�������������ǲ�����С�NtReadFile
Ȼ��ȥ����һ���¼�����ʱ��ǰ�߳̽��롰˯�ߡ�״̬��Ҳ����˵��ǰ
�̱߳�����ס�����̴߳��ڡ�Pending��״̬��
4. ����ǲ������һ��ὫIRP�������������IRP��ͨ��
IoCompleteRequest�����ڲ������øղŵȴ����¼������¼������߳�
���ָ����С�
���磬�ڶ�һ���ܴ���ļ��������豸��ʱ��ReadFile��������
���أ����ǵȴ�һ��ʱ�䡣���ʱ����ǵ�ǰ�̡߳�˯�ߡ����Ƕ�ʱ�䡣
IRP���󱻽�������־�������ִ����ϣ���ʱ��˯�ߡ����̱߳����ѡ�
IoCompleteRequest�����еĵڶ�������PriorityBoost����
һ�����ȼ���ָ���Ǳ��������߳��Ժ������ȼ��ָ����С�һ�������
���ȼ�����ΪIO_NO_INCREMENT����ĳЩ�����������Ҫ��������
�߳��ԡ����ȡ�����ݻָ����С�����̡����������豸��������Ҫ
����ط�Ӧ��

--*/

/*++

ͨ���豸������豸

Ҫ���豸������ͨ���豸�����ֲ��ܵõ����豸�ľ����ÿ��
�豸�����豸���ƣ���MyDriver1����������豸��Ϊ��\Device\MyDriver1Device����
�����豸���޷����û�ģʽ�µ�Ӧ�ó����ѯ�����豸��ֻ�ܱ��ں�
ģʽ�µĳ����ѯ����
��Ӧ�ó����У��豸����ͨ������������з��ʡ���������ͨ��
IoCreateSymbolicLink����������������MyDriver1���������
�豸����Ӧ�ķ�������\??\MyDriver1�����ڱ�д����ʱ����������
��д���ͱ���ˡ�\\.\MyDriver1����д��C���Ե��ַ�������
��\\\\.\\MyDriver1����
����Ĵ�����ʾ���������CreateFile�����豸������Լ�
�������CloseHandle�ر��豸������ڴ򿪺͹ر��豸�����ʱ��
����ϵͳ�ڲ��ᴴ��IRP������IRP���͵���Ӧ����ǲ�����С�
���ְ취�ǽ�AnsiString1�е�Bufferָ�����string1ָ�롣
���ֳ�ʼ�����ŵ��ǲ����򵥣�������������ڴ档����������һ��
���⣬����޸�string1��ͬʱ�ᵼ��AnsiString1�ַ������仯��

--*/

