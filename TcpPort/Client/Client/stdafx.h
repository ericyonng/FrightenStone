
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#pragma region 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN32
// #include<windows.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif // _WIN32
#pragma endregion


