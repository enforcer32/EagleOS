#include <Kernel/ESTD/StdLib.h>

#ifdef ESTD_BOOTLOADER_BUILD
	#include <Handshake/Memory/Malloc.h>
#elif ESTD_KERNEL_BUILD
	#include <Kernel/Memory/KMalloc.h>
#else
	#error ESTDMallocUndefined
#endif

namespace ESTD
{
	void* Malloc(uint32_t size)
	{
		#ifdef ESTD_BOOTLOADER_BUILD
			return Handshake::Malloc(size);
		#elif ESTD_KERNEL_BUILD
			return Kernel::Memory::KMalloc(size);
		#else
			#error ESTDMallocUndefined
		#endif
	}
}
