#include <Handshake/BIOS/Video/Print.h>
#include <Handshake/Video/VGA.h>
#include <Handshake/Stdio/Print.h>
#include <Handshake/Stdio/Panic.h>
#include <Handshake/System/MemoryMap.h>

namespace Handshake
{
	void DumpSystemMemory(const System::MemoryMap* memoryMap)
	{
		Printf("\n----------System Memory Map----------\n");
		for (uint32_t i = 0; i < memoryMap->RegionCount; i++)
		{
			const auto& regionInfo = memoryMap->Regions[i];
			Printf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Printf("Length= 0x%x, ", regionInfo.Length);
			Printf("Type= 0x%x, ", regionInfo.Type);
			Printf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		Printf("----------System Memory Map----------\n\n");
	}

	bool Init()
	{
		Video::VGA::Init();
		Video::VGA::ClearScreen();
		Printf("Initializing Handshake Bootloader...!\n");

		System::MemoryMap* systemMemoryMap = System::DetectMemoryMap();
		if (!systemMemoryMap)
		{
			Printf("Failed to Detect Memory Map!\n");
			return false;
		}
		DumpSystemMemory(systemMemoryMap);

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
