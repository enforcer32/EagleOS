#pragma once

#include <ESTD/Types.h>
#include <stdarg.h>

namespace Handshake
{
	void Print(const char* str);
	void PrintChar(char ch);
	int32_t Printf(const char* fmt, ...);
}
