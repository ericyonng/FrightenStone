#ifndef BASE_CODE_INL
#define BASE_CODE_INL

#pragma once

//#include <BaseCode/BaseCodeApi.h>

#ifndef _DEBUG
	#pragma comment(lib, "BaseCode/lib/Release/BaseCode.lib")
#else
	#pragma comment(lib, "BaseCode/lib/Debug/BaseCodeD.lib")
#endif

#endif