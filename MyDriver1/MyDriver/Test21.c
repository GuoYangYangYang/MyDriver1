#include "MyDriver.h"

/*++

ʵ�麯��21����������ע���

--*/

/*++

��ע���

	ZwCreateKey�����ȿ��Դ���ע����Ҳ���Դ�
ע����Ϊ�˼򻯴�ע����DDK�ṩ���ں˺���
ZwOpenKey���Լ򻯴򿪲��������ZwOpenKeyָ������
�����ڣ����ᴴ�������Ŀ�����Ƿ���һ������״̬���ú���
�������£�

NTSTATUS ZwOpenKey (
	OUT PHANDLE KeyHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes
);

	KeyHandle: ���ر��򿪵ľ����
	DesiredAccess: �򿪵�Ȩ�ޣ�һ����ΪKEY_ALL_ACCESS��
	ObjectAttributes: OBJECT_ATTRIBUTES���ݽṹ��
ָʾ�򿪵�״̬��
	����ֵ: �����Ƿ�򿪳ɹ���

	ZwOpenKey�Ĳ�����ZwCreateKey�Ĳ����򻯣�����Ա
ʹ�øú�����ܷ��㡣�������£�

--*/

VOID Test21()
{
	KdPrint(("Test21��������beginning\n"));

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

	// ��ע���
	NTSTATUS ntStatus = ZwOpenKey(
		&hRegister,
		KEY_ALL_ACCESS,
		&objectAttributes
	);

	// �жϲ����Ƿ�ɹ�
	if (NT_SUCCESS(ntStatus))
	{
		KdPrint(("Open register successfully\n"));
	}
	
	// �رվ��
	ZwClose(hRegister);

	KdPrint(("Test21��������finishing\n"));
}