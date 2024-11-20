#include <Handshake/BIOS/Video/Print.h>
#include <Handshake/Video/VGA.h>
#include <Handshake/Stdio/Print.h>
#include <Handshake/Stdio/Panic.h>

namespace Handshake
{
	bool Init()
	{
		Video::VGA::Init();
		Video::VGA::ClearScreen();
		Printf("Initializing Handshake Bootloader...!\n");

		for(;;);
		return false;
	}
}

extern "C" void HSMain()
{
	if (!Handshake::Init())
	{
		Handshake::BIOS::Print("Failed To Initialize Handshake Bootloader!\n");
		return;
	}

	for(;;);
}
