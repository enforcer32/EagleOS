#pragma once

#include <Kernel/NXN/Types.h>

namespace Kernel
{
	namespace NXN
	{
		namespace Bitwise
		{	
			template<typename T>
			void BitSet(T& number, T pos)
			{
				number |= (1 << pos);
			}

			template<typename T>
			void BitClear(T& number, T pos)
			{
				number &= ~(1 << pos);
			}

			template<typename T>
			void BitToggle(T& number, T pos)
			{
				number ^= (1 << pos);
			}

			template<typename T>
			bool BitTest(T number, T pos)
			{
				return ((number >> pos) & 1);
			}

			template<typename T>
			void BitmaskSet(T& number, T mask)
			{
				number |= mask;
			}

			template<typename T>
			void BitmaskClear(T& number, T mask)
			{
				number &= ~(mask);
			}

			template<typename T>
			void BitmaskToggle(T& number, T mask)
			{
				number ^= mask;
			}

			template<typename T>
			bool BitmaskTestOne(T number, T mask)
			{
				return (number & mask);
			}

			template<typename T>
			bool BitmaskTestAll(T number, T mask)
			{
				return !(~number & mask);
			}
		}
	}
}
