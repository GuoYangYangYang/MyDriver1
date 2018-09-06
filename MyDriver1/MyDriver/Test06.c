#include "MyDriver.h"

/*++

ʵ�麯��06��������try-finally

--*/

/*++

�ṹ���쳣����try-finally�飩

�ṹ���쳣����������һ��ʹ�÷�������������try-finally
�飬ǿ�Ⱥ������˳�ǰִ��һ�δ��롣

--*/

NTSTATUS Test06()
{
	KdPrint(("Test06��������beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));
		// do somethings
		KdPrint(("Leave __try block\n"));
		return STATUS_SUCCESS;
	}

	__finally
	{
		KdPrint(("Enter __finally block\n"));
		// do somethings
		KdPrint(("Leave __fianlly block\n"));
		KdPrint(("Test06��������finishing\n"));
	}

}

/*++

��������__try{}���У���������ʲô���루��ʹ��return
�����ߴ����쳣�����ڳ����˳�ǰ��������__finally{}���е�
���롢������Ŀ���ǣ����˳�ǰ��Ҫ����һЩ��Դ���յĹ�������
��Դ���մ�������λ�þ��Ƿ���������С�
����֮�⣬ʹ��try-finally�黹����ĳ�̶ֳ��ϼ򻯴��롣

--*/

/*++

ʹ�ú���Ҫע��ĵط�

DDK�ṩ�˴����ĺ꣬��ʹ����Щ���ʱ����Ҫ���⡰��Ч����

1. ����if��while��for��������䣬��ʡ��{}������������ȫ��֤
�����֡���Ч������

2. �ڱ�д���к��ʱ���ں��ǰ�����{}���磺
#define PRINT(mag ) {\
KdPrint (("===================\n"));\
KdPrint (mag);\
KdPrint (("===================\n"));\
}

--*/