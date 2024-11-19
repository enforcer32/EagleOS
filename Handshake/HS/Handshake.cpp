#include <Handshake/BIOS/Video/Print.h>

namespace Handshake
{
	bool InitHandshake()
	{
		BIOS::Print("Initializing Handshake Bootloader...\n");

		return false;
	}
}

extern "C" void HSMain()
{
	if (!Handshake::InitHandshake())
	{
		Handshake::BIOS::Print("Failed To Initialize Handshake Bootloader!\n");
		return;
	}

	for(;;);
}
