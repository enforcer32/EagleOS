#pragma once

#include <Kernel/ESTD/Types.h>

namespace ESTD
{
	template<typename T, size_t TSize>
	class Array
	{
	public:
		T& operator[](size_t index)
		{
			return m_Data[index];
		}

		const T& operator[](size_t index) const
		{
			return m_Data[index];
		}

		constexpr size_t Size() const
		{
			return TSize;
		}

		const T* Data() const
		{
			return m_Data;
		}

	private:
		T m_Data[TSize];
	};
}
