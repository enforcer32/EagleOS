#pragma once

#include <Kernel/ESTD/Types.h>

namespace ESTD
{
	template<typename T>
	const T& Min(const T& a, const T& b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	const T& Max(const T& a, const T& b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	T AlignUp(const T& data, size_t alignment)
	{
		return ((data % alignment) ? ((data + alignment - 1) & ~(alignment - 1)) : data);
	}

	template<typename T>
	T AlignDown(const T& data, size_t alignment)
	{
		return ((data % alignment) ? (data & ~(alignment - 1)) : data);
	}

	template<typename T>
	bool IsAligned(const T& data, size_t alignment)
	{
		return (data % alignment == 0);
	}
}
