#pragma once

namespace Kernel
{
	namespace x86
	{
		void EnablePaging();
		void DisablePaging();
		void LoadPageDirectory(void* dir);
	}
}
