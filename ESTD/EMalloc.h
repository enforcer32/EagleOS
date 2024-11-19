#pragma once

#ifdef ESTD_BOOTLOADER_BUILD
	#include <Axe/Memory/BMalloc.h>
	#define EMalloc Axe::BMalloc
#elif ESTD_KERNEL_BUILD
	#define EMalloc
#elif ESTD_NONE_BUILD
	#define EMalloc
#else
	#error EMallocUndefined
#endif
