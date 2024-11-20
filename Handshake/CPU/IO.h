#pragma once

#include <ESTD/Types.h>

namespace Handshake
{
	namespace CPU
	{
		namespace IO
		{
			extern "C" uint8_t INSB(uint16_t port);
			extern "C" void OUTB(uint16_t port, uint8_t data);
			extern "C" uint16_t INSW(uint16_t port);
			extern "C" void OUTW(uint16_t port, uint16_t data);
			extern "C" uint32_t INSD(uint16_t port);
			extern "C" void OUTD(uint16_t port, uint32_t data);
		}
	}
}
