#pragma once

#include <Kernel/ESTD/Types.h>

namespace Kernel
{
	namespace x86
	{
		class PIC8259
		{
		public:
			static int32_t Init();
			static void Shutdown();
			static void SendEOI(uint32_t irq);

		private:
			static int32_t Remap();
		};
	}
}
