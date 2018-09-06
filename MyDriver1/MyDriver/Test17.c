#include "MyDriver.h"

/*++

ʵ�麯��17���������ļ��Ĳ�ѯ���޸��ļ�����

--*/

/*++

��ȡ���޸��ļ�����

��ȡ���޸��ļ����ԣ�������ȡ�ļ���С����ȡ���޸��ļ�ָ��
λ�á���ȡ���޸��ļ�������ȡ���޸������ԣ�ֻ�����ԡ��������ԣ�
����ȡ���޸��ļ��������޸����ڵȡ�DDK�ṩ���ں˺���ZwSetInformationFile
��ZwQueryInformation���������л�ȡ���޸��ļ����ԡ�

NTSTATUS ZwSetInformationFile (
IN HANDLE FileHandle,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PVOID FileInformation,
IN ULONG Length,
IN FILE_INFORMATION_CLASS FileInformationClass
);

FileHandle: �ļ����
IoStatusBlocks: �������õ�״̬
FileInformation: ����FileInformationClass��ͬ����ͬ��
��Ϊ������Ϣ��
Length: FileInformation���ݵĳ���
FileInformationClass: �����޸����Ե�����

NTSTATUS ZwQueryInformationFile (
IN HANDLE FileHandle,
OUT PIO_STATUS)BLOCK IoStatusBlock,
OUT PVOID FileInformation,
IN ULONG Length,
IN FILE_INFORMATION_CLASS FileInformationClass
);

FileHandle: �ļ����
IoStatusBlock: �������õ�״̬
FileInformation: ����FileInformationClass��ͬ����ͬ��
��Ϊ������Ϣ��
Length: FileInformation���ݵĳ���
FileInformationClass: �����޸����Ե�����

����������������ͬ������FileInformationClassָ���޸Ļ���
��ѯ�����

1. ��FileInformationClass��FileStandardInformationʱ��
����������������FILE_STANDARD_INFORMATION�ṹ�壬�����ļ�
�Ļ�����Ϣ��

typedef struct FILE_STANDARD_INFORMATION {
LARGE_INTEGER AllocationSize; // Ϊ�ļ�����Ĵ�С
LARGE_INTEGER EndOfFile; // �����ļ���β���ж����ֽ�
ULONG NumberOfLinks; // �ж��ٸ������ļ�
BOOLEAN DeletePending; // �Ƿ�׼��ɾ��
BOOLEAN Directory; // �Ƿ�ΪĿ¼
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

2. ��FileInformationClass��FileBasicInformationʱ������
�������������FILE_BASIC_INFORMATION�ṹ�壬�����ļ��Ļ���
��Ϣ��

typedef struct FILE_BASIC_INFORMATION {
LARGE_INTEGER CreationTime; // �ļ�����ʱ��
LARGE_INTEGER LastAccessTime; // ������ʱ��
LARGE_INTEGER LastWriteTime; // ���дʱ��
LARGE_INTEGER ChangeTime; // �޸��޸�ʱ��
ULONG FileAttributes; // �ļ�����
} FILE_BASIC_INPORMATION, *PFILE_BASIC_INFORMATION;

���У�ʱ������Ǵ�һ��LARGE_INTEGER�������������������
1601�꾭������100ns��
FileAttributes�����ļ����ԡ�
FILE_ATTRIBUTE_NORMAL����һ���ļ���
FILE_ATTRIBUTE_DIRECTORY������Ŀ¼��
FILE_ATTRIBUTE_READONLY�������ļ�Ϊֻ����
FILE_ATTRIBUTE_HIDDEN���������ļ���
FILE_ATTRIBUTE_SYSTEM����ϵͳ�ļ���

3. ��FileInformationClass��FileNameInformationʱ������
�������������FILE_NAME_INFORMATION�ṹ�壬�����ļ�����Ϣ��

typedef struct _FILE_NAME_INFORMATION {
ULONG FileNameLength; // �ļ�������
WCHAR FileName[1]; // �ļ���
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

4. ��FileInformationClass��FilePositionInformationʱ����
��������������FILE_POSITION_INFORMATION�ṹ�壬�����ļ���
��Ϣ��

typedef struct FILE_POSITION_INFORMATION {
LARGE_INTEGER CurrentByteOffset; // ����ǰ�ļ�ָ��λ��
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

����������ʾ��

--*/

VOID Test17()
{
	KdPrint(("Test17��������beginning\n"));

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
		0,
		FILE_OPEN, // ���ļ�����������򷵻ش���
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("open file successfully\n"));
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

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("file length:%u\n", fsi.EndOfFile.QuadPart));
	}

	// �޸ĵ�ǰ�ļ�ָ��
	FILE_POSITION_INFORMATION fpi;
	fpi.CurrentByteOffset.QuadPart = 100i64;

	ntStatus = ZwSetInformationFile(
		hfile,
		&iostatus,
		&fpi,
		sizeof(FILE_POSITION_INFORMATION),
		FilePositionInformation
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("update the file pointer successfully!\n"));
	}

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test17��������finishing\n"));
}
