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
#include "FrightenStone/common/socket/Impl/SocketUtil.h"
#include <FrightenStone/common/status/status.h>
#include "FrightenStone/common/component/Impl/Time.h"

#ifdef _WIN32
#pragma region windows
// #include<WinSock2.h>
// #include <ws2def.h>
// #include "ws2tcpip.h"
// #pragma comment(lib,"ws2_32.lib")
#pragma endregion
#endif

#ifndef _WIN32
#include<fcntl.h>
#include<stdlib.h>
#endif // !_WIN32

#include <memory.h>

FS_NAMESPACE_BEGIN

bool SocketUtil::_isInitEnv = false;

int SocketUtil::InitSocketEnv()
{
    if(_isInitEnv)
        return StatusDefs::Success;

#ifdef _WIN32
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
#endif

#ifndef _WIN32
    // if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    //  return (1);
    // 忽略异常信号，默认情况会导致进程终止
    // TODO:Linux下
    signal(SIGPIPE, SIG_IGN);
#endif

    _isInitEnv = true;
    return StatusDefs::Success;
}

int SocketUtil::ClearSocketEnv()
{
    if(!_isInitEnv)
        return StatusDefs::Success;

#ifdef _WIN32
    int result = NO_ERROR;
    result = WSACleanup();
//     if(result == SOCKET_ERROR) 
//     {
//         if(WSACancelBlockingCall() == 0) //winsock 1.0
//             result = WSACleanup();
//     }

    if(result != NO_ERROR)
        return StatusDefs::Error;
#endif

    _isInitEnv = false;
    return StatusDefs::Success;
}

Int32 SocketUtil::SetNoBlock(MYSOCKET socket)
{
#ifdef _WIN32
    ULong ul = 1;
    if(SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &ul))
        return StatusDefs::Socket_SetNoBlockParamError;
#else
    int flags;
    if((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        // CELLLog_Warring("fcntl(%d, F_GETFL)", fd);
        return StatusDefs::Socket_SetNoBlockParamError;
    }

    if(!(flags & O_NONBLOCK)) {
        if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            // CELLLog_Warring("fcntl(%d, F_SETFL)", fd);
            return StatusDefs::Socket_SetNoBlockParamError;
        }
    }
#endif

    return StatusDefs::Success;
}

Int32 SocketUtil::SetBlock(MYSOCKET socket)
{
#ifdef _WIN32
    ULong ul = 0;
    if(0 != ioctlsocket(socket, FIONBIO, &ul))
        return StatusDefs::Socket_SetBlockParamError;

    return StatusDefs::Success;
#else
    int flags;
    if((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        // CELLLog_Warring("fcntl(%d, F_GETFL)", fd);
        return StatusDefs::Socket_SetBlockParamError;
    }

    flags &= ~O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) == -1) {
        // CELLLog_Warring("fcntl(%d, F_SETFL)", fd);
        return StatusDefs::Socket_SetBlockParamError;
    }
#endif

}


Int32 SocketUtil::MakeReUseAddr(MYSOCKET socket)
{
    int flag = 1;
    if(SOCKET_ERROR == ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag))) {
        // CELLLog_Warring("setsockopt socket<%d> SO_REUSEADDR failed", (int)fd);
        return StatusDefs::Socket_Error;
    }

    return StatusDefs::Success;
}

Int32 SocketUtil::MakeNoDelay(MYSOCKET socket)
{
    int flag = 1;
    if(SOCKET_ERROR == ::setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&flag, sizeof(flag))) {
        // CELLLog_Warring("setsockopt socket<%d> IPPROTO_TCP TCP_NODELAY failed", (int)fd);
        return StatusDefs::Socket_Error;
    }

    return StatusDefs::Success;
}

Int32 SocketUtil::DestroySocket(MYSOCKET &socket)
{
#ifdef _WIN32
    int ret = closesocket(socket);
#else
    int ret = close(socket);
#endif

    if(ret == 0)
        socket = INVALID_SOCKET;

    // if(ret < 0)
        // CELLLog_PError("destory sockfd<%d>", (int)sockfd);
    return ret == 0 ? StatusDefs::Success : StatusDefs::Socket_Error;
}

