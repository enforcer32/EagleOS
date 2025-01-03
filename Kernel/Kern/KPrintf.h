#pragma once

#include <Kernel/ESTD/Types.h>
#include <stdarg.h>

namespace Kernel
{
	void KPrint(const char* str);
	void KPrintChar(char ch);
	int32_t KPrintf(const char* fmt, ...);
}
