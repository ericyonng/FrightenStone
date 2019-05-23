#ifndef HEAP_EX_API_H_
#define HEAP_EX_API_H_

#pragma once


//导出定义
#ifndef HEAP_EX_API
#ifdef  HEAP_EX_DLL
#define HEAP_EX_API _declspec(dllexport)
#else
#define HEAP_EX_API _declspec(dllimport)
#endif
#endif

//定义文件名
#ifndef _DEBUG

#ifdef _UNICODE
#define HEAP_DLL_NAME				TEXT("HeapEx.dll")		//组件名字
#else
#define HEAP_DLL_NAME				"HeapEx.dll"				//组件名字
#endif

#else
#ifdef _UNICODE
#define HEAP_DLL_NAME				TEXT("HeapExD.dll")		//组件名字
#else
#define HEAP_DLL_NAME				"HeapExD.dll"			//组件名字
#endif
#endif

#ifndef HEAP_EX_DLL
#include <Heap/HeapMgr.h>
#endif


#endif

