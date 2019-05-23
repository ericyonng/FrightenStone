#ifndef MEMORY_POOL_API_H_
#define MEMORY_POOL_API_H_

#pragma once


//导出定义
#ifndef MEMORY_POOL_API
	#ifdef  MEMORY_POOL_DLL
		#define MEMORY_POOL_API _declspec(dllexport)
	#else
		#define MEMORY_POOL_API _declspec(dllimport)
	#endif
#endif

//定义文件名
#ifndef _DEBUG

	#ifdef _UNICODE
		#define MEMORY_POOL_DLL_NAME				TEXT("MemoryPool.dll")		//组件名字
	#else
		#define MEMORY_POOL_DLL_NAME				"MemoryPool.dll"				//组件名字
	#endif

#else
	#ifdef _UNICODE
		#define MEMORY_POOL_DLL_NAME				TEXT("MemoryPoolD.dll")		//组件名字
	#else
		#define MEMORY_POOL_DLL_NAME				"MemoryPoolD.dll"			//组件名字
	#endif
#endif

#ifndef MEMORY_POOL_DLL
//导出接口
//#include <BaseCode/BaseCode.inl>
#include <PoolSrc/UseMemPool.h>
#include <PoolSrc/MemPoolModule.h>
#include <PoolSrc/MemPoolSwich.hpp>


#endif

#endif