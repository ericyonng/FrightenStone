

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#undef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 



//#include "targetver.h"
#pragma region 
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define INIT_WIN_SOCK()   WORD ver = MAKEWORD(2, 2);  \
                        WSADATA dat;                \
                        WSAStartup(ver, &dat)
#pragma endregion

#pragma region C++
#include "iostream"
#pragma endregion

#include <stdio.h>
#include <tchar.h>
#include"base/exportbase.h"

