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
 * @file  : FS_EpollAcceptPoller.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/4/20
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollAcceptPoller.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Defs/FS_EpollSessionFlag.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
FS_NAMESPACE_BEGIN


Int32 FS_EpollAcceptPoller::BeforeStart()
{
    if (_isInit)
        return StatusDefs::Success;

    _pool = new FS_ThreadPool(0, 2);
    _epoll = new FS_Epoll;

    Int32 st = _epoll->Create();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollAcceptPoller>(_LOGFMT_("create epoll fail st[%d]"), st);
        return st;
    }

    st = _epoll->AddEvent(_bindAcceptorSock, __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__);
    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("acceptor epoll poll add event acceptorSockFd[%d], events[%x] evfd[%d] st[%d]")
        , _bindAcceptorSock, __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__, _epoll->GetEvFd(), st);
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollAcceptPoller>(_LOGFMT_("add event accept fd[%d] fail st[%d]"), _bindAcceptorSock, st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollAcceptPoller::Start()
{
    if (_isInit)
        return StatusDefs::Success;

    return StatusDefs::Success;
}

void FS_EpollAcceptPoller::AfterStart()
{
    if (_isInit)
        return;

    // 先启动消息预处理线程
    auto preHandleTask = DelegatePlusFactory::Create(this, &FS_EpollAcceptPoller::_OnPreHandleAcceptEvPoller);
    if (!_pool->AddTask(preHandleTask, true))
    {
        g_Log->e<FS_EpollAcceptPoller>(_LOGFMT_("add preHandleTask fail _monitorType[%d]"), _monitorType);
    }

    // 再启动事件触发线程
    auto task = DelegatePlusFactory::Create(this, &FS_EpollAcceptPoller::_OnAcceptorEvMonitorPoller);
    if (!_pool->AddTask(task, true))
    {
        g_Log->e<FS_EpollAcceptPoller>(_LOGFMT_("addtask _OnAcceptorEvMonitorPoller fail _monitorType[%d]"), _monitorType);
    }

    _isInit = true;
}

void FS_EpollAcceptPoller::WillClose()
{
    if (!_isInit)
        return;

    _isWillClose = true;
    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("acceptor poller [%s] will close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollAcceptPoller::BeforeClose()
{
    if (!_isInit)
        return;

    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("epoll poller [%s] before close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    if (!_epoll->IsDestroy())
        _epoll->Destroy();

    _preHandlerGuard.Broadcast();

    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("epoll poller [%s] before close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollAcceptPoller::Close()
{
    if (!_isInit)
        return;

    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("epoll poller [%s] Closing..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    _pool->Close();
    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("_pool close suc [%s]..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    Fs_SafeFree(_pool);
    Fs_SafeFree(_epoll);

    _isInit = false;

    g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("epoll poller [%s] close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

IFS_NetModule *FS_EpollAcceptPoller::GetNetModuleObj()
{
    return _epoll;
}

void FS_EpollAcceptPoller::_OnAcceptorEvMonitorPoller(FS_ThreadPool *pool)
{
    Int32 ret = 0;
    Int32 sockFd = INVALID_SOCKET;
    struct sockaddr_in inAddr;
    epoll_event *evs = NULL;
    Int32 loopPos = 0;

    // 等待其他组件ready
    const UInt32 compId = _engineComp->GetCompId();
    auto engine = _engineComp->GetEngine();
    _engineComp->MaskReady(true);
    g_Log->sys<FS_EpollAcceptPoller>(_LOGFMT_("epoll acceptor monitor working..."));
    EngineCompsMethods::WaitForAllCompsReady(engine);

    while (pool->IsPoolWorking())
    {
        ret = _epoll->Wait(1000);
        if (_epoll->IsDestroy())
            break;

        if (ret == -1)
        {
            g_Log->w<FS_EpollAcceptPoller>(_LOGFMT_("epoll wait fail ret[%d] errorno[%d:%s]"), ret, errno, strerror(errno));
        }

        if (ret <= 0)
            continue;

        evs = _epoll->GetEvs();
        g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_(" epoll ev: [%d] new socket will connected"), ret);
        // prehandle
        for (loopPos = 0; loopPos < ret; ++loopPos)
        {
            if (evs[loopPos].data.fd == _bindAcceptorSock && evs[loopPos].events & EPOLLIN)
            {
                _preHandlerGuard.Lock();
                _isEventArrived = true;
                _preHandlerGuard.Sinal();
                _preHandlerGuard.Unlock();
                break;
            }
        }
    }

    // 结束唤醒
    _preHandlerGuard.Lock();
    _preHandlerGuard.Sinal();
    _preHandlerGuard.Unlock();

    _engineComp->MaskReady(false);
    g_Log->sys<FS_EpollAcceptPoller>(_LOGFMT_("acceptor compId[%u]  threadId[%llu] end")
        , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollAcceptPoller::_OnPreHandleAcceptEvPoller(FS_ThreadPool *pool)
{
    Int32 sockFd = INVALID_SOCKET;
    struct sockaddr_in inAddr;
    socklen_t addrSize = static_cast<socklen_t>(sizeof(inAddr));
    auto netEngine = _engineComp->GetEngine();

    const UInt32 compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollAcceptPoller>(_LOGFMT_("epoll acceptor prehandle poller working..."));
    while (pool->IsPoolWorking())
    {
        _preHandlerGuard.Lock();
        if (_isEventArrived)
        {
            // 重置
            _isEventArrived = false;
            _preHandlerGuard.Unlock();

            do
            {
                // accept 直到eagain
                sockFd = ::accept(_bindAcceptorSock, reinterpret_cast<sockaddr *>(&inAddr), &addrSize);
                // g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("accept a new connect acceptorSock[%d] new sock[%d]"), _bindAcceptorSock, sockFd);
                if (sockFd > 0)
                {
                    // 建立连接
                    _HandleSessionWillConnect(netEngine, sockFd, &inAddr);
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("EAGAIN or EWOULDBLOCK coming when accept in a dead loop]"));
                        break;
                    }
                }
            } while (true);

            // 再次判断有没有事件到来
            continue;
        }

        // 判断是否退出
        if (!pool->IsPoolWorking() || _epoll->IsDestroy())
        {
            _preHandlerGuard.Unlock();
            break;
        }

        // 没有事件要处理等待下次事件的到来
        // g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("wait for new connect in the future"));
        _preHandlerGuard.DeadWait();
        // g_Log->i<FS_EpollAcceptPoller>(_LOGFMT_("new connect is coming"));
        _preHandlerGuard.Unlock();
    }

    g_Log->sys<FS_EpollAcceptPoller>(_LOGFMT_("epoll acceptor prehandle poller finished compId[%u]  threadId[%llu] end")
        , compId, SystemUtil::GetCurrentThreadId());
}

FS_NAMESPACE_END

#endif
