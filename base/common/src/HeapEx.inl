#ifndef HEAP_EX_INL
#define HEAP_EX_INL

#pragma once

#include <BaseCode/HeapExApi.h>

#ifndef _DEBUG
	#pragma comment(lib, "BaseCode/lib/Release/HeapEx.lib")
#else
	#pragma comment(lib, "BaseCode/lib/Debug/HeapExD.lib")
#endif

#endif