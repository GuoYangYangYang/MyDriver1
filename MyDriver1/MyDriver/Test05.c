#include "MyDriver.h"

/*++

实验函数05————try-except

--*/


/*++

结构体异常处理（try-except块）

结构体异常处理是微软编译器提供的独特处理机制，这种处理方式
能在一定程序上在出现错误的情况下，免于程序崩溃。

1. 异常: 异常的概念类似于中断的概念。当程序中某段错误触发一个
异常，操作系统会寻找处理这个异常的处理函数。如果程序提供错误
处理函数，则进入错误处理函数，如果没有提供处理函数，则由操作
系统的默认错误处理函数处理。在内核模式下，操作系统默认处理错
误的方法往往很简单，直接让系统蓝屏，并在蓝屏上简单描述出错的
信息，之后系统就进入死机状态。

2. 回卷: 程序执行到某个地方出现异常错误时，系统会寻找出错点
是否处于一个try{}中，并进入try提供的异常处理代码。如果当前
try块没有提供异常处理，则会向更外一层的try块，寻找异常处理
代码。直到最外层try{}块也没有提供异常处理代码，则交由从操作
系统处理。

这种向更外一层寻找异常处理的机制，被称为回卷。一般处理
异常，是通过try-except块来处理的。

__try
{
}
__except(filter_value)
{
}

在被__try{}包围的块中，如果出现异常，会根据filter_value
的数值，判断是否需要在__except{}中处理。filter_value的数值
会有三种可能。

1. EXCEPTION_EXECUTE_HANDLER，该数值为1。进入到__except
进行错误处理，处理完不再回到__try{}块中，转为继续进行。

2. EXCEPTION_CONTINUE_SEARCH，该数值为0。不使用__except
块中的异常处理，转而向上一层回卷。如果已经是最外层，则向操作
系统请求异常处理函数。

3. EXCEPTION_CONTINUE_EXECUTION，该数值为-1.重复先前错误
的指令，这个很少用到。

ProbeForRead和ProbeForWrite函数可以和try-except块
配合，用来检查某段内存是否可读写。

--*/

VOID Test05()
{
	PVOID badPointer = NULL;

	KdPrint(("Test05————beginning\n"));

	__try
	{
		KdPrint(("Enter __try block\n"));

		// 判断空指针是否可读，显然会导致异常
		ProbeForWrite(badPointer, 100, 4);

		// 由于在上面引发异常，所以以后语句不会被执行
		KdPrint(("Leave __try block\n"));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Catch the exception\n"));
		KdPrint(("The program will keep going\n"));
	}

	// 该语句会被执行
	KdPrint(("Test05————finishing\n"));

}