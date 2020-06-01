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
 * @file  : IFS_AcceptorPoller.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/18
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_AcceptorPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_AcceptorPoller_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/objpool/objpool.h"

struct sockaddr_in;
FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class ConcurrentMessageQueueNoThread;
class IFS_EngineComp;
class IFS_NetEngine;

class BASE_EXPORT IFS_AcceptorPoller : public IFS_BasePoller
{
    OBJ_POOL_CREATE_DEF(IFS_AcceptorPoller);
public:
    IFS_AcceptorPoller(IFS_EngineComp *engineComp, Int32 monitorType);
    ~IFS_AcceptorPoller();

public:
    virtual void AttachAcceptorParam(SOCKET sock
                                     , Int32 maxSessionQuantityLimit
                                     , Locker *locker
                                     , Int32 *curSessionCnt
                                     , UInt64 *curMaxSessionId);

    virtual bool OnSessionDisconnected(UInt64 sessionId);

    /* 网络事件 */
protected:
    void _HandleSessionWillConnect(IFS_NetEngine *netEngine, SOCKET sock, sockaddr_in *addrInfo);

protected:
    // 客户端连接上限
    SOCKET _bindAcceptorSock;   // 外部释放内部不需要释放
    Locker *_locker;
    Int32 *_curSessionCnt;
    Int32 _maxSessionQuantityLimit;
    UInt64 *_curMaxSessionId;
    std::set<UInt64> _sucConnectedSessionIds;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_AcceptorPollerImpl.h"

#endif
