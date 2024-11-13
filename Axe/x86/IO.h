#pragma once

#include <ESTD/Types.h>

namespace Axe
{
	namespace x86
	{
		namespace IO
		{
			uint8_t INSB(uint16_t port);
			void OUTB(uint16_t port, uint8_t data);
			uint16_t INSW(uint16_t port);
			void OUTW(uint16_t port, uint16_t data);
			uint32_t INSD(uint32_t port);
			void OUTD(uint32_t port, uint32_t data);
		}
	}
}
