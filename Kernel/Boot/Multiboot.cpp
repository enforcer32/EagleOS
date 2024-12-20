#include <Kernel/Boot/Multiboot.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/Bitwise.h>

namespace Boot
{
	void MultibootDumpMemoryMap(const MultibootInfo* multibootInfo)
	{
		Kernel::KPrintf("\n----------Multiboot Memory Map----------\n");
		if (!(multibootInfo->Flags & MULTIBOOT_INFO_MEMORY_MAP))
		{
			Kernel::KPrintf("Multiboot Memory Map Flag Not Set!\n");
			return;
		}

		Kernel::KPrintf("MemoryLower 0x%x, MemoryUpper 0x%x\n", (multibootInfo->MemoryLower * 1024) , (multibootInfo->MemoryUpper * 1024));
		for (uint32_t i = 0; i < (multibootInfo->MemoryMapLength / sizeof(MultibootMemoryMap)); i++)
		{
			const auto& memoryMap = multibootInfo->MemoryMapAddress[i];
			Kernel::KPrintf("MemoryMap: {Base= 0x%x, ", memoryMap.BaseAddress);
			Kernel::KPrintf("Length= 0x%x, ", memoryMap.Length);
			Kernel::KPrintf("Type= 0x%x}\n", memoryMap.Type);
		}
		Kernel::KPrintf("----------Multiboot Memory Map----------\n\n");
	}
}