Int32 SocketUtil::DestroySocket2(MYSOCKET socket)
{
#ifdef _WIN32
    int ret = closesocket(socket);
#else
    int ret = close(socket);
#endif

    if(ret == 0)
        socket = INVALID_SOCKET;

    // if(ret < 0)
        // CELLLog_PError("destory sockfd<%d>", (int)sockfd);
    return ret == 0 ? StatusDefs::Success : StatusDefs::Socket_Error;
}

Int32 SocketUtil::GetPeerAddr(UInt64 sSocket, Int32 sizeIp, Byte8 *&ip, UInt16 &port, Int32 &lastError)
{
    if(!sSocket || sSocket == INVALID_SOCKET)
        return StatusDefs::Socket_ParamError;

    //定义
    struct sockaddr_in dstadd_in;
    memset(&dstadd_in, 0, sizeof(dstadd_in));
    socklen_t  len = sizeof(dstadd_in);

    // 获取客户端地址
    if(getpeername(sSocket, (struct sockaddr*)&dstadd_in, &len) != 0)
    {
        lastError = WSAGetLastError();
        return StatusDefs::FS_IPUtil_GetPeerNameFailed;
    }

    if(inet_ntop(dstadd_in.sin_family, &dstadd_in.sin_addr.s_addr, ip, sizeIp) == NULL)
        return -1;

    port = ntohs(dstadd_in.sin_port);

    return StatusDefs::Success;
}

bool SocketUtil::FillTcpAddrInfo(const char *ip, UInt16 port, UInt16 family, sockaddr_in &addrObj)
{
    if(!ip || strlen(ip) == 0)
        return false;

    memset(&addrObj, 0, sizeof(addrObj));
    addrObj.sin_family = family;
    if(inet_pton(addrObj.sin_family, ip, &addrObj.sin_addr.s_addr) != 1)
        return false;

    addrObj.sin_port = htons(port);

    return true;
}

bool SocketUtil::GetAddrInfoFromNetInfo(const sockaddr_in &addrObj, UInt64 szip, char *&ip, UInt16 &port)
{
    if(inet_ntop(addrObj.sin_family, const_cast<ULong *>(&addrObj.sin_addr.s_addr), ip, szip) == NULL)
        return false;

    port = ntohs(addrObj.sin_port);

    return true;
}

bool SocketUtil::IsDetectTimeOut(
    SOCKET &socket
    , fd_set &readableSet
    , fd_set &writableSet
    , long tv_sec
    , long tv_usec
    , bool enableReadableDetect
    , bool enableWriteableDetect
    , int *errOut
    , bool setOneAtLeast
    , bool isInfiniteWaiting)
{
    //清理
    FD_ZERO(&readableSet);
    FD_ZERO(&writableSet);
    FD_SET(socket, &readableSet);
    FD_SET(socket, &writableSet);

    // 超时监控参数初始化 秒/微妙
    timeval timeout;
    timeout.tv_sec = tv_sec;
    timeout.tv_usec = tv_usec;

    int ret = SOCKET_ERROR;
    if(isInfiniteWaiting) { //永久阻塞
        // 0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。 
        // 当返回为正数时，表示已经准备好的描述符数。
        ret = select(static_cast<Int32>(socket + 1), &readableSet, &writableSet, NULL, NULL); 
    }
    else {
        // 0表示超时，否则返回SOCKET_ERROR 当返回为-1时，所有描述符集清0。
        // 当返回为正数时，表示已经准备好的描述符数。
        ret = select(static_cast<Int32>(socket + 1), &readableSet, &writableSet, NULL, &timeout); 
    }

    //出错带出
    if(errOut) 
        *errOut = ret;

    if(ret == SOCKET_ERROR) {
        FD_CLR(socket, &readableSet);
        FD_CLR(socket, &writableSet);
        return true;
    }

    // 监听状态导出
    bool isTimeOut = false;
    if(setOneAtLeast) {
        isTimeOut = (!FD_ISSET(socket, &readableSet) || !FD_ISSET(socket, &writableSet));
    }
    else {
        if(enableReadableDetect && !enableWriteableDetect) {
            isTimeOut = !FD_ISSET(socket, &readableSet);
        }
        else if(!enableReadableDetect && enableWriteableDetect) {
            isTimeOut = !FD_ISSET(socket, &writableSet);
        }
        else {
            isTimeOut = (!FD_ISSET(socket, &readableSet) && !FD_ISSET(socket, &writableSet));
        }
    }

    return isTimeOut;
}

