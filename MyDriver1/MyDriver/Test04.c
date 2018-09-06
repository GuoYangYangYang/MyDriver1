#include "MyDriver.h"

/*++



--*/

/*++

����ʱ����

һ����������̣��ڷ������������ͬʱ���Ὣ����ʱ����һ��
�������û�������ʱ�����ǳ������е�ʱ��ز����ٵģ����ɱ�����
�ṩ����Բ�ͬ�Ĳ���ϵͳ������ʱ������ʵ�ַ�����ͬ�����ӿ�
��������һ�¡�

1. �ڴ�临�ƣ����ص���

���������򿪷��У������õ��ڴ�ĸ��ơ�DDK�ṩ���º�����

VOID RtlCopyMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: ��ʾҪ�����ڴ��Ŀ�ĵ�ַ��
Source: ��ʾҪ�����ڴ��Դ��ַ��
Length: ��ʾҪ�����ڴ�ĳ��ȣ���λ���ֽڡ�
�͸ú����������ƵĻ���RtlCopyBytes�������������Ĳ�����ȫ
һ��������Ҳ��ȫһ����ֻ���ڲ��õ�ƽ̨���в�ͬ��ʵ�֡�

2. �ڴ�临�ƣ����ص���

��RtlCopyMemory���Ը����ڴ棬�����ڲ�û�п����ڴ��ص���
����������εȳ����ڴ棬ABCD�������Ҫ��A��C�ε��ڴ渴�Ƶ�B
��D����ڴ��ϣ���ʱB��C�ε��ڴ�����ص��Ĳ��֡�
RtlCopyMemory�������ڲ�ʵ�ַ���������memory����ʵ�ֵġ�
����C99���壬memoryû�п����ص��Ĳ��֣���������ܱ�֤�ص���
���Ƿ񱻸��ơ�
Ϊ�˱�֤�ص�����Ҳ����ȷ���ƣ�C99�涨memmove���������
��������������������ڴ��Ƿ��ص��������жϣ���ȴ�������ٶȡ�
�������Ա��ȷ�����Ƶ��ڴ�û���ص�����ѡ��ʹ��memopy������
������ܱ�֤�ڴ��Ƿ��ص�����ʹ��memmove������ͬ����Ϊ�˱�֤��
��ֲ�ԣ�DDK�ú��memmove�����˷�װ�����Ʊ�ΪRtlMoveMemory��

VOID RtlMoveMemory (
IN VOID UNALIGNED *Destination,
IN CONST VOID UNALIGNED *Source,
IN SIZE_T Length
);

Destination: ��ʾҪ�����ڴ��Ŀ�ĵ�ַ��
Source: ��ʾҪ�����ڴ��Դ��ַ��
Length: ��ʾҪ�����ڴ�ĳ��ȣ���λ���ֽڡ�

3. ����ڴ�

�������򿪷��У��������õ���ĳ���ڴ������ù̶��ֽ���䡣
DDKΪ����Ա�ṩ�˺���RtlFillMemory��ʵ�ʵĺ���ʱmemset����

VOID RtlFillMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length,
IN UCHAR Fill
);

Destination: Ŀ�ĵ�ַ��
Length: ���ȡ�
Fill: ��Ҫ�����ֽڡ�

���������򿪷��У�������Ҫ��ĳ���ڴ����㣬DDK�ṩ�ĺ���
RtlZeroBytes��RtlZeroMemory��

VOID RtlZeroMemory (
IN VOID UNALIGNED *Destination,
IN SIZE_T Length
);

Destination: Ŀ�ĵ�ַ��
Length: ���ȡ�

4. �ڴ�Ƚ�

�������򿪷��У������õ��Ƚ������ڴ��Ƿ�һ�¡��ú�����
RtlCompareMemory���������ǣ�

ULONG RtlEqualMemory (
CONST VOID *Source1,
CONST VOID *Source2,
SIZE_T Length
);

Source1: �Ƚϵĵ�һ���ڴ��ַ��
Source2: �Ƚϵĵڶ����ڴ��ַ��
Length: �Ƚϵĳ��ȣ���λΪ�ֽڡ�
����ֵ: ��ȵ��ֽ�����

RtlEqualMemoryͨ���жϷ���ֵ��Length�Ƿ���ȣ����ж���
���ڴ��Ƿ���ȫһ�¡��������ڴ�һ�µ�����·��ط���ֵ���ڲ�һ
�µ�����·����㡣

--*/

