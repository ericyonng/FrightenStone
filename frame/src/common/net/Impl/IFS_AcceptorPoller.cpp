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
 * @file  : IFS_AcceptorPoller.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/18
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_AcceptorPoller.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(IFS_AcceptorPoller, 32);

bool IFS_AcceptorPoller::OnSessionDisconnected(UInt64 sessionId)
{
    _locker->Lock();
    bool isOwner = false;
    auto iterSessinId = _sucConnectedSessionIds.find(sessionId);
    if(iterSessinId != _sucConnectedSessionIds.end())
    {
        --*_curSessionCnt;
        _sucConnectedSessionIds.erase(iterSessinId);
        isOwner = true;
    }

    _locker->Unlock();

    return isOwner;
}

void IFS_AcceptorPoller::_HandleSessionWillConnect(IFS_NetEngine *netEngine, SOCKET sock, sockaddr_in *addrInfo)
{
    if(INVALID_SOCKET == sock)
    {
        g_Log->e<IFS_AcceptorPoller>(_LOGFMT_("IFS_BasePoller sock invalid socket"));
        return;
    }

    _locker->Lock();
    if(*_curSessionCnt < _maxSessionQuantityLimit)
    {
        ++*_curSessionCnt;
        auto curMaxSession = ++*_curMaxSessionId;
        _sucConnectedSessionIds.insert(curMaxSession);
        _locker->Unlock();

        // socket重用
        SocketUtil::MakeReUseAddr(sock);
        // 设置非阻塞
        SocketUtil::SetNoBlock(sock);

        // 新会话连入
        BriefSessionInfo newSessionInfo;
        newSessionInfo._addrInfo = addrInfo;
        newSessionInfo._sessionId = curMaxSession;
        newSessionInfo._sock = sock;
        newSessionInfo._acceptorCompId = _engineComp->GetCompId();
        newSessionInfo._srcCompType = _engineComp->GetCompType();
        newSessionInfo._protocolPort = _engineComp->CastTo<IFS_Acceptor>()->GetListenPort();
#ifndef _WIN32
        g_Log->i<IFS_AcceptorPoller>(_LOGFMT_("new session will post to dispatcher sessionId[%llu]"), curMaxSession);
#endif
        if(!netEngine->HandleCompEv_WillConnect(&newSessionInfo))
        {
            _locker->Lock();
            --(*_curSessionCnt);
            _sucConnectedSessionIds.erase(curMaxSession);
            SocketUtil::DestroySocket2(sock);
            _locker->Unlock();
        }
    }
    else {
        _locker->Unlock();

        // 获取IP地址 inet_ntoa(clientAddr.sin_addr)
        BUFFER256 ip;
        ip[0] = 0;
        Byte8 *ptr = ip;
        UInt16 port = 0;
        SocketUtil::GetAddrInfoFromNetInfo(*addrInfo, sizeof(ip), ptr, port);
        SocketUtil::DestroySocket2(sock);
        g_Log->w<IFS_AcceptorPoller>(_LOGFMT_("reach session quantity limit[%d] %s:%hu"), _maxSessionQuantityLimit, ip, port);
    }
}

FS_NAMESPACE_END
