#include "MyDriver.h"

/*++

实验函数06————try-finally

--*/

/*++

结构体异常处理（try-finally块）

结构体异常处理还有另外一种使用方法，就是利用try-finally
块，强迫函数在退出前执行一段代码。

--*/

NTSTATUS Test06()
{
	KdPrint(("Test06————beginning\n"));

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
		KdPrint(("Test06————finishing\n"));
	}

}

/*++

上面代码的__try{}块中，无论运行什么代码（即使是return
语句或者触发异常），在程序退出前都会运行__finally{}块中的
代码、这样的目的是，在退出前需要运行一些资源回收的工作，而
资源回收代码的最佳位置就是放在这个块中。
除此之外，使用try-finally块还可以某种程度上简化代码。

--*/

/*++

使用宏需要注意的地方

DDK提供了大量的宏，在使用这些宏的时候需要避免“侧效”。

1. 对于if、while、for这样的语句，不省略{}。这样就能完全保证
不出现“侧效”错误。

2. 在编写多行宏的时候，在宏的前后加上{}。如：
#define PRINT(mag ) {\
KdPrint (("===================\n"));\
KdPrint (mag);\
KdPrint (("===================\n"));\
}

--*/