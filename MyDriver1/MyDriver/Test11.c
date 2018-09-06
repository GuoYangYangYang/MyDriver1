#include "MyDriver.h"

/*++

ʵ�麯��11���������ַ����Ĵ�д

--*/


/*++

�ַ���ת���ɴ�д

DDK�ṩ�˶�ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
����ַ�����Сдת���ĺ�����

1. ANSI_STRING�ַ���ת���ɴ�д

VOID RtlUpperString (
IN OUT PSTRING DestinationString,
IN PSTRING SourceString
);

DestinationString: Ŀ���ַ�����
SourceString: Դ�ַ�����

2. UNICODE_STRING�ַ���ת���ɴ�д

NTSTATUS RtlUpcaseUnicodeString (
IN OUT PUNICODE_STRING DestinationString OPTIONAL,
IN PCUNICODE_STRING SourceString,
IN BOOLEAN AllocateDestinationString
);

DestinationString: Ŀ���ַ�����
SourceString: Դ�ַ�����
AllocateDestinationString: �Ƿ�ΪĿ���ַ��������ڴ档
����ֵ: ����ת���Ƿ�ɹ���

RtlUpcaseUnicodeString������RtlUpperString������һ��
����AllocateDestinationString���������ָ���Ƿ�ΪĿ���ַ���
�����ڴ档Ŀ���ַ�����Դ�ַ���������ͬһ���ַ�����
DDK��Ȼ�ṩ��ת���ɴ�д�ĺ�������ȴû���ṩת����Сд��
������������ʾ��

--*/

VOID Test11()
{
	KdPrint(("Test11��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2;
	// RtlInitUnicodeString(&UnicodeString2, L" ");

	// �仯ǰ
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));

	// ת���ɴ�д
	RtlUpcaseUnicodeString(&UnicodeString2, &UnicodeString1, TRUE);

	// �仯��
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	KdPrint(("Test11��������finishing\n"));
}