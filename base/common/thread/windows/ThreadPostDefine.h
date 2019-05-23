#ifndef THREAD_POST_DEFINE_H_
#define THREAD_POST_DEFINE_H_

#pragma once

#include <TDPoster/ThreadPostHelper.hpp>
#include<TDPoster/ThreadPostInterfaceClass.hpp>

//若需要postthread只需要在此定义接口类，以及相应的接口函数 和枚举
DECLEAR_GENERAL_THREAD_POST_BEGIN()
GENERAL_ADDTHREAD_POST(IMainThread, HandleRecvMsg, POST_OBJ_TYPE_MAIN_THREAD)
GENERAL_ADDTHREAD_POST(INetworkSenderThread, HandleRecvMsg, POST_OBJ_TYPE_NETWORK_SENDER)
DECLEAR_GENERAL_THREAD_POST_END()







#endif

