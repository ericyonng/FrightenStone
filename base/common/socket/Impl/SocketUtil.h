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
 * @file  : SocketUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Socket_Impl_SocketUtil_H__
#define __Base_Common_Socket_Impl_SocketUtil_H__

#pragma once

#include <base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/net/net.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT SocketUtil
{
public:
    static int InitSocketEnv();
    static int ClearSocketEnv();
    static bool SetNoBlock(MYSOCKET socket);
    static bool SetBlock(MYSOCKET socket);

    //获取对象地址信息 0表示成功其他值为错误信息
    static Int32 GetPeerAddr(UInt64 sSocket, P_OUT char *&ip, const UInt64 szIp, P_OUT UInt16& port);
    //转换为网络字节序
    static bool FillTcpAddrInfo(P_OUT SOCKADDR_IN& AddrObj, P_IN const char *ip, P_IN  const U16 port, P_IN  const U16 family);

    //转换为主机信息
    static bool GetAddrInfoFromNetInfo(P_IN const SOCKADDR_IN& AddrObj, P_OUT char *& ip, P_OUTSIZE const U64 szip, P_OUT U16& port);

    static bool IsDetectTimeOut(SOCKET& rSocket, fd_set&rReadableSet, fd_set&rWritableSet, long tv_sec, long tv_usec, bool bReadableDetect = true, bool bWriteableDetect = false, int *pErrOut = NULL, bool bSetOneAtLeast = false, bool bInfiniteWaiting = false);

    //设置socket缓冲区大小
    static bool SetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, const I64 nSize);

    //获取socket缓冲区大小
    static bool GetSocketCacheSize(SOCKET& rSocket, SOCKET_CACHE_TYPE eType, I64& nSize);

private:
    static bool _isInitEnv;
};

FS_NAMESPACE_END

#endif // !__Base_Common_Socket_Impl_SocketUtil_H__
