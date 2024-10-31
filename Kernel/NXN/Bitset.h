#pragma once

#include <Kernel/NXN/Types.h>

namespace Kernel
{
	namespace NXN
	{
		template<size_t TSize>
		class Bitset
		{
		public:
			void Set(size_t pos)
			{
				if (pos > TSize)
					return;
				size_t byteIndex = pos / 8;
				size_t bitIndex = pos % 8;
				m_Bits[byteIndex] |= (1 << bitIndex);
			}

			void Test(size_t pos) const
			{
				if (pos > TSize)
					return;
				size_t byteIndex = pos / 8;
				size_t bitIndex = pos % 8;
				return (m_Bits[byteIndex] & (1 << bitIndex)) != 0;
			}

			void Reset(size_t pos)
			{
				if (pos > TSize)
					return;
				size_t byteIndex = pos / 8;
				size_t bitIndex = pos % 8;
				m_Bits[byteIndex] &= ~(1 << bitIndex);
			}

			void Toggle(size_t pos)
			{
				if (pos > TSize)
					return;
				size_t byteIndex = pos / 8;
				size_t bitIndex = pos % 8;
				m_Bits[byteIndex] ^= (1 << bitIndex);
			}
		
		private:
			uint8_t m_Bits[TSize];
		};
	}
}
