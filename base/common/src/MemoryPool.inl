#ifndef MEMORY_POOL_INL_
#define MEMORY_POOL_INL_

#pragma once

#include <BaseCode/MemoryPoolApi.h>

#ifndef _DEBUG
	#pragma comment(lib, "BaseCode/lib/Release/MemoryPool.lib")
#else
	#pragma comment(lib, "BaseCode/lib/Debug/MemoryPoolD.lib")
#endif

#endif