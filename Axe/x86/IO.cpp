#include <Axe/x86/IO.h>

namespace Axe
{
	namespace x86
	{
		namespace IO
		{
			extern "C"
			{
				uint8_t x86_INSB(uint16_t port);
				void x86_OUTB(uint16_t port, uint8_t data);
				uint16_t x86_INSW(uint16_t port);
				void x86_OUTW(uint16_t port, uint16_t data);
				uint32_t x86_INSD(uint32_t port);
				void x86_OUTD(uint32_t port, uint32_t data);
			}

			uint8_t INSB(uint16_t port)
			{
				return x86_INSB(port);
			}

			void OUTB(uint16_t port, uint8_t data)
			{
				x86_OUTB(port, data);
			}

			uint16_t INSW(uint16_t port)
			{
				return x86_INSW(port);
			}

			void OUTW(uint16_t port, uint16_t data)
			{
				x86_OUTW(port, data);
			}

			uint32_t INSD(uint32_t port)
			{
				return x86_INSD(port);
			}

			void OUTD(uint32_t port, uint32_t data)
			{
				x86_OUTD(port, data);
			}
		}
	}
}

