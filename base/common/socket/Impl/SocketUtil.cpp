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
