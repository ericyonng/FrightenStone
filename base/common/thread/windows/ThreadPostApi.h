#ifndef THREAD_POST_API_H
#define THREAD_POST_API_H

#pragma once


//导出定义
#ifndef THREAD_POST_API
#ifdef  THREAD_POST_DLL
#define THREAD_POST_API _declspec(dllexport)
#else
#define THREAD_POST_API _declspec(dllimport)
#endif
#endif

//定义文件名
#ifndef _DEBUG

#ifdef _UNICODE
#define THREAD_POST_API_DLL_NAME				TEXT("ThreadPost.dll")		//组件名字
#else
#define THREAD_POST_API_DLL_NAME				"ThreadPost.dll"				//组件名字
#endif

#else
#ifdef _UNICODE
#define THREAD_POST_API_DLL_NAME				TEXT("ThreadPostD.dll")		//组件名字
#else
#define THREAD_POST_API_DLL_NAME				"ThreadPostD.dll"			//组件名字
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef THREAD_POST_DLL







#endif



NAMESPACE_BEGIN(THREAD_TRANSFER)

extern bool THREAD_POST_API InitThreadPostModule();

extern bool THREAD_POST_API FiniThreadPostModule();

//pRecvObj 只能设置一次，设完后不会改变 要保证 pRecvObj 在post生命周期内始终有效，即必须设置为单例全局 pData必须为堆区创建，并最终被智能指针对象持有
extern bool THREAD_POST_API Post2Thread(const void * pRecvObj, POST_OBJ_TYPE eType, const U64 u64Param, const void *pData, const I64 nSize); //线程安全

extern bool THREAD_POST_API IsMsgQueueEmpty(const void * pRecvObj, POST_OBJ_TYPE eThreadType);	//线程安全

extern bool THREAD_POST_API RecverWaitAndHandle(const void *pRecvObj, POST_OBJ_TYPE eThreadType); //线程安全

extern bool THREAD_POST_API SetThreadAwakeStatus(const void *pRecvObj, POST_OBJ_TYPE eThreadType, bool bAwake); //线程安全

extern bool THREAD_POST_API WaitForFini(const void *pRecvObj, POST_OBJ_TYPE eThreadType, const bool bBreakLoopFinal); //线程安全
extern void THREAD_POST_API SetEnablePost(const void *pRecvObj, POST_OBJ_TYPE eThreadType, const bool bEnable); //使能Post


//extern bool THREAD_POST_API SetDisplayFun(const void *pRecvObj, POST_OBJ_TYPE eThreadType, void* DisplayFunPtr);

NAMESPACE_END(THREAD_TRANSFER)

#endif

