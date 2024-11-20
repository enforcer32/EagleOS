#pragma once

#ifdef ESTD_BOOTLOADER_BUILD
	#include <Handshake/Memory/Malloc.h>
	#define EMalloc Handshake::Malloc
#elif ESTD_KERNEL_BUILD
	#define EMalloc
#elif ESTD_NONE_BUILD
	#define EMalloc
#else
	#error EMallocUndefined
#endif
