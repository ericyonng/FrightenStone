/**
* @file InitProcess.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/29
* @brief
*/

#include "pch.h"
#include "TcpPort/Server/Impl/InitProcess.h"
#include "base/exportbase.h"

InitProcess::InitProcess()
{
    _locker.Lock();

    auto st = fs::SocketUtil::InitSocketEnv();
    if(st != StatusDefs::Success)
        printf("InitSocketEnv fail st[%d]", st);

    // 2...

    _locker.Unlock();
}

InitProcess::~InitProcess()
{
    _locker.Lock();
    
    auto st = fs::SocketUtil::ClearSocketEnv();
    if(st != StatusDefs::Success)
        printf("ClearSocketEnv fail st[%d]", st);

    // 2....

    _locker.Unlock();
}
