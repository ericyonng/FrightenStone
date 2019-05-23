#ifndef INLINE_FUN_HPP_
#define INLINE_FUN_HPP_

#pragma once
#include <BaseCode/MyBaseMacro.h>
//#include <limits>
NAMESPACE_BEGIN(gtool)

template<typename T>
static inline int GetInputWidth(const T& tInput)		//T需要为整型类型
{
	T tTmp = tInput;
	int nCount = 1;
	const auto nLoopMax = (std::numeric_limits<char>::max)();
	int usLoopCount = 0;
	while ((tTmp /= 10) != 0)
	{
		DEAD_LOOP_BREAK(usLoopCount, nLoopMax);
		nCount++;
	}

	return nCount;
}


template<typename T>
static inline const int GetDataTypeStrWidth()		//T需要为整型类型
{
	int nCount = 1;
	const auto nLoopMax = (std::numeric_limits<char>::max)();
	auto nValueMax = (std::numeric_limits<T>::max)();
	int usLoopCount = 0;
	while ((nValueMax /= 10) != 0)
	{
		DEAD_LOOP_BREAK(usLoopCount, nLoopMax);
		nCount++;
	}

	return nCount;
}

template<typename T>
static inline const int GetDataTypeBitWidth()		//T需要为整型类型
{
	int nCount = 1;
	const auto nLoopMax = (std::numeric_limits<char>::max)();
	auto nValueMax = (std::numeric_limits<T>::max)();
	int usLoopCount = 1;
	while ((nValueMax /= 2) != 0)
	{
		DEAD_LOOP_BREAK(usLoopCount, nLoopMax);
		nCount++;
	}

	return nCount;
}

NAMESPACE_END(gtool)

#endif