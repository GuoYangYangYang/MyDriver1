#include "MyDriver.h"

/*++ 

ʵ�麯��20�������������ر�ע���

--*/

/*++

�ں�ģʽ�µ�ע������

����������Ŀ����У��������õ���ע���Ĳ�����DDK�ṩ��
����һ�׶�ע����������غ�����

ע����еĸ��

ע�����: ע����е�һ����Ŀ������Ŀ¼�ĸ��ÿ������
�洢�����Ԫ�ṹ������������ֵ��ÿ�����У����������ɸ����
ע�������: ������Ŀ¼�е���Ŀ¼��
����: ͨ����������Ѱ�ҵ���Ӧ�ļ�ֵ��
��ֵ���: ÿ����ֵ�洢��ʱ���в�ͬ����𣬿��������Σ�
�ַ��������ݡ�
��ֵ: �����¶�Ӧ�洢�����ݡ�

--*/

/*++

�����ر�ע���

���ļ��������ƣ���ע����������Ҫ��ȡһ��ע���������
ע���Ĳ�������Ҫ�������������в���������ͨ��ZwCreateKey
������ô򿪵�ע����������������ע���󣬲�����һ����
��������亯����������:

NTSTATUS ZwCreateKey (
OUT PHANDLE KeyHandle,
IN ACCESS_MASK DesiredAccess,
IN POBJECT_ATTRIBUTES ObjectAttriburtes,
IN ULONG TitleIndex,
IN PUNICODE_STRING Class OPTIONAL,
IN ULONG CreateOptions,
OUT PULONG Disposition OPTIONAL
);

KeyHandle: ��õ�ע�������
DesiredAccess: ����Ȩ�ޣ�һ������ΪKEY_ALL_ACCESS��
ObjectAttributes: OBJECT_ATTRIBUTES���ݽṹ��
TitleIndex: �����õ���һ������Ϊ0��
Class: �����õ���һ������ΪNULL��
CreateOptions: ����ʱ��ѡ�һ������ΪREG_OPTION_NON_VOLATILE��
Disposition: �����Ǵ����ɹ������Ǵ򿪳ɹ�������ֵ��
REG_CREATED_NEW_KEY������REG_OPENED_EXISTING_KEY��
����ֵ: �����Ƿ񴴽��ɹ���

���ZwCreateKeyָ������Ŀ�����ڣ���ֱ�Ӵ��������Ŀ����
����Disposition��������REG_CREATED_NEW_KEY���������Ŀ�Ѿ�
�����ˣ�Disposition��������REG_OPENED_EXISTING_KEY������
������ʾ��

--*/

VOID Test20()
{
	KdPrint(("Test20��������beginning\n"));

	// �������ĳע�����Ŀ
	UNICODE_STRING RegUnicodeString;
	HANDLE hRegister;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&RegUnicodeString,
		MY_REG_SOFTWARE_KEY_NAME
	);

	OBJECT_ATTRIBUTES objectAttributes;
	// ��ʼ��objectAttributes
	InitializeObjectAttributes(
		&objectAttributes,
		&RegUnicodeString,
		OBJ_CASE_INSENSITIVE, // �Դ�Сд����
		NULL,
		NULL
	);

	ULONG ulResult;
	// �������ע�����Ŀ
	NTSTATUS ntStatus = ZwCreateKey(
		&hRegister,
		KEY_ALL_ACCESS,
		&objectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&ulResult
	);

	if (NT_SUCCESS(ntStatus))
	{
		// �ж��Ǳ��´����������Ѿ�������
		if (ulResult == REG_CREATED_NEW_KEY)
		{
			KdPrint(("The register item is created\n"));
		}
		else if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The register item has been created, and now is opened\n"));
		}
	}

	// �������ĳע�����Ŀ������
	UNICODE_STRING subRegUnicodeString;
	HANDLE hSubRegister;

	// ��ʼ��UNICODE_STRING�ַ���
	RtlInitUnicodeString(
		&subRegUnicodeString,
		L"SubItem"
	);

	OBJECT_ATTRIBUTES subObjectAttributes;
	// ��ʼ��subObjectAttributes
	InitializeObjectAttributes(
		&subObjectAttributes,
		&subRegUnicodeString,
		OBJ_CASE_INSENSITIVE, // �Դ�Сд����
		hRegister,
		NULL
	);

	// �������ע�����Ŀ
	ntStatus = ZwCreateKey(
		&hSubRegister,
		KEY_ALL_ACCESS,
		&subObjectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		&ulResult
	);

	if (NT_SUCCESS(ntStatus))
	{
		// �ж��Ǳ��´����������Ѿ�������
		if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The sub register item is created\n"));
		}
		else if (ulResult == REG_OPENED_EXISTING_KEY)
		{
			KdPrint(("The sub register item has been created, and now is opened\n"));
		}
	}

	// �ر�ע�����
	ZwClose(hRegister);
	ZwClose(hSubRegister);

	KdPrint(("Test20��������finishing\n"));
}