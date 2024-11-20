#include <Handshake/Stdio/Print.h>
#include <Handshake/Video/VGA.h>
#include <ESTD/TypeConverter.h>

namespace Handshake
{
	void Print(const char* str)
	{
		Video::VGA::WriteStr(str);
	}

	void PrintChar(char ch)
	{
		Video::VGA::WriteChar(ch);
	}

	int32_t Printf(const char* fmt, ...)
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
				PrintChar(*ptr);
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
				ESTD::TypeConverter::IntToStr(val, tmp, sizeof(tmp), 10);
				Print(tmp);
				break;
			}
			case 'x':
			{
				val = va_arg(ap, int);
				char tmp[12];
				ESTD::TypeConverter::IntToStr(val, tmp, sizeof(tmp), 16);
				Print(tmp);
				break;
			}
			case 'c':
			{
				char ch = va_arg(ap, int);
				PrintChar(ch);
				break;
			}
			case 's':
			{
				c = va_arg(ap, char*);
				Print(c);
				break;
			}
			default:
				PrintChar(*ptr);
				break;
			}
		}
		va_end(ap);

		return 0;
	}
}
