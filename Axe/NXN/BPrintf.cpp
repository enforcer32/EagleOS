#include <Axe/NXN/BPrintf.h>
#include <Axe/Graphics/VGA.h>
#include <Axe/NXN/TypeConverter.h>

namespace Axe
{
	void BPrint(const char* str)
	{
		Graphics::VGA::WriteStr(str);
	}

	void BPrintChar(char ch)
	{
		Graphics::VGA::WriteChar(ch);
	}

	int32_t BPrintf(const char* fmt, ...)
	{
		va_list ap;
		const char* ptr;
		char* c;
		int32_t val;

		va_start(ap, fmt);
		for (ptr = fmt; *ptr; ptr++)
		{
			if (*ptr != '%')
			{
				BPrintChar(*ptr);
				continue;
			}

			switch (*++ptr)
			{
			case 'd':
			case 'u':
			case 'i':
			{
				val = va_arg(ap, int);
				char tmp[12];
				NXN::TypeConverter::IntToStr(val, tmp, sizeof(tmp), 10);
				BPrint(tmp);
				break;
			}
			case 'x':
			{
				val = va_arg(ap, int);
				char tmp[12];
				NXN::TypeConverter::IntToStr(val, tmp, sizeof(tmp), 16);
				BPrint(tmp);
				break;
			}
			case 'c':
			{
				char ch = va_arg(ap, int);
				BPrintChar(ch);
				break;
			}
			case 's':
			{
				c = va_arg(ap, char*);
				BPrint(c);
				break;
			}
			default:
				BPrintChar(*ptr);
				break;
			}
		}
		va_end(ap);

		return 0;
	}
}
