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
 * @file  : FS_Epoll.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/18
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/log/Log.h"

#include "FrightenStone/common/status/status.h"

#ifndef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_Epoll, 32);

FS_Epoll::FS_Epoll()
    :_ev{static_cast<UInt32>(EpollDefs::InvalidFd)}
    ,_evPtr(_ev)
    , _evFd{EpollDefs::InvalidFd}
{

}

FS_Epoll::~FS_Epoll()
{
    if(!IsDestroy())
        Destroy();
}

Int32 FS_Epoll::Create()
{
    _evFd = ::epoll_create(__FS_MAX_EPOLL_EVENT_NUM_LIMIT__);
    if(_evFd == -1)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("%d = epoll_create(%d) fail"), (Int32)_evFd, __FS_MAX_EPOLL_EVENT_NUM_LIMIT__);
        return StatusDefs::InvalidFd;
    }

    return StatusDefs::Success;
}

Int32 FS_Epoll::Destroy()
{
    Int32 ret = ::close(_evFd);
    if(ret != 0)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("close evfd[%d] fail ret[%d]"), (Int32)_evFd, ret);
        return StatusDefs::Failed;
    }

    _evFd = EpollDefs::InvalidFd;
    return StatusDefs::Success;
}

Int32 FS_Epoll::Ctl(Int32 opType, Int32 fd, epoll_event *ev)
{
    // epoll_ctl 是线程安全的
    Int32 evFd = (Int32)_evFd;
    if(::epoll_ctl(fd, opType, fd, ev) == EpollDefs::EpollError)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("::epoll_ctl fail evFd[%d] optye[%d] fd[%d] ev[%p] eventFlags[%x]")
                           , evFd, opType, fd, ev, ev->events);
        return StatusDefs::Failed;
    }
    
    return StatusDefs::Success;
}

Int32 FS_Epoll::AddEvent(Int32 newFd, UInt64 sessionId, UInt32 eventFlags)
{
    // 关联的事件类型,sessionId
    epoll_event ev;
    ev.events = eventFlags;
    ev.data.u64 = sessionId;

    Int32 fd = _evFd.load();
    if(::epoll_ctl(fd, EPOLL_CTL_ADD, newFd, &ev) == EpollDefs::EpollError)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("::epoll_ctl(evFd[%d], add, newFd[%d], ev(%p) eventFlags[%x]) fail")
                           , fd, newFd, &ev, eventFlags);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

Int32 FS_Epoll::AddEvent(Int32 newFd, UInt32 eventFlags)
{
    // 关联的事件类型,sessionId
    epoll_event ev;
    ev.events = eventFlags;
    ev.data.fd = newFd;

    Int32 fd = _evFd.load();
    if(::epoll_ctl(fd, EPOLL_CTL_ADD, newFd, &ev) == EpollDefs::EpollError)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("::epoll_ctl(evFd[%d], add, newFd[%d], ev(%p) eventFlags[%x]) fail")
                           , fd, newFd, &ev, eventFlags);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

Int32 FS_Epoll::ModifyEvent(Int32 modFd, UInt64 sessionId, UInt32 eventFlags)
{
    // 关联的事件类型,sessionId
    epoll_event ev;
    ev.events = eventFlags;
    ev.data.fd = modFd;

    Int32 fd = _evFd.load();
    if(::epoll_ctl(fd, EPOLL_CTL_MOD, modFd, &ev) == EpollDefs::EpollError)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("::epoll_ctl(evFd[%d], mod, modFd[%d], ev(%p) eventFlags[%x]) fail")
                           , fd, modFd, &ev, eventFlags);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

Int32 FS_Epoll::DelEvent(Int32 delFd, UInt64 sessionId, UInt32 eventFlagsToDel)
{
    epoll_event ev;
    ev.events = eventFlagsToDel;
    ev.data.u64 = sessionId;
    Int32 fd = _evFd.load();
    if(::epoll_ctl(fd, EPOLL_CTL_DEL, delFd, &ev) == EpollDefs::EpollError)
    {
        g_Log->e<FS_Epoll>(_LOGFMT_("::epoll_ctl(evFd[%d], mod, delFd[%d], ev(%p) eventFlags[%x]) fail")
                           , fd, delFd, &ev, __DEL_FS_EPOLL_EVENT_FLAGS_DEF__);
        return StatusDefs::Failed;
    }

    return StatusDefs::Success;
}

Int32 FS_Epoll::Wait(Int32 millisecondsToWait /*= EpollWaitTimeOutType::BlockNoTimeOut*/)
{
    // EINTR 表示中断，需要再次wait
    Int32 ret = 0;
    //g_Log->i<FS_Epoll>(_LOGFMT_("_evFd[%d] epoll wait ev milliseconds to wait[%d]"), _evFd, millisecondsToWait);
    Int32 fd = _evFd.load();
    while((ret = epoll_wait(fd, _ev, __FS_MAX_EVENT_NUM_TO_HANDLE_ONCE_DEF__, millisecondsToWait)) < 0 && errno == EINTR)
    {
        //g_Log->i<FS_Epoll>(_LOGFMT_("_evFd[%d] epoll wait error ret[%d] errno[%d]"), _evFd, ret, errno);
    }

    // g_Log->i<FS_Epoll>(_LOGFMT_("_evFd[%d] epoll wait suc break loop ret[%d] errno[%d]"), _evFd, ret, errno);
    return ret;
}

FS_NAMESPACE_END

#endif
