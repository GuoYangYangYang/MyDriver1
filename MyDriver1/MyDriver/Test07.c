#include "MyDriver.h"

/*++

ʵ�麯��07���������ַ����ĳ�ʼ��

--*/

/*++

�ں��µ��ַ�������

��Ӧ�ó���һ��������������Ҫ�������ַ����򽻵������а���
ASCII�ַ��������ַ���������DDK�����ANSI_STRING���ݽṹ��
UNICODE_STRING���ݽṹ��

--*/

/*++

ASCII�ַ����Ϳ��ַ���

��Ӧ�ó����У�����ʹ�������ַ���һ����char�͵��ַ�����
�����¼ANSI�ַ���������ָ��һ��char�����ָ�룬ÿ��char��
�����Ĵ�СΪһ���ֽڣ��ַ�������0��־�ַ�������������һ����
wchar_t�����ָ�룬wchar_t�ַ���СΪ�����ֽڣ��ַ�����0��־
�ַ���������
ANSI�ַ��Ĺ������£�
char* str1 = "abc";
str1ָ��ָ���������61 626 300
UNICODE�ַ��Ĺ������£�
wchar_t *str2 = L"abc";
str2ָ��ָ���������6100 620 063 000 000���ڹ����ַ���
��ʱ��ʹ��һ���ؼ��֡�L"�����������Զ���������Ҫ�Ŀ��ַ���
���������򿪷��У�DDK��char��wchar_t����滻��CHAR��
WCHAR���
������������ַ�����DDK�ṩ��Ӧ���ַ����������������磬
strpy��sprintf��strcat��strlen�ȡ���΢��˾������ֱ��ʹ��
��Щ������ȡ����֮����ʹ��ͬ�����ܵĺꡣ

--*/

/*++

ANSI_STRING�ַ�����UNICODE_STRING�ַ���

DDK����������Աʹ��C���Ե��ַ�������Ҫ����Ϊ����׼C����
�������������׵��»���������ȴ����������Ա�����ַ�����
���Ƚ��м��飬�����׵���������󣬴Ӷ�������������ϵͳ�ı�����
DDK��������Աʹ��DDK�Զ�����ַ������������ݸ�ʽ�Ķ������£�

typedef struct _STRING {
USHORT Length;
USHORT MaximumLength;
PCHAR Buffer;
} STRING;
typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;

������ݽṹ��ASCII�ַ��������˷�����
Length: �ַ��ĳ��ȡ�
MaximumLength: �����ַ�������������󳤶ȡ�
Buffer: ��������ָ�롣

�ͱ�׼���ַ�����ͬ��STRING�ַ���������0��־�ַ��������ַ�
��������Length�ֶΡ��ڱ�׼C�е��ַ����У����������������N��
��ôֻ������N-1���ַ����ַ�����������ΪҪ��һ���ֽڴ洢NULL��
����STRING�ַ����У��������Ĵ�СMaximumLength�������ַ���
���ȿ�����MaximumLength��������MaximumLength-1��
��ANSI_STRING���Ӧ��DDK�����ַ�����װ��UNICODE_STRING
���ݽṹ��

typedef struct _UNICODE_STRING {
USHORT Length;
USHORT MaximumLength;
PWSTR Buffer;
} UNICODE_STRING;

Length: �ַ��ĳ��ȡ���λ���ֽڡ������N���ַ�����ôLength
����N��2����
MaximumLength: �����ַ�������������󳤶ȣ���λҲ���ֽڡ�
Buffer: ��������ָ�롣

��ANSI_STRING��ͬ��UNICODE_STRING�Ļ������Ǽ�¼���ַ�
�Ļ�������ÿ��Ԫ���ǿ��ַ�����ANSI_STRINGһ�����ַ����Ľ���
������NULLΪ��־�����������ֶ�Length��

ANSI_STRING�ַ�����UNICODE_STRING�ַ�����KdPrintͬ��
�ṩ�˴�ӡlog�ķ�����

ANSI_STRING ansiString;
// ʡȥ��ʼ��
KdPrint("%Z\n", &ansiString); // ע����%Z

������ӡһ�ο��ַ���ʱ����Ҫ�������²�����

UNICODE_STRING uniString;
// ʡȥ��ʼ��
KdPrint("%wZ\n", &uniString); // ע����%wZ

--*/

/*++

�ַ���ʼ��������

ANSI_STRING�ַ�����UNICODE_STRING�ַ���ʹ��ǰ��Ҫ����
��ʼ���������ְ취����������ݽṹ��

1. ����һ����ʹ��DDK�ṩ����Ӧ�ĺ���

��ʼ��ANSI_STRING�ַ���

VOID RtlInitAnsiString (
IN OUT PANSI_STRING DestinationString,
IN PCSZ SourceString
);

DestinationString: Ҫ��ʼ����ANSI_STRING�ַ�����
SourceString: �ַ��������ݡ�

��ʼ��UNICODE_STRING�ַ���

VOID RtlInitUnicodeString (
IN OUT PUNCODE_STRING DestinationString,
IN PCWSTR SourceString
);

DestinationString: Ҫ��ʼ����UNICODE_STRING�ַ�����
SourceString: �ַ��������ݡ�

��RtlInitAnsiStringΪ������ʹ�÷����ǣ�

ANSI_STRING AnsiString1;
CHAR* string1 = "hello";
RtlInitAnsiString(&AnsiString, string1);

--*/


VOID Test07()
{
	KdPrint(("Test07��������beginning\n"));

	ANSI_STRING AnsiString1;

	CHAR * string1 = "hello";
	// ��ʼ��ANSI_STRING�ַ���
	RtlInitAnsiString(&AnsiString1, string1);
	KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // ��ӡhello

												 // �ı�string1���˴�������������ʹ��
												 //string1[0] = 'H';
												 //string1[1] = 'E';
												 //string1[2] = 'L';
												 //string1[3] = 'L';
												 //string1[4] = 'O';
												 // �ı�string1��AnsiStringͬ���ᵼ�±仯���˴���������
												 // KdPrint(("AnsiString1:%Z\n", &AnsiString1)); // ��ӡHELLO
	KdPrint(("Test07��������finishing\n"));
}
