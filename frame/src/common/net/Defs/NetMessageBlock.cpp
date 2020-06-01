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
 * @file  : NetMessageBlock.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/04
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_NetMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(SessionConnectedNetMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostConnectionFailureToDispatcherMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostAsynConnectionToConnectorMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(SessionDisconnectedMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)

#ifdef _WIN32
OBJ_POOL_CREATE_DEF_IMPL(IocpMsgArrivedMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)

#else
OBJ_POOL_CREATE_DEF_IMPL(EpollEvArriveMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvEpollInEvMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollEvEpollOutEvMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostRecvMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostSendMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollRecvDataMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(EpollSendSucMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostCancelIoToRecverMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)
OBJ_POOL_CREATE_DEF_IMPL(PostCancelIoToSenderMsgBlock, __DEF_OBJ_POOL_OBJ_NUM__)

#endif

FS_String SessionConnectedNetMsgBlock::ToString()
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
        .AppendFormat("_connectorCompId[%u]\n", _connectorCompId)
        .AppendFormat("addrinfo[%s:%hu]\n", addr, port);

    return info;
}


FS_NAMESPACE_END