#define BUFFER_SIZE 1024
#pragma INITCODE
VOID Test04()
{
	KdPrint(("Test04��������beginning\n"));

	PUCHAR pBuffer = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// ��������ڴ�
	RtlZeroMemory(pBuffer, BUFFER_SIZE);
	PUCHAR pBuffer2 = (PUCHAR)ExAllocatePool(PagedPool,
		BUFFER_SIZE);

	// �ù̶��ֽ�����ڴ�
	RtlFillMemory(pBuffer2, BUFFER_SIZE, 0xAA);

	// �ڴ渴��
	RtlCopyMemory(pBuffer, pBuffer2, BUFFER_SIZE);

	// �ж��ڴ��Ƿ�һ��
	ULONG ulRet = RtlCompareMemory(pBuffer, pBuffer2, BUFFER_SIZE);
	if (ulRet == BUFFER_SIZE)
	{
		KdPrint(("The two blocks are same.\n"));
	}

	KdPrint(("Test04��������finishing\n"));
}

/*++

ʹ��C++���Է����ڴ�

΢�������û���ṩ�ں�ģʽ�µ�newģʽ����Ӧ�ö�new��
delete�������������ء�
����new��delete�������������ַ�����һ����ȫ�����أ�һ��
�����������ء�

--*/

/*++

��������

��C�����У�����������8λ��16λ��32λ�������ͣ�����DDK��
��������һ��64λ�ĳ�����������64λ������ֻ���޷�����ʽ����
LONGLONG���ͱ�ʾ��64λ�������ĳ���ǰ����һ�����֣��������
i64��β���磺

LONGLONG llvalue = 100i64;

����LONGLONG���⣬DDK���ṩ��һ��64λ�����ı�ʾ��������
LARGE_INTEGER���ݽṹ����������LONGLONG�ǻ��������ݣ���
LARGE_INTEGER�����ݽṹ��LARGE_INTEGER��������:

typedef union _LARGE_INTEGER {
struct {
ULONG LowPart;
LONG HighPart;
};
struct {
ULONG LowPart;
LONG HighPart;
} u;
LONGLONG QuadPart;
} LARGE_INTEGER;

���Ǹ������壬�������е�����Ԫ�ؿ�����Ϊ��LARGE_INTEGER
���������塣

1. LARGE_INTEGER������Ϊ��������������ɡ�һ���ǵ�32λ����
��HighPart��һ���Ǹ�32λ������HighPart����little endian��
����£���32λ������ǰ����32λ�����ں󡣸�ֵ100���£�

LARGE_INTEGER LargeValue;
LargeValue.LowPart = 100;
LargeValue.HighPart = 0;

2. LARGE_INTEGER������Ϊ��������������ɣ�һ���ǵ�32λ����
��HighPart��һ���Ǹ�32λ������HighPart����big endian��
����£���32λ������ǰ����32λ�����ں󡣸�ֵ100���£�

LARGE_INTEGER LargeValue;
LargeValue.u.LowPart = 100;
LargeValue.u.HighPart = 0;

3. LARGE_INTEGER�ȼ���LONGLONG���ݡ�����������£������ֵ
100�����£�

LARGE_INTEGER LargeValue;
LargeValue.QuadPart = 100i64;

--*/

/*++

����״ֵ̬

DDK�󲿷ֺ����ķ���ֵ������NTSTATUS���͡��ο�DDK.h�ļ���
NTSTATUS������LONG�ȼۡ�

--*/

/*++

����ڴ������

���������򿪷��У����ڴ�Ĳ���Ҫ����С�ģ����ĳ���ڴ���
ֻ���ġ�������������ͼȥд�������ͻᵼ��ϵͳ�ı�����ͬ������
ĳ���ڴ��ǲ��ɶ�������£�����������ͼȥ����ͬ���ᵼ��ϵͳ��
������
DDK�ṩ�����ֺ��������ڲ�֪��ĳ���ڴ��Ƿ�ɶ�д������£�
��̽����ڴ�ɶ�д�ԡ������������ֱ���ProbeForRead��
ProbeForWrite��

VOID ProbeForRead (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: ��Ҫ�������ڴ�ĵ�ַ��
Length: ��Ҫ�������ڴ�ĳ��ȣ���λ���ֽڡ�
Alignment: �����ö��ڴ����Զ����ֽڶ���ġ�

VOID ProbeForWrite (
IN CONST VOID *Address,
IN SIZE_T Length,
IN ULONG Alignment
);

Address: ��Ҫ�������ڴ�ĵ�ַ��
Length: ��Ҫ�������ڴ�ĳ��ȣ���λ���ֽڡ�
Alignment: �����ö��ڴ����Զ����ֽڶ���ġ�
�������������Ƿ��ظö��ڴ��Ƿ�ɶ�д�����ǵ����ɶ�д��ʱ��
������һ���쳣������쳣��Ҫ�õ�΢��������ṩ�ġ��ṹ���쳣��
����취��

--*/