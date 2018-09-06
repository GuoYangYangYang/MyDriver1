#include "MyDriver.h"

/*++

ʵ�麯��18���������ļ���д����

--*/

/*

�ļ���д����

DDK�ṩ���ļ�д�������ں˺������亯���������£�

NTSTATUS ZwWriteFile (
IN HANDLE FileHandle,
IN HANDLE Event OPTIONAL,
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
IN PVOID ApcContext OPTIONAL,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PVOID Buffer,
IN ULONG Length,
IN PLARGE_INTEGER ByteOffset OPTIONAL,
IN PULONG Key OPTIONAL
);

FileHandle: �ļ��򿪵ľ����
Event: �����õ���һ������ΪNULL��
ApcRoutine: �����õ���һ������ΪNULL��
ApcContext: �����õ���һ������ΪNULL��
IoStatusBlock: ��¼д������״̬�����У�IoStatusBlock.
Information��¼ʵ��д�˶����ֽڡ�
Buffer: �������������ʼ���ļ���д��
Length: ׼��д�����ֽڡ�
BytesOffset: ���ļ��Ķ���ƫ�Ƶ�ַ��ʼд��
Key: �����õ���һ������ΪNULL��

����������ʾ��

*/

VOID Test18()
{
	KdPrint(("Test18��������beginning\n"));

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
		GENERIC_WRITE,
		&objectAttributes,
		&iostatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_WRITE,
		FILE_OPEN_IF, // ��ʹ���ڸ��ļ�Ҳ����
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);

	// ����Ҫ��������
	RtlFillMemory(pBuffer, BUFFER_SIZE, 0xAA);

	KdPrint(("The program will write %d bytes\n", BUFFER_SIZE));

	// д�ļ�
	ZwWriteFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		BUFFER_SIZE,
		NULL,
		NULL
	);

	KdPrint(("The program really wrote %d bytes\n", iostatus.Information));

	// ����Ҫ��������
	RtlFillMemory(pBuffer, BUFFER_SIZE, 0xBB);

	KdPrint(("The program will append %d bytes\n", BUFFER_SIZE));

	// ׷������
	LARGE_INTEGER number;
	number.QuadPart = 1024i64; // �����ļ�ָ��

							   // ���ļ����и���д
	ZwWriteFile(
		hfile,
		NULL,
		NULL,
		NULL,
		&iostatus,
		pBuffer,
		BUFFER_SIZE,
		&number,
		NULL
	);

	KdPrint(("The program really appended %d bytes\n", iostatus.Information));

	// �ر��ļ����
	ZwClose(hfile);

	ExFreePool(pBuffer);

	KdPrint(("Test18��������finishing\n"));
}
