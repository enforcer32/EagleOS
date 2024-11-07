#pragma once

#include <Axe/NXN/Types.h>
#include <stdarg.h>

namespace Axe
{
	void BPrint(const char* str);
	void BPrintChar(char ch);
	int32_t BPrintf(const char* fmt, ...);
}
