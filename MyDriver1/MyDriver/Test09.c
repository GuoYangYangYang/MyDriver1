#include "MyDriver.h"

/*++

ʵ�麯��09���������ַ����ĸ���

--*/

/*++

�ַ�������

DDK�ṩ���ANSI_STRING�ַ�����UNICODE_STRING�ַ�����
�����ַ�������ֱ��ǣ�
ANSI_STRING�ַ������ƺ���

VOID RtlCopyString (
IN OUT PSTRING DestinationString,
IN PSTRING SourceString OPTIONAL
);

DestinationString: Ŀ���ַ���
SourceString: Դ�ַ���

UNICODE_STRING�ַ������ƺ���

VOID RtlCopyUnicodeString (
IN OUT PUNICODE_STRING DestinationString,
IN PUNICODE_STRING SourceString
);

DestinationString: Ŀ���ַ���
SourceString: Դ�ַ���

����Ĵ�����ʾ�����ʹ��RtlCopyUnicodeString������

--*/

VOID Test09()
{
	KdPrint(("Test09��������beginning\n"));

	// ��ʼ��UnicodeString1
	UNICODE_STRING UnicodeString1;
	RtlInitUnicodeString(&UnicodeString1, L"Hello World");

	// ��ʼ��UnicodeString2
	UNICODE_STRING UnicodeString2 = { 0 };
	UnicodeString2.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	UnicodeString2.MaximumLength = BUFFER_SIZE;

	// ����ʼ��UnicodeString2���Ƶ�UnicodeString1
	RtlCopyUnicodeString(&UnicodeString2, &UnicodeString1);

	// �ֱ���ʾUnicodeString1��UnicodeString2
	KdPrint(("UnicodeString1:%wZ\n", &UnicodeString1));
	KdPrint(("UnicodeString2:%wZ\n", &UnicodeString2));

	// ����UnicodeString2
	// ע��UnicodeString1��������
	RtlFreeUnicodeString(&UnicodeString2);

	KdPrint(("Test09��������finishing\n"));
}