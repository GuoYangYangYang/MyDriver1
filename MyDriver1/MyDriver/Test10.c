#include "MyDriver.h"

/*++

ʵ�麯��10���������ַ����ıȽ�

--*/


/*++

�ַ����Ƚ�

DDK�ṩ�˶�ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
����ַ����Ƚϵ�����ֱ���ANSI_STRING�ַ����ȽϺ�����
UNICODE_STRING�ַ����ȽϺ�����

LONG RtlCompareString (
IN PSTRING String1,
IN PSTRING String2,
BOOLEAN CaseInSensitive
);

String1: Ҫ�Ƚϵĵ�һ���ַ���
String2: Ҫ�Ƚϵĵڶ����ַ���
CaseInSensitive: �Ƿ�Դ�Сд����
����ֵ: �ȽϽ��

LONG RtlCompareUnicodeString(
IN PUNICODE_STRING String1,
IN PUNICODE_STRING String2,
IN BOOLEAN CaseInSensitive
);

String1: Ҫ�Ƚϵĵ�һ���ַ���
String2: Ҫ�Ƚϵĵڶ����ַ���
CaseInSensitive: �Ƿ�Դ�Сд����
����ֵ: �ȽϽ��

�����������Ĳ�����ʽ��ͬ�������������ֵΪ0�����ʾ����
�ַ�����ȡ����С��0�����ʾ��һ���ַ���С�ڵڶ����ַ�����
�������0�����ʾ��һ���ַ������ڵڶ����ַ�����
ͬʱ��DDK���ṩ��RtlEqualString��RtlEqualUnicodeString
��������ʹ�÷��������������������ơ�ֻ�Ƿ���Ϊ���������ȣ���
������ȡ�

--*/

VOID Test10()
{
	KdPrint(("Test10��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2;
	RtlInitUnicodeString(&UnicodeString2, L"Hello");

	// �ж��ַ����Ƿ����
	if (RtlEqualUnicodeString(&UnicodeString1, &UnicodeString2, TRUE))
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are equal\n"));
	}
	else
	{
		KdPrint(("UnicodeString1 and UnicodeString2 are NOT equal"));
	}

	KdPrint(("Test10��������finishing\n"));
}