#include "MyDriver.h"

/*++

ʵ�麯��19���������ļ��Ķ�����

--*/

/*++

�ļ��Ķ�����

DDK�ṩ���ļ����������ں˺������亯���������£�

NTSTATUS ZwReadFile (
IN HANDLE FileHandle,
IN HANDLE Event OPTIONAL,
IN PIO_APC_BOUTINE ApcPoutine OPTIONAL,
IN PVOID ApcContext OPTIONAL,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PVOID Buffer,
IN ULONG Length,
IN PLAYER_INTEGER ByteOffset OPTIONAL,
IN PULONG Key OPTIONAL
);

FileHandle: �ļ��򿪵ľ����
Event: �����õ���һ������ΪNULL��
ApcRoutine: �����õ���һ������ΪNULL��
ApcContext: �����õ���һ������ΪNULL��
IoStatusBlock: ��¼д������״̬�����У�IoStatusBlock.
Information��¼ʵ��д�˶����ֽڡ�
Buffer: �������������ʼ���ļ������
Length: ׼��д�����ֽڡ�
ByteOffset: ���ļ��Ķ���ƫ�Ƶ�ַ��ʼд��
Key: �����õ���һ������ΪNULL��

����Ƕ�ȡ�����ļ�����Ҫ֪���ļ��Ĵ�С���ù��ܿ���ͨ���ں�
����ZwQueryInformationFile��ʵ�֡�

����������ʾ��

--*/

VOID Test19()
{
	KdPrint(("Test19��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// ����д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE, // �Դ�Сд����
		NULL,
		NULL
	);

	// �����ļ�
	NTSTATUS ntStatus = ZwCreateFile(
		&hfile,
		GENERIC_READ,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN, // ��ʹ���ڸ��ļ���Ҳ����
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (!NT_SUCCESS(ntStatus))
	{
		KdPrint(("The file is not exist!\n"));
		return;
	}

	FILE_STANDARD_INFORMATION fsi;
	// ��ȡ�ļ�����
	ntStatus = ZwQueryInformationFile(
		hfile,
		&iostatus,
		&fsi,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);

	KdPrint(("The program want to read %d bytes\n",
		fsi.EndOfFile.QuadPart));

	// Ϊ��ȡ���ļ����仺����
	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(
		PagedPool, (LONG)fsi.EndOfFile.QuadPart
	);

	// ��ȡ�ļ�
	ZwReadFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		(LONG)fsi.EndOfFile.QuadPart,
		NULL,
		NULL
	);

	KdPrint(("The program really read %d bytes\n",
		iostatus.Information));

	// �ر��ļ����
	ZwClose(hfile);

	// �ͷŻ�����
	ExFreePool(pBuffer);

	KdPrint(("Test19��������finishing\n"));
}