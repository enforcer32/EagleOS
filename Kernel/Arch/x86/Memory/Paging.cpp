#include <Kernel/Arch/x86/Memory/Paging.h>
#include <Kernel/NXN/Errno.h>

namespace Kernel
{
	namespace x86
	{
		extern "C" void x86_EnablePaging();
		extern "C" void x86_DisablePaging();
		extern "C" void x86_LoadPageDirectory(void* dir);

		void EnablePaging()
		{
			x86_EnablePaging();
		}

		void DisablePaging()
		{
			x86_DisablePaging();
		}

		void LoadPageDirectory(void* dir)
		{
			x86_LoadPageDirectory(dir);
		}
	}
}
