#include <Handshake/Stdio/Panic.h>
#include <Handshake/Stdio/Print.h>

namespace Handshake
{
	void Panic(const char* str)
	{
		Printf(str);
		while (true);
	}
}
