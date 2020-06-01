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
 * @file  : BriefSessionInfo.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/03/15
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(BriefSessionInfo, __DEF_OBJ_POOL_OBJ_NUM__);

FS_String BriefSessionInfo::ToString() const
{
    // 获取地址信息
    BUFFER256 addr = {};
    char *ptrAddr = addr;
    UInt16 port = 0;
    sockaddr_in addrInfo = *_addrInfo;
    SocketUtil::GetAddrInfoFromNetInfo(addrInfo, sizeof(addr), ptrAddr, port);

    FS_String info;
    info.AppendFormat("_sessionId[%llu]\n", _sessionId)
#ifdef _WIN32
        .AppendFormat("_sock[%llu]\n", _sock)
#else
        .AppendFormat("_sock[%d]\n", _sock)
#endif
        .AppendFormat("_acceptorCompId[%u]\n", _acceptorCompId)
        .AppendFormat("addrinfo[%s:%hu]\n", addr, port);

    return info;
}

FS_NAMESPACE_END
