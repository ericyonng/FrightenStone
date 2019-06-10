/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : SocketUtil.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/socket/Impl/SocketUtil.h"
#include <base/common/status/status.h>

#pragma region windows
#include<WinSock2.h>
#include "ws2tcpip.h"
#pragma comment(lib,"ws2_32.lib")
#pragma endregion

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

Int32 SocketUtil::GetPeerAddr(UInt64 sSocket, P_OUT char *&ip, const UInt64 szIp, P_OUT UInt16& port)
{
    if(!sSocket || sSocket == INVALID_SOCKET || !szIp)
        return StatusDefs::Socket_ParamError;

    //¶¨Òå
    struct sockaddr_in dstadd_in;
    memset(&dstadd_in, 0, sizeof(dstadd_in));
    socklen_t  len = sizeof(dstadd_in);

    if(getpeername(sSocket, (struct sockaddr*)&dstadd_in, &len) >= 0)
        return WSAGetLastError();

    if(inet_ntop(dstadd_in.sin_family, &dstadd_in.sin_addr.s_addr, ip, szIp) == NULL)
        return StatusDefs::ParamError;

    port = ntohs(dstadd_in.sin_port);

    return StatusDefs::Success;
}

FS_NAMESPACE_END
