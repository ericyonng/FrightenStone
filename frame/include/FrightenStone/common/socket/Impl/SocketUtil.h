/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : SocketUtil.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Socket_Impl_SocketUtil_H__
#define __Frame_Include_FrightenStone_Common_Socket_Impl_SocketUtil_H__

#pragma once

#include <FrightenStone/exportbase.h>
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/net/net.h"
#include "FrightenStone/common/socket/Defs/SocketDefs.h"

// struct sockaddr_in;

FS_NAMESPACE_BEGIN

class BASE_EXPORT SocketUtil
{
public:
    static int InitSocketEnv();
    static int ClearSocketEnv();
    static Int32 SetNoBlock(SOCKET socket);
    static Int32 SetBlock(SOCKET socket);
    static Int32 MakeReUseAddr(SOCKET socket);
    static Int32 MakeNoDelay(SOCKET socket);
    static Int32 DestroySocket(SOCKET &socket);
    static Int32 DestroySocket2(SOCKET socket);
    // 获取对象地址信息 0表示成功其他值为错误信息 获取远程客户端地址信息 需要在accept成功完成后才可以调用 windows 下lasterror是WSAGetLastError获得,linux下lasterror是errno获得
    static Int32 GetPeerAddr(UInt64 sSocket, Int32 sizeIp, Byte8 *&ip, UInt16 &port, Int32 &lastError);
    // 对监听端口而言只有bind之后才可以调用,windows 下lasterror是WSAGetLastError获得,linux下lasterror是errno获得
    static Int32 GetSockName(SOCKET sock, sockaddr_in &addrInfo, Int32 &lastError);
    // 转换为网络字节序 family:AF_INET(udp,tcp)
    static bool FillTcpAddrInfo(const char *ip, UInt16 port, UInt16 family, sockaddr_in &addrObj);
    // 转换为主机信息
    static bool GetAddrInfoFromNetInfo(sockaddr_in &addrObj, UInt64 szip, char *&ip, UInt16 &port);
    // 套接字等待超时
    static bool IsDetectTimeOut(
        SOCKET &socket
        , timeval timeout
        , bool enableReadableDetect = true
        , bool enableWriteableDetect = false
        , int *errOut = NULL
        , bool isInfiniteWaiting = false);
    // 睡眠挂起
    static void Sleep(UInt64 milliSec, UInt64 microSec = 0);
    // 设置socket缓冲区大小
    static Int32 SetSocketCacheSize(SOCKET &socket, SocketDefs::SOCKET_CACHE_TYPE eType, Int64 cacheSize, Int32 &lastErr);
    // 获取socket缓冲区大小
    static Int32 GetSocketCacheSize(SOCKET &socket, SocketDefs::SOCKET_CACHE_TYPE eType, Int64 &cacheSize, Int32 &lastErr);

    // 网络速度计
    static FS_String ToFmtSpeedPerSec(Int64 bytesps);
    // 是否无效套接字
    static bool IsValidSock(SOCKET sock);

    // 套接字api函数
#pragma region socket api
    static SOCKET CreateSock(Int32 protoFamily = AF_INET    // ipv4 default
                             , Int32 type = SOCK_STREAM     // 默认字节流, udp是 SOCK_DGRAM 报文
                             , Int32 protocol = IPPROTO_TCP // 默认tcp协议
    );
    static Int32 Bind(SOCKET sock, const BriefAddrInfo &briefAddr
                      , Int32 protoFamily = AF_INET     // ipv4 default
    );
    static Int32 Listen(SOCKET sock, Int32 unconnectQueueLen = SOMAXCONN);   // 默认排队数量

#pragma endregion

private:
    static bool _isInitEnv;
};

FS_NAMESPACE_END

#endif // !__Frame_Include_FrightenStone_Common_Socket_Impl_SocketUtil_H__
