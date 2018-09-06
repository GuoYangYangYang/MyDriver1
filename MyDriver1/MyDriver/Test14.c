#include "MyDriver.h"

/*++

ʵ�麯��14���������ļ��Ĵ���

--*/

/*++

�ں�ģʽ�µ��ļ�����

���������򿪷��У���������ļ����в�������Win32API��ͬ��
DDK�ṩ����һ�׶��ļ��Ĳ���������

--*/

/*++

�ļ��Ĵ���

���ļ��Ĵ������ߴ򿪶���ͨ���ں˺���ZwCreateFileʵ�ֵġ�
��Window API���ƣ�����ں˺�������һ���ļ�������ļ������в�
�������������������в����ġ����ļ�������Ϻ���Ҫ�ر����
�����

NTSTATUS ZwCreateFile (
OUT PHANDLE FileHandle,
IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttributes,
OUT PIO_STATUS_BLOCK IoStatusBlock,
IN PLARGE_INTEGER AllocationSize OPTIONAL,
IN ULONG FileAttributes,
IN ULONG ShareAccess,
IN ULONG CreateDisposition,
IN ULONG CreateOption,
IN PVOID EaBuffer OPTIONAL,
IN ULONG EaLength
);

FileHandle: ���ش��ļ��ľ����
DesiredAccess: �Դ��ļ�����������������д����������
һ��ָ��ΪGENERIC_READ��GENERIC_WRITE��
ObjectAttributes: ��OBJECT_ATTRIBUTES�ṹ�ĵ�ַ����
�ṹ����Ҫ��i���ļ�����
IoStatusBlock: ָ��һ��IO_STATUS_BLOCK�ṹ���ýṹ����
ZwCreateFile�����Ľ��״̬��
AllocationSize: ��һ��ָ�룬ָ��һ��64λ����������ָ��
�ļ���ʼ����ʱ�Ĵ�С���ò�������ϵ����������д�ļ����������
����������ô�ļ����Ƚ���0��ʼ��������д���������
FileAttributes: 0����FILE_ATTRIBUTE_NORMAL��ָ����
�����ļ������ԡ�
ShareAccess: FILE_SHARE_READ��0��ָ���ļ��Ĺ���ʽ��
�����Ϊ�����ݶ����ļ���������������߳�ͬʱ��ȡ���ļ������
Ϊд���ݶ����ļ������ܲ�ϣ�������̷߳��ʸ��ļ���
CreateDisposition: FILE_OPEN��FILE_OVERWRITE_IF��
������ָ���ļ����ڻ򲻴���ʱӦ��δ���
CreateOption: FILE_SYNCHRONOUS_IO_NONALERT��ָ������
�򿪲����;��ʹ�õĸ��ӱ�־λ��
EaBuffer: һ��ָ�룬ָ���ѡ����չ��������
EaLength: ��չ�������ĳ��ȡ�

���������Ҫ��дCreateDisposition�������������ļ���
CreateDisposition��������ΪFILE_OPEN������봴���ļ���
CreateDisposition�������ó�FILE_OVERWRITE_IF����ʱ������
�ļ��Ƿ���ڣ����ᴴ�����ļ���
�ļ�����ָ����ͨ���趨����������ObjectAttributes�����
������һ��OBJECT_ATTRIBUTES�ṹ��DDK�ṩ��OBJECT_ATTRIBUTES
�ṹ��ʼ���ĺ�InitializeObjectAttributes��

VOID InitializeObjectAttributes (
OUT POBJECT_ATTRIBUTES InitializeAttributes,
IN PUNICODE_STRING ObjectName,
IN ULONG Attributes,
IN HANDLE RootDirectory,
IN PSECURITY_DESCRIPTOR SecurityDescriptor
);

InitializeAttributes: ���ص�OBJECT_ATTRIBUTES�ṹ��
ObjectName: �������ƣ���UNICODE_STRING��������������
�����ļ�����
Attributes: һ����ΪOBJ_CASE_INSENSITIVE���Դ�Сд���С�
RootDirectory: һ����ΪNULL��
SecurityDescriptor: һ����ΪNULL��

���⣬�ļ��������Ƿ������ӻ������豸����

������ʾ��

--*/

VOID Test14()
{
	KdPrint(("Test14��������beginning\n"));

	OBJECT_ATTRIBUTES objectAttributes;
	IO_STATUS_BLOCK iostatus;
	HANDLE hfile;
	UNICODE_STRING logFileUnicodeString;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&logFileUnicodeString,
		L"\\??\\C:\\1.log"
	);
	// ��д��"\\Device\\HarddiskVolume1\\1.LOG"

	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&logFileUnicodeString,
		OBJ_CASE_INSENSITIVE,
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
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);

	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Create file successfully!\n"));
	}
	else
	{
		KdPrint(("Create file unsuccessfully!\n"));
	}

	// �ļ�����
	// do sometihings
	KdPrint(("do somethings\n"));

	// �ر��ļ����
	ZwClose(hfile);

	KdPrint(("Test14��������finishing\n"));
}
