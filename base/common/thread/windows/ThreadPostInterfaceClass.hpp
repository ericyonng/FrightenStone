#ifndef THREAD_POST_INTERFACE_CLASS_HPP_
#define THREAD_POST_INTERFACE_CLASS_HPP_

#pragma once

//继承与IMainThread 就可以接受RecvMsg
//
#include <BaseCode/TaskBase.h>

#include<TDPoster/ThreadPostApi.h>

#undef DECLEAR_POST_INTERFACE_CLASS
#define DECLEAR_POST_INTERFACE_CLASS(interface_class, fnRecvHandleMsg)\
enum{ _THREAD_POST_INTERFACE##_##interface_class##_##fnRecvHandleMsg##_=__LINE__};\
class THREAD_POST_API interface_class : public CTaskBase	\
{\
public:\
	virtual bool fnRecvHandleMsg(const U64 u64Param, const void *pData, const I64 nSize)=0;\
};

DECLEAR_POST_INTERFACE_CLASS(IMainThread, HandleRecvMsg)
DECLEAR_POST_INTERFACE_CLASS(INetworkSenderThread, HandleRecvMsg)

#endif

