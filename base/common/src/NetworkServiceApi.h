#ifndef NETWORK_SERVICE_API_H_
#define NETWORK_SERVICE_API_H_

#pragma once



//导出定义
#ifndef NETWORK_SERVICE_API
#ifdef  NETWORK_SERVICE_DLL
#define NETWORK_SERVICE_API _declspec(dllexport)
#else
#define NETWORK_SERVICE_API _declspec(dllimport)
#endif
#endif

//定义文件名
#ifndef _DEBUG

#ifdef _UNICODE
#define NETWORK_SERVICE_DLL_NAME				TEXT("NetworkService.dll")		//组件名字
#else
#define NETWORK_SERVICE_DLL_NAME				"NetworkService.dll"				//组件名字
#endif

#else
#ifdef _UNICODE
#define NETWORK_SERVICE_DLL_NAME				TEXT("NetworkServiceD.dll")		//组件名字
#else
#define NETWORK_SERVICE_DLL_NAME				"NetworkServiceD.dll"			//组件名字
#endif
#endif

#ifndef NETWORK_SERVICE_DLL
//导出接口
 //#include <Net/NetConfig.h>
// #include <Net/IOCPModel.h>
#include <Net/SocketProtocol.h>
#include <Net/NetMacro.h>
#include <Net/NetEnum.h>
#include<Net/NetWorkModule.h>

#endif



#endif

