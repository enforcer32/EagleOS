#pragma once

#ifdef ESTD_BOOTLOADER_BUILD
	#include <Axe/Memory/BMalloc.h>
	#define EMalloc Axe::BMalloc
#elif ESTD_KERNEL_BUILD
	#error KERNELNOTDEFINEDYET
#else
	#error EMallocUndefined
#endif
