#ifndef WIN_DEPEND_INL_H_
#define WIN_DEPEND_INL_H_

#pragma once

//#ifdef COMPILE_WINDOWS_BASECODE_API

#if defined(NEED_WINDOWS_COMMON_API)

	#include <BaseCode/Win/WinDataType.h>			//数据类型
	#include <BaseCode/Win/WinDepend.h>			//依赖头文件
	#include <BaseCode/Win/WinDependLib.h>		//依赖库

#endif // defined(_WIN32)|defined(_WIN64)
//#endif

#endif // !WIN_DEPEND_INL_H_


