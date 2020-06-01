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
 * @file  : FS_EpollAcceptPoller.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/20
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollAcceptPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollAcceptPoller_H__

#pragma once

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/IFS_AcceptorPoller.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class FS_CpuInfo;
class FS_Epoll;
class MessageQueueNoThread;
class IFS_NetModule;
class FS_ThreadPool;

class BASE_EXPORT FS_EpollAcceptPoller : public IFS_AcceptorPoller
{
public:
    FS_EpollAcceptPoller(IFS_EngineComp *engineComp);
    ~FS_EpollAcceptPoller();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();

    // ÍøÂçÄ£ÐÍepoll
    virtual IFS_NetModule *GetNetModuleObj();
    FS_Epoll *GetEpollObj();

private:
    void _OnAcceptorEvMonitorPoller(FS_ThreadPool *pool);
    void _OnPreHandleAcceptEvPoller(FS_ThreadPool *pool);

private:
    FS_ThreadPool *_pool;
    FS_Epoll *_epoll;
    ConditionLocker _preHandlerGuard;
    std::atomic_bool _isInit;
    std::atomic_bool _isWillClose;
    std::atomic_bool _isEventArrived;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_EpollAcceptPollerImpl.h"

#endif

#endif
