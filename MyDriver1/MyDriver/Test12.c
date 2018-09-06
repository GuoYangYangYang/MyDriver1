#include "MyDriver.h"

/*++

ʵ�麯��12���������ַ��������ֵ�ת��

--*/

/*++

�ַ��������������໥ת��

DDK�ṩ��UNICODE_STRING�ַ����������໥ת�����ں˺�����

1. ��UNICODE_STRING�ַ���ת�������Ρ�

���������RtlUnicodeStringToInteger���������ǣ�

NTSTATUS RtlUnicodeStringToInteger (
IN PUNICODE_STRING String,
IN ULONG Base OPTIONAL,
OUT PULONG Value
);

String: ��Ҫת�����ַ���
Base: ת�������Ľ��ƣ���2��8��10��16��
Value: ��Ҫת��������
����ֵ: ָ���Ƿ�ת���ɹ�

2. ������ת����UNICODE_STRING�ַ���

�������RtlIntegerToUnicodeString���������ǣ�

NTSTATUS RtlIntegerToUnicodeString (
IN ULONG Value,
IN ULONG Base OPTIONAL,
IN OUT PUNICODE_STRING String
);

Value: ��Ҫת��������
Base: ת�������Ľ��ƣ���2��8��10��16��
String: ��Ҫת�����ַ���
����ֵ: ָ���Ƿ�ת���ɹ�

������ʾ��

--*/

VOID Test12()
{
	KdPrint(("Test12��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"-100");

	ULONG lNumber;
	NTSTATUS nStatus = RtlUnicodeStringToInteger(
		&UnicodeString1, 10, &lNumber);
	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to integer successfully!\n"));
		KdPrint(("Result:%d\n", lNumber));
	}
	else
	{
		KdPrint(("Conver to integer unsuccessfully!\n"));
	}

	// ����ת�����ַ���
	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;
	nStatus = RtlIntegerToUnicodeString(200, 10, &UnicodeString2);

	if (NT_SUCCESS(nStatus))
	{
		KdPrint(("Conver to string successfully!\n"));
		KdPrint(("Result:%wZ\n", &UnicodeString2));
	}
	else
	{
		KdPrint(("Conver to string unsuccessfully\n"));
	}

	// ����UnicodeString2��ע��UnicodeString1��������
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test12��������finishing\n"));
}
