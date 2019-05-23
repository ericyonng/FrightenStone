#ifndef LOG_EX_INL
#define LOG_EX_INL

#pragma once

#include <BaseCode/LogExApi.h>

#ifndef _DEBUG
	#pragma comment(lib, "BaseCode/lib/Release/LogEx.lib")
#else
	#pragma comment(lib, "BaseCode/lib/Debug/LogExD.lib")
#endif

#endif