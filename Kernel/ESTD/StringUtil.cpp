#include <Kernel/ESTD/StringUtil.h>
#include <Kernel/ESTD/TypeConverter.h>

namespace ESTD
{
	namespace StringUtil
	{
		ESTD::String ToString(int32_t num)
		{
			char buf[20];
			return ESTD::String(TypeConverter::IntToStr(num, buf, sizeof(buf), 10));
		}
	}
}