void SocketUtil::Sleep(UInt64 milliSec, UInt64 microSec)
{
    std::chrono::microseconds t(milliSec*Time::_microSecondPerMilliSecond + microSec);
    std::this_thread::sleep_for(t);
}

Int32 SocketUtil::SetSocketCacheSize(SOCKET &socket, SocketDefs::SOCKET_CACHE_TYPE eType, Int64 cacheSize)
{
    if(UNLIKELY(!IS_VALID_SOCKET_IDD(socket)))
        return StatusDefs::Socket_InvalidSocket;

    switch(eType)
    {
        case SocketDefs:: SOCKET_CACHE_TYPE_RECV:
        {
            auto ret = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)(&cacheSize), sizeof(cacheSize));
            Int32 err = StatusDefs::Success;
#ifdef _WIN32
            err = WSAGetLastError();
            if(ret != 0)
                return StatusDefs::Socket_SetSockOptFailed;
#endif
        }
        break;
        case SocketDefs::SOCKET_CACHE_TYPE_SEND:
        {
            auto ret = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)(&cacheSize), sizeof(cacheSize));
#ifdef _WIN32
            auto err = WSAGetLastError();
            if(ret != 0)
                return StatusDefs::Socket_SetSockOptFailed;
#endif
        }
        break;
        default:
        {
            return StatusDefs::Socket_ParamError;
        }
    }

    return StatusDefs::Success;
}

Int32 SocketUtil::GetSocketCacheSize(SOCKET &socket, SocketDefs::SOCKET_CACHE_TYPE eType, Int64 &cacheSize)
{
    if(UNLIKELY(!IS_VALID_SOCKET_IDD(socket)))
        return StatusDefs::Socket_InvalidSocket;

    switch(eType)
    {
        case SocketDefs::SOCKET_CACHE_TYPE_RECV:
        {
            Int32 Len = sizeof(cacheSize);
            auto nRet = getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)(&cacheSize), &Len);
#ifdef _WIN32
            auto nErr = WSAGetLastError();
            if(nRet != 0)
                return StatusDefs::Socket_GetsockoptFailed;
#endif
        }
        break;
        case SocketDefs::SOCKET_CACHE_TYPE_SEND:
        {
            Int32 Len = sizeof(cacheSize);
            auto ret = getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)(&cacheSize), &Len);
#ifdef _WIN32
            auto err = WSAGetLastError();
            if(ret != 0)
                return StatusDefs::Socket_GetsockoptFailed;
#endif
        }
        break;
        default:
        {
            return StatusDefs::Socket_Unknown;
        }
    }

    return StatusDefs::Success;
}

FS_String SocketUtil::ToFmtSpeedPerSec(Int64 bytesps)
{
    // 小于1MB大于1KB的使用KB/s
    // 大于1MB小于1GB的使用MB/s
    // 大于1GB的使用GB/s 
    // 精度保留3位小数
    FS_String info;
    if(bytesps < __FS_DATA_1KB__)
    {// 小于1KB的使用B/s
        info.AppendFormat("%lld B/s", bytesps);
        return info;
    }
    else if(bytesps >= __FS_DATA_1KB__ && bytesps < __FS_DATA_1MB__)
    {
        double speedData = static_cast<double>(bytesps);
        speedData = speedData / __FS_DATA_1KB__;
        info.AppendFormat("%.3lf KB/s", speedData);
        return info;
    }
    else if(bytesps >= __FS_DATA_1MB__ && bytesps < __FS_DATA_1GB__)
    {
        double speedData = static_cast<double>(bytesps);
        speedData = speedData / __FS_DATA_1MB__;
        info.AppendFormat("%.3lf MB/s", speedData);
        return info;
    }
    else if(bytesps >= __FS_DATA_1GB__)
    {
        double speedData = static_cast<double>(bytesps);
        speedData = speedData / __FS_DATA_1GB__;
        info.AppendFormat("%.3lf GB/s", speedData);
        return info;
    }

    return "";
}
FS_NAMESPACE_END

