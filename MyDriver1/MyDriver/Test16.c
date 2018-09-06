#include "MyDriver.h"

/*++

ʵ�麯��16���������ļ��Ĵ�2

--*/

/*++

�ļ��Ĵ�

����ʹ��ZwCreateFile�������Դ��ļ���DDK���ṩ��һ��
�ں˺���ZwOpenFile��ZwOpenFile�ں˺����Ĳ�����ZwCreateFile
�Ĳ����򻯣��������Ա���ļ����ú������������£�

NTSTATUS ZwOpenFile (
OUT PHANDLE FileHandle,
IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttributes,
OUT PIO_STATUS_NLOCK IoStatusBlock,
IN ULONG ShareAccess,
IN ULONG OpenOptions
);

FileHandle: ���ش򿪵��ļ����
DesiredAccess: �򿪵�Ȩ�ޣ�һ����ΪGENERIC_ALL
ObjectAttributes: objectAttributes�ṹ
IoStatusBlock: ָ��һ���ṹ���ָ�롣�ýṹ��ָ����
�ļ���״̬
ShareAccess: �����Ȩ�ޡ�������FILE_SHARE_READ����
FILE_SHARE_WRITE��
OpenOption: ��ѡ�һ����ΪFILE_SYNCHRONOUS_IO_NONALERT
����ֵ: ָ���ļ��Ƿ񱻳ɴ�

����������ʾ��

--*/

VOID Test16()
{
	KdPrint(("Test16��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString, L"\\??\\C:\\1.log"
	);
	// ����д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwOpenFile(
		&hfile,
		GENERIC_ALL,
		&objectAttributes,
		&iostatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_SYNCHRONOUS_IO_NONALERT
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!"));
	}

	// �ļ�����
	KdPrint(("do somethings"));

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test16��������finishing\n"));
}
