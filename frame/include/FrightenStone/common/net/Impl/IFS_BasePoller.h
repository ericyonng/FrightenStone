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
 * @file  : IFS_BasePoller.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/28
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BasePoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_BasePoller_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class ConcurrentMessageQueueNoThread;
class IFS_Session;
class IFS_NetModule;
class IFS_NetEngine;
class IFS_EngineComp;

class BASE_EXPORT IFS_BasePoller
{
public:
    IFS_BasePoller(IFS_EngineComp *engineComp, Int32 monitorType);
    virtual ~IFS_BasePoller();

public:
    virtual Int32 BeforeStart() { return StatusDefs::Success; }
    virtual Int32 Start() = 0;
    virtual void AfterStart() {}
    virtual void WillClose() {}
    virtual void BeforeClose() {}
    virtual void Close() = 0;
    virtual void AfterClose() {}

    virtual void AttachMessageQueue(ConcurrentMessageQueueNoThread *mq);
    virtual void AttachAcceptorParam(SOCKET sock
                                     , Int32 maxSessionQuantityLimit
                                     , Locker *locker
                                     , Int32 *curSessionCnt
                                     , UInt64 *curMaxSessionId);

    virtual void OnAcceptorDisconnected(UInt64 sessionId);

    // 网络模型iocp,epoll
    virtual IFS_NetModule *GetNetModuleObj() = 0;

    FS_String ToString() const;

    /* 网络事件 */
protected:
    void _HandleSessionWillConnect(IFS_NetEngine *netEngine, SOCKET sock, const sockaddr_in *addrInfo);

protected:
    IFS_EngineComp *_engineComp;
    Int32 _monitorType;
    SOCKET _acceptorSockFd;
    ConcurrentMessageQueueNoThread *_mq;

    // acceptor客户端连接上限
    Locker *_locker;
    Int32 *_curSessionCnt;
    Int32 _maxSessionQuantityLimit;
    UInt64 *_curMaxSessionId;
    std::set<UInt64> _sucConnectedSessionIds;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_BasePollerImpl.h"
#endif
