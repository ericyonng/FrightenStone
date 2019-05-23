/**
* @file SocketUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/24
* @brief
*/
#include "stdafx.h"
#include "base/common/socket/Impl/SocketUtil.h"
#include <base/common/status/status.h>

#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <memory.h>

FS_NAMESPACE_BEGIN

bool SocketUtil::_isInitEnv = false;

int SocketUtil::InitSocketEnv()
{
    if(_isInitEnv)
        return StatusDefs::Success;

    WSADATA wsaData;
    memset(&wsaData, 0, sizeof(WSADATA));
    auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return StatusDefs::Error;
    }

    if(result != NO_ERROR)
        return StatusDefs::Error;

    _isInitEnv = true;
    return StatusDefs::Success;
}

int SocketUtil::ClearSocketEnv()
{
    int result = NO_ERROR;
    if(!_isInitEnv)
        return StatusDefs::Success;

    result = WSACleanup();
//     if(result == SOCKET_ERROR) 
//     {
//         if(WSACancelBlockingCall() == 0) //winsock 1.0
//             result = WSACleanup();
//     }

    if(result != NO_ERROR)
        return StatusDefs::Error;

    _isInitEnv = false;
    return StatusDefs::Success;
}

bool SocketUtil::SetNoBlock(MYSOCKET socket)
{
    ULong ul = 1;
    if(0 != ioctlsocket(socket, FIONBIO, &ul))
        return false;

    return true;
}

bool SocketUtil::SetBlock(MYSOCKET socket)
{
    ULong ul = 0;
    if(0 != ioctlsocket(socket, FIONBIO, &ul))
        return false;

    return true;
}

FS_NAMESPACE_END
