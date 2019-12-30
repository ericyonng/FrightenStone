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
 * @file  : FS_IocpPoller.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/29
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpPoller_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

#include "FrightenStone/common/component/Impl/FS_Delegate.h"

#ifdef _WIN32

struct sockaddr_in;
FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class FS_Iocp;
struct IoEvent;
class ConcurrentMessageQueueNoThread;
class IFS_EngineComp;
class IFS_NetEngine;

class BASE_EXPORT FS_IocpPoller : public IFS_BasePoller
{
public:
    FS_IocpPoller(IFS_EngineComp *engineComp, Int32 pollerSubType);
    ~FS_IocpPoller();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();

    virtual void AttachMessageQueue(ConcurrentMessageQueueNoThread *mq);
    void AttachAcceptorParam(SOCKET sock
                             , Int32 maxSessionQuantityLimit
                             , Locker *locker
                             , Int32 *curSessionCnt
                             , UInt64 *curMaxSessionId);

    virtual void OnAcceptorDisconnected(UInt64 sessionId);

private:
    void _TransferMonitor(FS_ThreadPool *pool);
    void _AcceptorMonitor(FS_ThreadPool *pool);
    void _HandleSessionWillConnect(IFS_NetEngine *netEngine, SOCKET sock, const sockaddr_in *addrInfo);

private:
    IFS_EngineComp *_engineComp;
    Int32 _mainType;
    Int32 _monitorType;
    SOCKET _acceptorSock;
    FS_ThreadPool *_pool;
    FS_Iocp *_iocp;
    IoEvent *_ioEv;
    IDelegate<void> *_quitIocpMonitor;      // 当poller是acceptor时用到
    ConcurrentMessageQueueNoThread *_mq;
    std::atomic_bool _isInit;

    // acceptor客户端连接上限
    Locker *_locker;
    Int32 *_curSessionCnt;
    Int32 _maxSessionQuantityLimit;
    UInt64 *_curMaxSessionId;
    std::set<UInt64> _sucConnectedSessionIds;

    typedef void (FS_IocpPoller::*MonitorHandler)(FS_ThreadPool *pool);
    static MonitorHandler _monitorHandler[PollerDefs::MonitorType_End];
};

FS_NAMESPACE_END
#endif

#include "FrightenStone/common/net/Impl/FS_IocpPollerImpl.h"

#endif
