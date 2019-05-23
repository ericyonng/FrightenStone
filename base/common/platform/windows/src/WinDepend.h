#ifndef WIN_DEPEND_H_
#define WIN_DEPEND_H_
#pragma once

#if defined(NEED_WINDOWS_COMMON_API)

//#include<windows.h>

//64位获取路径名称
//#include <WS2tcpip.h>
#include <tlhelp32.h>  
#include <Psapi.h>  


#endif // defined(_WIN32)|defined(_WIN64)


//windows依赖文件

#endif // !WIN_DEPEND_H_


