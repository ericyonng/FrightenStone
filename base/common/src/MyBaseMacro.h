#ifndef MY_BASE_MACRO_H_
#define MY_BASE_MACRO_H_

#pragma once

#undef ASSERT
#ifdef _DEBUG
#include "ASSERT.h"
#define ASSERT(x)	assert(x) 

#else
#define ASSERT(x)	(x)
#endif

#undef DEAD_LOOP_BREAK
#define DEAD_LOOP_BREAK(x, n) \
		if((++(x))>n){ ASSERT((x)<=n);break;}

#undef DEAD_LOOP_RETURN
#define DEAD_LOOP_RETURN(x, n) \
		if((++(x))>n){ ASSERT((x)<=n);return;}

#undef DEAD_LOOP_RETURNF
#define DEAD_LOOP_RETURNF(x, n) \
		if((++(x))>n){ ASSERT((x)<=n);return false;}

#undef DEAD_LOOP_LOGF
#define DEAD_LOOP_LOGF(x, n)\
if ((++(x))>n) { ASSERTEX((x) <= n); return false; }

#undef DEAD_LOOP_ASSERT
#define DEAD_LOOP_ASSERT(x, n)\
if ((++(x))>n) { ASSERT((x) <= n); }

#undef DEAD_LOOP_LOG_BREAK
#define DEAD_LOOP_LOG_BREAK(x, n) \
if ((++(x))>n) { ASSERTEX((x) <= n); break; }

#undef DEAD_LOOP_BREAK_NOASSERT
#define DEAD_LOOP_BREAK_NOASSERT(x, n) \
if ((++(x))>n) { break; }

#undef DEAD_LOOP_LOG
#define DEAD_LOOP_LOG(x, n)\
if ((++(x))>n) { ASSERTEX((x) <= n); }

#undef DEAD_LOOP_LOG_DISPLAY
#define DEAD_LOOP_LOG_DISPLAY(x, n, fnDisplay)\
if ((++(x))>n) { ASSERTEX((x) <= n); fnDisplay; }

#define  NAMESPACE_BEGIN(x)  namespace x{

#define  NAMESPACE_END(x)  }

//内存对齐
#define MEM_ALIGN_BEGIN(n)	pack(push, n)
#define MEM_ALIGN_END(n)	pack(pop)

#undef CountArray
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//系统
#undef OS_TYPE_MICRO_WINDOWS
#define OS_TYPE_MICRO_WINDOWS U64(0x1)

#undef OS_TYPE_LINUX
#define OS_TYPE_LINUX	U64(0x2)

#if defined(_WIN32)|defined(_WIN64)

#undef OS_TYPE
#define OS_TYPE	(OS_TYPE_MICRO_WINDOWS)

#else

#if defined(linux)|defined(__CWGWIN__)

#undef OS_TYPE
#define OS_TYPE	(OS_TYPE_LINUX)

#endif

#endif

//参数的输入输出标识
#undef	P_OUT	
#define  P_OUT 	/*输出*/


#undef P_OUTSIZE
#define P_OUTSIZE /*输出目标尺寸*/

#undef	P_IN
#define  P_IN 	/*输入*/

#undef P_INSIZE
#define P_INSIZE /*输入目标尺寸*/

#endif
