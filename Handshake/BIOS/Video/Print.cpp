#include <Handshake/BIOS/Video/Print.h>
#include <ESTD/CString.h>

namespace Handshake
{
	namespace BIOS
	{
		void Print(const char* str)
		{
			size_t len = ESTD::Strlen(str);
			for(size_t i = 0; i < len; i++)
				PrintChar(str[i]);
		}
	}
}
