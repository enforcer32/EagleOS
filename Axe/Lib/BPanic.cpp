#include <Axe/Lib/BPanic.h>
#include <Axe/Lib/BPrintf.h>

namespace Axe
{
	void BPanic(const char* str)
	{
		BPrintf(str);
		while (true);
	}
}
