#include "MyDriver.h"

/*++

ʵ�麯��08���������ַ����ĳ�ʼ��

--*/

/*++

����һ�ַ����ǳ���Ա�Լ������ڴ档����ʼ���ڴ棬��������
����ʱ����Ҫ�����ַ���ռ�õ��ڴ档

--*/

VOID Test08()
{
	KdPrint(("Test08��������beginning\n"));
	UNICODE_STRING UnicodeString1 = { 0 };

	// ���û�������С
	UnicodeString1.MaximumLength = BUFFER_SIZE;

	// �����ڴ�
	UnicodeString1.Buffer = (PWSTR)ExAllocatePool(
		PagedPool, BUFFER_SIZE
	);
	WCHAR* wideString = L"hello";

	// �����ַ����ȣ���Ϊ�ǿ��ַ����������ַ����ȵ�2��
	UnicodeString1.Length = 2 * wcslen(wideString);

	// ��֤�������㹻�󣬷��������ֹ
	ASSERT(UnicodeString1.MaximumLength >=
		UnicodeString1.Length);

	// �ڴ渴��
	RtlCopyMemory(
		UnicodeString1.Buffer,
		wideString,
		UnicodeString1.Length);

	// �����ַ�����
	UnicodeString1.Length = 2 * wcslen(wideString);

	KdPrint(("UnicodeString:%wZ\n", &UnicodeString1));

	// �����ڴ�
	ExFreePool(UnicodeString1.Buffer);
	UnicodeString1.Buffer = NULL;
	UnicodeString1.Length = UnicodeString1.MaximumLength = 0;

	KdPrint(("Test08��������finishing\n"));
}

/*++

�������һ�������ڴ棬DDKͬ�������˼򻯺������ֱ���
RtlFreeAnsiString��RtlFreeUnicodeString�������������ڲ�
������ExFreePoolȥ�����ڴ档

--*/