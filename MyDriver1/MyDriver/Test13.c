#include "MyDriver.h"

/*++

ʵ�麯��13�������������ַ���֮���ת��

--*/

/*++

ANSI_STRING�ַ�����UNICODE_STRING�ַ����໥ת��

DDK�ṩ��ANSI_STRING�ַ�����UNICODE_STRING�ַ����໥
ת������غ�����

1. ��UNICODE_STRING�ַ���ת����ANSI_STRING�ַ�����

DDK��������ת���ṩ�ĺ�����RtlUnicodeStringToAnsiString,
�������ǣ�

NTSTATUS RtlUnicodeStringToAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PUNICODE_STRING SourceString,
IN BOOLEAN AllocateDestinationString
);

DestinationString: ��Ҫ��ת�����ַ���
SourceString: ��Ҫת����Դ�ַ���
AllocateDestinationString: �Ƿ���Ҫ�Ա�ת�����ַ�����
���ڴ档
����ֵ: ָ���Ƿ�ת���ɹ���

2. ��ANSI_STRING�ַ���ת����UNICODE_STRING�ַ���

DDK��������ת���ṩ�ĺ�����RtlAnsiStringToUnicodeString,
�������ǣ�

NTSTATUS RtlAnsiStringToUnicodeString (
IN OUT PUNICODE_STRING DestinationString,
IN PUNICODE_STRING SourceString,
IN BOOLEAN AllocateDestinationString
);

DestinationString: ��Ҫ��ת�����ַ���
SourceString: ��Ҫת����Դ�ַ���
AllocateDestinationString: �Ƿ���Ҫ�Ա�ת�����ַ�����
���ڴ档
����ֵ: ָ���Ƿ�ת���ɹ���

������ʾ��

--*/

VOID Test13()
{
	KdPrint(("Test13��������beginning\n"));

	// ��UNICODE_STRING�ַ���ת����ANSI_STRING�ַ���
	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	ANSI_STRING AnsiString1;
	NTSTATUS nStatus = RtlUnicodeStringToAnsiString(
		&AnsiString1, &UnicodeString1, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%Z\n", &AnsiString1));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// ����AnsiString1
	RtlFreeAnsiString(&AnsiString1);

	// ��ANSI_STRING�ַ���ת����UNICODE_STRING�ַ���
	// ��ʼ��AnsiString2
	ANSI_STRING AnsiString2;
	RtlInitString(&AnsiString2, "Hello world");

	UNICODE_STRING UnicodeString2;
	nStatus = RtlAnsiStringToUnicodeString(
		&UnicodeString2, &AnsiString2, TRUE
	);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver unsuccessfully!\n"));
	}

	// ����UnicodeString2
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test13��������finishing\n"));
}

