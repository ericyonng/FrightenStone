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
 * @file  : FS_IocpAcceptPoller.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/17
 * @brief :
 */


#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpAcceptPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpAcceptPoller_H__

#pragma once

#ifdef _WIN32
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"

#include "FrightenStone/common/net/Impl/IFS_AcceptorPoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/objpool/objpool.h"

struct sockaddr_in;
FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class FS_Iocp;
struct IoEvent;
class ConcurrentMessageQueueNoThread;
class IFS_EngineComp;
class IFS_NetEngine;
struct IoDataBase;

class BASE_EXPORT FS_IocpAcceptPoller : public IFS_AcceptorPoller
{
    OBJ_POOL_CREATE_DEF(FS_IocpAcceptPoller);
public:
    FS_IocpAcceptPoller(IFS_EngineComp *engineComp);
    ~FS_IocpAcceptPoller();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();

    // 网络模型iocp,epoll
    virtual IFS_NetModule *GetNetModuleObj();

    virtual bool OnSessionDisconnected(UInt64 sessionId);
    /* 监听 */
private:
    virtual void _AcceptorMonitor(FS_ThreadPool *pool);

protected:
    FS_ThreadPool *_pool;
    FS_Iocp *_iocp;
    IoEvent *_ioEv;
    IDelegate<void> *_quitIocpMonitor;
    std::atomic_bool _isInit;
    Locker _prepareQueueGuard;
    std::list<IoDataBase *> _prepareQueue;      // 预投递accept队列
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_IocpAcceptPollerImpl.h"

#endif

#endif