#ifndef THREAD_POST_INL
#define THREAD_POST_INL

#pragma once

#include <TDPoster/ThreadPostApi.h>


#ifndef _DEBUG
	#pragma comment(lib, "BaseCode/lib/Release/ThreadPost.lib")
#else
	#pragma comment(lib, "BaseCode/lib/Debug/ThreadPostD.lib")
#endif

#endif