#ifndef MEM_POOL_OBJ_API_H_
#define MEM_POOL_OBJ_API_H_

#pragma once



//导出定义
#ifndef MEM_POOL_OBJ_API
#ifdef  MEM_POOL_OBJ_DLL
#define MEM_POOL_OBJ_API _declspec(dllexport)
#else
#define MEM_POOL_OBJ_API _declspec(dllimport)
#endif
#endif

//定义文件名
#ifndef _DEBUG

#ifdef _UNICODE
#define MEM_POOL_OBJ_DLL_NAME				TEXT("MemPoolObj.dll")		//组件名字
#else
#define MEM_POOL_OBJ_DLL_NAME				"MemPoolObj.dll"				//组件名字
#endif

#else
#ifdef _UNICODE
#define MEM_POOL_OBJ_DLL_NAME				TEXT("MemPoolObjD.dll")		//组件名字
#else
#define MEM_POOL_OBJ_DLL_NAME				"MemPoolObjD.dll"			//组件名字
#endif
#endif

#ifndef MEM_POOL_OBJ_DLL
//导出接口
#include <PoolObj/xByte.h>
#include <PoolObj/xUByte.h>

#endif



#endif

