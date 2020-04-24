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
 * @file  : FS_EpollEvMessageBlock.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/06
 * @brief :
 */

#include "stdafx.h"
#include "FrightenStone/common/net/Defs/FS_EpollEvMessageBlock.h"
#include "FrightenStone/common/net/Impl/FS_EpollPoller.h"

#include "FrightenStone/common/socket/socket.h"

#ifndef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_EpollEvMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvArriveMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvSessionWillConnectMsg, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvEpollInEvMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvEpollOutEvMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollRecvDataMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollSendSucMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostRecvMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DEF_IMPL(PostSendMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DEF_IMPL(PostCancelIoToRecverMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_DEF_IMPL(PostCancelIoToSenderMessageBlock, __DEF_OBJ_POOL_OBJ_NUM__);


FS_String EpollEvSessionWillConnectMsg::ToString()
{
    // 获取地址信息
    BUFFER256 addr = {};
    char *ptrAddr = addr;
    UInt16 port = 0;
    SocketUtil::GetAddrInfoFromNetInfo(_addrInfo, sizeof(addr), ptrAddr, port);

    FS_String info;
    info.AppendFormat("_sessionId[%llu]\n", _sessionId)
        .AppendFormat("_sock[%d]\n", _sock)
        .AppendFormat("_acceptorCompId[%u]\n", _acceptorCompId)
        .AppendFormat("epoll poller info:[%s]\n", _poller->ToString().c_str())
        .AppendFormat("addrinfo[%s:%hu]\n", addr, port);

    return info;
}

FS_NAMESPACE_END

#endif
