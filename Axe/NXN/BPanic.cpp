#include <Axe/NXN/BPanic.h>
#include <Axe/NXN/BPrintf.h>

namespace Axe
{
	void BPanic(const char* str)
	{
		BPrintf(str);
		while (true);
	}
}
