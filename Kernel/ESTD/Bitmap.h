#pragma once

#include <Kernel/ESTD/Types.h>
#include <Kernel/ESTD/CString.h>

namespace ESTD
{
	template<size_t TSize>
	class Bitmap
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

		void SetAll()
		{
			ESTD::Memset(m_Bits, 1, sizeof(m_Bits));
		}

		bool Test(size_t pos) const
		{
			if (pos > TSize)
				return false;
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

		void ResetAll()
		{
			ESTD::Memset(m_Bits, 0, sizeof(m_Bits));
		}

		void Toggle(size_t pos)
		{
			if (pos > TSize)
				return;
			size_t byteIndex = pos / 8;
			size_t bitIndex = pos % 8;
			m_Bits[byteIndex] ^= (1 << bitIndex);
		}

		int32_t FindUnsetBits(size_t count)
		{
			int32_t firstBit = -1;
			uint32_t currentBit = 0;

			for (uint32_t i = 0; i < TSize; i++)
			{
				if (Test(i))
				{
					firstBit = -1;
					currentBit = 0;
					continue;
				}

				if (firstBit == -1)
					firstBit = i;
				
				currentBit++;
				if (currentBit == count)
					break;
			}

			if (firstBit == -1)
				return -1;

			return firstBit;
		}

		void SetBits(size_t start, size_t count)
		{
			if ((start + count) > TSize)
				return;
			for (size_t i = start; i < (start + count); i++)
				Set(i);
		}

		void ClearBits(size_t start, size_t count)
		{
			if ((start + count) > TSize)
				return;
			for (size_t i = start; i < (start + count); i++)
				Reset(i);
		}
	
	private:
		uint8_t m_Bits[TSize];
	};
}
