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
 * @file  : FS_EpollPoller.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/18
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollPoller.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Defs/FS_EpollEvMessageBlock.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Defs/FS_EpollSessionFlag.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_CpuInfo.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"
#include "FrightenStone/common/assist/utils/Impl/FS_BitUtil.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/component/component.h"

//#ifndef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_EpollPoller, 32);

FS_EpollPoller::MonitorHandler FS_EpollPoller::_monitorHandler[PollerDefs::MonitorType_End] = {
NULL
, &FS_EpollPoller::_OnAcceptorEvMonitorPoller
, &FS_EpollPoller::_OnTransferEvMonitorPoller
};

FS_EpollPoller::MsgPreHandler FS_EpollPoller::_msgPreHandler[PollerDefs::MonitorType_End] = {
NULL
, &FS_EpollPoller::_OnPreHandleAcceptEvPoller
, &FS_EpollPoller::_OnPreDistributeEvPoller
};

FS_EpollPoller::MessageBlockHandler FS_EpollPoller::_msgBlockHandler[FS_EpollEvMessageType::End] = {
  NULL
, NULL
, &FS_EpollPoller::_OnEpollReadableEv
, &FS_EpollPoller::_OnEpollWritableEv
, &FS_EpollPoller::_OnBePostedRecv
, &FS_EpollPoller::_OnBePostedSend                  // 5
, NULL
, NULL
, NULL
, NULL
, &FS_EpollPoller::_OnBePostedCancleIoToRecver
, &FS_EpollPoller::_OnBePostedCancleIoToSender
};

Int32 FS_EpollPoller::BeforeStart()
{
    if(_isInit)
        return StatusDefs::Success;

    _cpuInfo = new FS_CpuInfo;
    if(!_cpuInfo->Initialize())
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("cpu info initialize fail"));
        return StatusDefs::Failed;
    }

    // const Int32 cpuCount = _cpuInfo->GetCpuCoreCnt();
    _maxThreadCnt = 1 + 1;   // 1是给事件触发线程,1是给触发事件后prehandle使用
    if(_monitorType == PollerDefs::MonitorType_Transfer)
        _maxThreadCnt += 2; // transfer 会额外多 send线程与recv线程

    _pool = new FS_ThreadPool(0, _maxThreadCnt);
    _epoll = new FS_Epoll;
    _innerEvThread2MsgPreHandleMq = new MessageQueueNoThread();
    _recverMailBox = new MessageQueueNoThread();
    _senderMailBox = new MessageQueueNoThread();

    Int32 st = _epoll->Create();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("create epoll fail st[%d]"), st);
        return st;
    }

    if(_monitorType == PollerDefs::MonitorType_Acceptor)
    {
        st = _epoll->AddEvent(_acceptorSockFd, __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__);
        g_Log->i<FS_EpollPoller>(_LOGFMT_("acceptor epoll poll add event acceptorSockFd[%d], events[%x] evfd[%d] st[%d]")
                                 , _acceptorSockFd, __ADD_FS_EPOLL_ACCEPT_EVENT_FLAGS_DEF__, _epoll->GetEvFd(), st);
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("add event accept fd[%d] fail st[%d]"), _acceptorSockFd, st);
            return st;
        }
    }

    st = _innerEvThread2MsgPreHandleMq->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("_innerP2PMq BeforeStart fail st[%d]"), st);
        return st;
    }

    st = _innerEvThread2MsgPreHandleMq->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("_innerP2PMq Start fail st[%d]"), st);
        return st;
    }

    if(_monitorType == PollerDefs::MonitorType_Transfer)
    {
        st = _recverMailBox->BeforeStart();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("_recverMailBox BeforeStart fail st[%d]"), st);
            return st;
        }

        st = _senderMailBox->BeforeStart();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("_senderMailBox BeforeStart fail st[%d]"), st);
            return st;
        }

        st = _recverMailBox->Start();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("_recverMailBox Start fail st[%d]"), st);
            return st;
        }

        st = _senderMailBox->Start();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("_senderMailBox Start fail st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_EpollPoller::Start()
{
    if(_isInit)
        return StatusDefs::Success;

    return StatusDefs::Success;
}

void FS_EpollPoller::AfterStart()
{
    if(_isInit)
        return;

    if(_monitorType == PollerDefs::MonitorType_Transfer)
    {
        _generatorId = _engineComp->CastTo<IFS_MsgTransfer>()->GetGeneratorId();

        auto readPoller = DelegatePlusFactory::Create(this, &FS_EpollPoller::_OnReadPoller);
        if(!_pool->AddTask(readPoller, true))
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("add readPoller fail _monitorType[%d]"), _monitorType);
        }

        auto sendPoller = DelegatePlusFactory::Create(this, &FS_EpollPoller::_OnSendPoller);
        if(!_pool->AddTask(sendPoller, true))
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("add sendPoller fail _monitorType[%d]"), _monitorType);
        }
    }

    // 先启动消息预处理线程
    auto preHandleTask = DelegatePlusFactory::Create(this, _msgPreHandler[_monitorType]);
    if(!_pool->AddTask(preHandleTask, true))
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("add preHandleTask fail _monitorType[%d]"), _monitorType);
    }

    // 再启动事件触发线程
    auto task = DelegatePlusFactory::Create(this, _monitorHandler[_monitorType]);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("addtask fail _monitorType[%d]"), _monitorType);
    }

    _isInit = true;
}

void FS_EpollPoller::WillClose()
{
    if(!_isInit)
        return;

    _isWillClose = true;
    g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll poller [%s] will close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollPoller::BeforeClose()
{
    if(!_isInit)
        return;

    g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll poller [%s] before close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
    if(_monitorType == PollerDefs::MonitorType_Transfer)
    {
        _recverMailBox->BeforeClose();
        _senderMailBox->BeforeClose();
    }

    _innerEvThread2MsgPreHandleMq->BeforeClose();

    if(!_epoll->IsDestroy())
        _epoll->Destroy();

    _preHandlerGuard.Broadcast();

    g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll poller [%s] before close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollPoller::Close()
{
    if(!_isInit)
        return;

    g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll poller [%s] Closing..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    _pool->Close();
    g_Log->i<FS_EpollPoller>(_LOGFMT_("_pool close suc [%s]..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    if(_monitorType == PollerDefs::MonitorType_Transfer)
    {
        _recverMailBox->Close();
        _senderMailBox->Close();

    }

    _innerEvThread2MsgPreHandleMq->Close();

    Fs_SafeFree(_pool);
    Fs_SafeFree(_epoll);
    Fs_SafeFree(_innerEvThread2MsgPreHandleMq);
    Fs_SafeFree(_recverMailBox);
    Fs_SafeFree(_senderMailBox);
    Fs_SafeFree(_cpuInfo);

    _isInit = false;

     g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll poller [%s] close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

IFS_NetModule * FS_EpollPoller::GetNetModuleObj()
{
    return _epoll;
}

bool FS_EpollPoller::PushRecvMsg(EpollIoData *ioData)
{
    PostRecvMessageBlock *newMsgBlock = new PostRecvMessageBlock;
    newMsgBlock->_sessionId = ioData->_sessionId;
    newMsgBlock->_ioData = ioData;
    if(!_recverMailBox->Push(newMsgBlock))
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("_recverMailBox push msg fail sessionId[%llu]"), ioData->_sessionId);
        Fs_SafeFree(newMsgBlock);
        return false;
    }

    return true;
}

bool FS_EpollPoller::PushSendMsg(EpollIoData *ioData)
{
    PostSendMessageBlock *newMsgBlock = new PostSendMessageBlock;
    newMsgBlock->_sessionId = ioData->_sessionId;
    newMsgBlock->_ioData = ioData;
    if(!_senderMailBox->Push(newMsgBlock))
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("_senderMailBox push msg fail sessionId[%llu]"), ioData->_sessionId);
        Fs_SafeFree(newMsgBlock);
        return false;
    }

    return true;
}

void FS_EpollPoller::CancelRecvIo(UInt64 sessionId, Int32 reason, bool giveRes)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("will cancel recv io sessionId[%llu] [reason:%s]"), sessionId, CancelIoReason::GetStr(reason).c_str());

    // 只需要分别向recver与sender发送cancel消息，recver与sender直接向dispatcher返回transfer为0的消息即可认定为会话断开
    // TODO:post recver cancel
    _PostCancelToRecver(sessionId, reason, giveRes);
}

void FS_EpollPoller::CancelSendIo(UInt64 sessionId, Int32 reason, bool giveRes)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("will cancel send io sessionId[%llu] [reason:%s]"), sessionId, CancelIoReason::GetStr(reason).c_str());

    // 只需要分别向recver与sender发送cancel消息，recver与sender直接向dispatcher返回transfer为0的消息即可认定为会话断开
    _PostCancelToSender(sessionId, reason, giveRes);
}

bool FS_EpollPoller::OnWillConnect(SOCKET sock, UInt64 sessionId)
{// TRANSFER 与 recver sender之间可以等待，可直接加锁
    // 判断是否连接已创建
    _recvLock.Lock();
    if(_sessionIdRefRecverEvStatus.find(sessionId) != _sessionIdRefRecverEvStatus.end())
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("sock[%d] sessionId[%llu] has already connect or it is not free before"), sock, sessionId);
        _recvLock.Unlock();
        return false;
    }
    _recvLock.Unlock();

    _sendLock.Lock();
    if(_sessionIdRefSenderEvStatus.find(sessionId) != _sessionIdRefSenderEvStatus.end())
    {
        g_Log->e<FS_EpollPoller>(_LOGFMT_("sock[%d] sessionId[%llu] has already connect or it is not free before"), sock, sessionId);
        _sendLock.Unlock();
        return false;
    }
    _sendLock.Unlock();

    _recvLock.Lock();
    _sessionIdRefRecverEvStatus[sessionId] = false;
    _sessionIdRefRecverEvIoData[sessionId] = NULL;
    _recvLock.Unlock();

    _sendLock.Lock();
    _sessionIdRefSenderEvStatus[sessionId] = false;
    _sessionIdRefSenderIoData[sessionId] = NULL;
    _sendLock.Unlock();

    g_Log->i<FS_EpollPoller>(_LOGFMT_(" epoll poller type[%d] new sessionId[%llu] sock[%d] init status")
                   ,_monitorType, sessionId, sock);

    return true;

//     const Int32 st = _epoll->AddEvent(sock, sessionId);
//     if(st != StatusDefs::Success)
//     {
//         g_Log->e<FS_EpollPoller>(_LOGFMT_("_epoll->AddEvent fail epoll poller info[%s] sock[%d] sessionId[%llu] st[%d]")
//                                  , ToString().c_str(), sock, sessionId, st);
//     }
}

bool FS_EpollPoller::IsIoFinish() const
{
    _recvLock.Lock();
    if(!_sessionIdRefRecverEvIoData.empty())
    {
        _recvLock.Unlock();
        return false;
    }
    _recvLock.Unlock();

    _sendLock.Lock();
    if(!_sessionIdRefSenderIoData.empty())
    {
        _sendLock.Unlock();
        return false;
    }
    _sendLock.Unlock();

    return true;
}

void FS_EpollPoller::_OnAcceptorEvMonitorPoller(FS_ThreadPool *pool)
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
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll acceptor monitor working..."));
    EngineCompsMethods::WaitForAllCompsReady(engine);

    while(pool->IsPoolWorking())
    {
         ret = _epoll->Wait(1000);
         if(_epoll->IsDestroy())
             break;

         if(ret == -1)
         {
             g_Log->w<FS_EpollPoller>(_LOGFMT_("epoll wait fail ret[%d] errorno[%d]"), ret, errno);
         }

         if(ret <= 0)
             continue;

         evs = _epoll->GetEvs();
         g_Log->i<FS_EpollPoller>(_LOGFMT_(" epoll ev: [%d] new socket will connected"), ret);
         // prehandle
         for(loopPos = 0; loopPos < ret; ++loopPos)
         {
             if(evs[loopPos].data.fd == _acceptorSockFd && evs[loopPos].events & EPOLLIN)
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
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("acceptor FS_EpollPoller compId[%u]  threadId[%llu] end")
                              , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnPreHandleAcceptEvPoller(FS_ThreadPool *pool)
{
    Int32 sockFd = INVALID_SOCKET;
    struct sockaddr_in inAddr;
    socklen_t addrSize = static_cast<socklen_t>(sizeof(inAddr));
    auto netEngine = _engineComp->GetEngine();

    const UInt32 compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll acceptor prehandle poller working..."));
    while(pool->IsPoolWorking())
    {
        _preHandlerGuard.Lock();
        if(_isEventArrived)
        {
            // 重置
            _isEventArrived = false;
            _preHandlerGuard.Unlock();

            do 
            {
                // accept 直到eagain
                sockFd = ::accept(_acceptorSockFd, reinterpret_cast<sockaddr *>(&inAddr), &addrSize);
                g_Log->i<FS_EpollPoller>(_LOGFMT_("accept a new connect acceptorSock[%d] new sock[%d]"), _acceptorSockFd, sockFd);
                if(sockFd > 0)
                {
                    // 建立连接
                    _HandleSessionWillConnect(netEngine, sockFd, &inAddr);
                }
                else
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        g_Log->i<FS_EpollPoller>(_LOGFMT_("EAGAIN or EWOULDBLOCK coming when accept in a dead loop]"));
                        break;
                    }
                }
            } while(true);

            // 再次判断有没有事件到来
            continue;
        }

        // 判断是否退出
        if(!pool->IsPoolWorking() || _epoll->IsDestroy())
        {
            _preHandlerGuard.Unlock();
            break;
        }

        // 没有事件要处理等待下次事件的到来
        g_Log->i<FS_EpollPoller>(_LOGFMT_("wait for new connect in the future"));
        _preHandlerGuard.DeadWait();
        g_Log->i<FS_EpollPoller>(_LOGFMT_("new connect is coming"));
        _preHandlerGuard.Unlock();
    }

    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll acceptor prehandle poller finished compId[%u]  threadId[%llu] end")
                               , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnTransferEvMonitorPoller(FS_ThreadPool *pool)
{
    Int32 ret = 0;
    Int32 sockFd = INVALID_SOCKET;
    struct sockaddr_in inAddr;
    epoll_event *evs = NULL;
    Int32 loopPos = 0;

    // 等待其他组件ready
    const UInt32 generatorId = _engineComp->CastTo<IFS_MsgTransfer>()->GetGeneratorId();
    const UInt32 compId = _engineComp->GetCompId();
    auto engine = _engineComp->GetEngine();
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll transfer monitor working..."));
    _engineComp->MaskReady(true);
    EngineCompsMethods::WaitForAllCompsReady(engine);
    Int64 loop = 0;
    Int64 trigCount = 0;
    while(pool->IsPoolWorking())
    {
        ret = _epoll->Wait(1000);
        if(_epoll->IsDestroy())
            break;

        if(ret == -1)
        {
            g_Log->w<FS_EpollPoller>(_LOGFMT_("epoll wait fail ret[%d] errorno[%d]"), ret, errno);
        }

        if(ret <= 0)
        {
            ++loop;
            if(loop >= 5)
            {
                loop = 0;
                g_Log->i<FS_EpollPoller>(_LOGFMT_("epoll block"));
            }
            continue;
        }

        loop = 0;
        ++trigCount;
        evs = _epoll->GetEvs();
        g_Log->i<FS_EpollPoller>(_LOGFMT_("[%d] new transfer epoll poller ev on compid[%u] trigCount[%lld]")
                                   , ret, compId, trigCount);
        auto newEvMessageBlock = new EpollEvArriveMessageBlock;
        newEvMessageBlock->_triggerAmount = ret;
        ::memcpy(newEvMessageBlock->_ev, evs, sizeof(*evs)*ret);
        if(!_innerEvThread2MsgPreHandleMq->Push(newEvMessageBlock))
        {
            g_Log->e<FS_EpollPoller>(_LOGFMT_("push new ev message block fail, stack capture:\n%s")
                                     , CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
            Fs_SafeFree(newEvMessageBlock);
        }
    }

    _engineComp->MaskReady(false);
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("acceptor FS_EpollPoller compId[%u] generatorId[%u] threadId[%llu] end")
                               , compId, generatorId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnPreDistributeEvPoller(FS_ThreadPool *pool)
{
    EpollEvArriveMessageBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    auto compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll pre distribute ev poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while(true)
    {
        _innerEvThread2MsgPreHandleMq->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _innerEvThread2MsgPreHandleMq->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if(!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for(auto iterMsg = messageBlockList->begin(); iterMsg != messageBlockList->end();)
        {
            evMsgBlock = reinterpret_cast<EpollEvArriveMessageBlock *>(*iterMsg);
            auto &evs = evMsgBlock->_ev;
            for(Int32 i = 0; i < evMsgBlock->_triggerAmount; ++i)
            {
                auto &ev = evs[i];
                if(ev.events & (EPOLLHUP | EPOLLERR))
                {
                    g_Log->i<FS_EpollPoller>(_LOGFMT_("hup or err ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                                             , ev.data.u64, compId);

                    // 取消会话io
                    CancelRecvIo(ev.data.u64, CancelIoReason::EpollHupOrError, true);
                    CancelSendIo(ev.data.u64, CancelIoReason::EpollHupOrError, true);
//                     Fs_SafeFree(*iterMsg);
//                     iterMsg = messageBlockList->erase(iterMsg);
                    continue;
                }

                if(ev.events & EPOLLIN)
                {
                    g_Log->i<FS_EpollPoller>(_LOGFMT_("epollin ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                                             , ev.data.u64, compId);
                    auto newMsgBlock = MessageBlockUtil::BuildEpollInEvMessageBlock(ev.data.u64);
                    if(!_recverMailBox->Push(newMsgBlock))
                    {
                        g_Log->e<FS_EpollPoller>(_LOGFMT_("push epoll in ev message block fail sessionId[%llu]")
                                                 , ev.data.u64);
                        Fs_SafeFree(newMsgBlock);
                    }
                }

                if(ev.events & EPOLLOUT)
                {
                    g_Log->i<FS_EpollPoller>(_LOGFMT_("epollout ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                                             , ev.data.u64, compId);
                    // 先判断sessionId是否存在
                    auto newMsgBlock = MessageBlockUtil::BuildEpollOutEvMessageBlock(ev.data.u64);
                    if(!_senderMailBox->Push(newMsgBlock))
                    {
                        g_Log->e<FS_EpollPoller>(_LOGFMT_("push epoll out ev message block fail sessionId[%llu]")
                                                 , ev.data.u64);
                        Fs_SafeFree(newMsgBlock);
                    }
                }
            }

            Fs_SafeFree(evMsgBlock);
            iterMsg = messageBlockList->erase(iterMsg);
        }

        if(!isMqWorking)
            break;
    }

    // 销毁资源
    if(!messageBlockList->empty())
    {
        g_Log->w<FS_EpollPoller>(_LOGFMT_("has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll pre distribute ev poller finished compId[%u]  threadId[%llu] end")
                               , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnReadPoller(FS_ThreadPool *pool)
{
    FS_EpollEvMessageBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll read poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while(true)
    {
        _recverMailBox->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _recverMailBox->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if(!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for(auto iterBlock = messageBlockList->begin(); iterBlock != messageBlockList->end();)
        {
            g_Log->i<FS_EpollPoller>(_LOGFMT_("new recv will come "));
            evMsgBlock = reinterpret_cast<FS_EpollEvMessageBlock *>(*iterBlock);
            (this->*_msgBlockHandler[evMsgBlock->_messageType])(evMsgBlock);
            Fs_SafeFree(evMsgBlock);
            iterBlock = messageBlockList->erase(iterBlock);
        }

        if(!isMqWorking)
            break;
    }

    // 销毁资源
    if(!messageBlockList->empty())
    {
        g_Log->w<FS_EpollPoller>(_LOGFMT_("_OnReadPoller has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll read poller finished compId[%u]  threadId[%llu] end")
                               , _engineComp->GetCompId(), SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnSendPoller(FS_ThreadPool *pool)
{
    FS_EpollEvMessageBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    const UInt32 compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll send poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while(true)
    {
        _senderMailBox->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _senderMailBox->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if(!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for(auto iterBlock = messageBlockList->begin(); iterBlock != messageBlockList->end();)
        {
            evMsgBlock = reinterpret_cast<FS_EpollEvMessageBlock *>(*iterBlock);
            (this->*_msgBlockHandler[evMsgBlock->_messageType])(evMsgBlock);
            Fs_SafeFree(evMsgBlock);
            iterBlock = messageBlockList->erase(iterBlock);
        }

        if(!isMqWorking)
            break;
    }

    // 销毁资源
    if(!messageBlockList->empty())
    {
        g_Log->w<FS_EpollPoller>(_LOGFMT_("_OnSendPoller has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollPoller>(_LOGFMT_("epoll send poller finished compId[%u]  threadId[%llu] end")
                               , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollPoller::_OnEpollReadableEv(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a readable msg is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("EpollReadable");
    _recvLock.Lock();
    do 
    {
        // 1.判断sessionId是否被移除
        EpollEvEpollInEvMessageBlock *block = reinterpret_cast<EpollEvEpollInEvMessageBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a readable msg is coming sessionId[%llu]")
                                 , sessionId);
        auto iterStatus = _sessionIdRefRecverEvStatus.find(sessionId);
        if(iterStatus == _sessionIdRefRecverEvStatus.end())
            break;

        // 2.设置_sessionIdRefRecverEvStatus为触发状态
        iterStatus->second = true;

        // 3.获取_sessionIdRefRecverEvIoData,iodata，若没有说明dispatcher还没post
        auto iterIoData = _sessionIdRefRecverEvIoData.find(sessionId);
        if(iterIoData == _sessionIdRefRecverEvIoData.end())
            break;

        // 4.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if(!ioData) // ioData在刚刚连接上时候为空
            break;

        const SOCKET sock = ioData->_sock;

        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while((len = _OnRecv(sock,
                             buff + bytesOffset,
                             leftBufferSize - bytesOffset,
                             sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollPoller>(_LOGFMT_("will post recv suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
                                 , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if(bytesOffset)
        {
            _sessionIdRefRecverEvIoData.erase(iterIoData);
            _PostRecvMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        // 7.处理错误
        if(len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if(sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
               sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 接收结束
                iterStatus->second = false;
                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv eagain or ewouldblock")
                                         ,sessionId, len, sockErrorCode);
            }
            else if(sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接

                _sessionIdRefRecverEvStatus.erase(iterStatus);
                _sessionIdRefRecverEvIoData.erase(sessionId);
                _PostRecvMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv not buffer full and remove session")
                                         , sessionId, len, sockErrorCode);
            }

            g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] eagain or ewouldblock")
                                     , sessionId, len, sockErrorCode);
            break;
        }
        else if(len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefRecverEvStatus.erase(iterStatus);
            _sessionIdRefRecverEvIoData.erase(sessionId);
            _PostRecvMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv len is zero and remove session")
                                     , sessionId, len, sockErrorCode);
            break;
        }
    } while (0);
    _recvLock.Unlock();   
}

void FS_EpollPoller::_OnEpollWritableEv(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a writable msg is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("EpollWritable");
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        EpollEvEpollOutEvMessageBlock *block = reinterpret_cast<EpollEvEpollOutEvMessageBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a writable msg is coming sessionId[%llu]")
                                 ,sessionId);

        auto iterStatus = _sessionIdRefSenderEvStatus.find(sessionId);
        if(iterStatus == _sessionIdRefSenderEvStatus.end())
            break;

        // 2.设置_sessionIdRefRecverEvStatus为触发状态
        iterStatus->second = true;

        // 3.获取_sessionIdRefRecverEvIoData,iodata，若没有说明dispatcher还没post
        auto iterIoData = _sessionIdRefSenderIoData.find(sessionId);
        if(iterIoData == _sessionIdRefSenderIoData.end())
            break;

        // 4.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if(!ioData)
            break;

        const SOCKET sock = ioData->_sock;

        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while((len = _OnSend(sock,
                             buff + bytesOffset,
                             leftBufferSize - bytesOffset,
                             sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollPoller>(_LOGFMT_("will post send suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
                                 , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if(bytesOffset)
        {
            _sessionIdRefSenderIoData.erase(iterIoData);
            _PostSendSucMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        // 7.处理错误
        if(len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if(sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
               sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可写设置触发状态为false
             // 发送结束
                iterStatus->second = false;

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send eagain or ewouldblock")
                                         , sessionId, len, sockErrorCode);
            }
            else if(sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send not buffer full and remove session")
                                         , sessionId, len, sockErrorCode);

                // 其他错误则断开网络连接
                _sessionIdRefSenderIoData.erase(sessionId);
                _sessionIdRefSenderEvStatus.erase(iterStatus);
                _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
            }

            break;
        }
        else if(len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            // _OnSendRemoveSession(sock, sessionId);
            _sessionIdRefSenderIoData.erase(sessionId);
            _sessionIdRefSenderEvStatus.erase(iterStatus);
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send len is zeror and remove session")
                                     , sessionId, len, sockErrorCode);
            break;
        }
    } while(0);
    _sendLock.Unlock();
}

void FS_EpollPoller::_OnBePostedRecv(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a recv msg posted from dispatcher is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("BePostedRecv");
    _recvLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostRecvMessageBlock *block = reinterpret_cast<PostRecvMessageBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const SOCKET sock = block->_ioData->_sock;

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a recv msg posted from dispatcher is coming sock[%d] sessionId[%llu]")
                                 ,sock, sessionId);

        auto iterStatus = _sessionIdRefRecverEvStatus.find(sessionId);
        if(iterStatus == _sessionIdRefRecverEvStatus.end())
            break;

        // 2. 插入iodata
        auto iterIoData = _sessionIdRefRecverEvIoData.find(sessionId);
        if(iterIoData == _sessionIdRefRecverEvIoData.end())
            iterIoData = _sessionIdRefRecverEvIoData.insert(std::make_pair(sessionId, block->_ioData)).first;
        else
            iterIoData->second = block->_ioData;

        // 2.判断sock的触发状态
        if(!iterStatus->second)
            break;

        // 3.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if(!ioData)
            break;

        // 4. 读取数据
        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while((len = _OnRecv(sock,
                             buff + bytesOffset,
                             leftBufferSize - bytesOffset,
                             sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollPoller>(_LOGFMT_("will post recv suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
                                 , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if(bytesOffset)
        {
            _sessionIdRefRecverEvIoData.erase(iterIoData);
            _PostRecvMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

//         if(bytesOffset)
//         {
// 
//         }

        // 7.处理错误
        if(len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if(sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
               sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 接收结束
                iterStatus->second = false;

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv eagain or ewouldblock")
                                         , sessionId, len, sockErrorCode);
            }
            else if(sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接
                _sessionIdRefRecverEvStatus.erase(iterStatus);
                _sessionIdRefRecverEvIoData.erase(sessionId);
                _PostRecvMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv not buffer full and remove session")
                                         , sessionId, len, sockErrorCode);
            }

            break;
        }
        else if(len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefRecverEvStatus.erase(iterStatus);
             _sessionIdRefRecverEvIoData.erase(sessionId);
             _PostRecvMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv len is zeror and remove session")
                                     , sessionId, len, sockErrorCode);
            break;
        }
    } while(0);
    _recvLock.Unlock();
}

void FS_EpollPoller::_OnBePostedSend(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a send msg posted from dispatcher is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("BePostedSend");
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostSendMessageBlock *block = reinterpret_cast<PostSendMessageBlock *>(messageBlock);
        const SOCKET sock = block->_ioData->_sock;
        const UInt64 sessionId  = block->_sessionId;

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a send msg posted from dispatcher is coming sessionId[%llu]")
                                 , sessionId);

        auto iterStatus = _sessionIdRefSenderEvStatus.find(sessionId);
        if(iterStatus == _sessionIdRefSenderEvStatus.end())
            break;

        // 2. 插入iodata
        auto iterIoData = _sessionIdRefSenderIoData.find(sessionId);
        if(iterIoData == _sessionIdRefSenderIoData.end())
            iterIoData = _sessionIdRefSenderIoData.insert(std::make_pair(sessionId, block->_ioData)).first;
        else
            iterIoData->second = block->_ioData;

        // 2.判断sock的触发状态
        if(!iterStatus->second)
            break;

        // 3.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if(!ioData)
            break;

        // 4. 读取数据
        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while((len = _OnSend(sock,
                             buff + bytesOffset,
                             leftBufferSize - bytesOffset,
                             sockErrorCode)) > 0)
        {
            bytesOffset += static_cast<Int64>(len);
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollPoller>(_LOGFMT_("will post send suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
                                 , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if(bytesOffset)
        {
            _sessionIdRefSenderIoData.erase(iterIoData);
            _PostSendSucMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

//         if(bytesOffset)
//         {
// 
//         }

        // 7.处理错误
        if(len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if(sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
               sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 发送结束
                iterStatus->second = false;

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send eagain or ewouldblock")
                                         , sessionId, len, sockErrorCode);
            }
            else if(sockErrorCode!=StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接
                _sessionIdRefSenderEvStatus.erase(iterStatus);
                _sessionIdRefSenderIoData.erase(sessionId);
                _PostSendSucMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send not buffer full and remove session")
                                         , sessionId, len, sockErrorCode);
            }

            break;
        }
        else if(len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefSenderEvStatus.erase(iterStatus);
            _sessionIdRefSenderIoData.erase(sessionId);
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);

            g_Log->i<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send len is zeror and remove session")
                                     , sessionId, len, sockErrorCode);
            break;
        }
    } while(0);
    _sendLock.Unlock();
}

void FS_EpollPoller::_OnBePostedCancleIoToRecver(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a cancel io to recver is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);
    _recvLock.Lock();
    do
    {

        // 1.判断sessionId是否被移除
        PostCancelIoToRecverMessageBlock *block = reinterpret_cast<PostCancelIoToRecverMessageBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const Int32 reason = block->_reason;
        const bool giveRes = block->_giveRes;

        // 2.判断是否在监听队列中避免重复Post到dispatcherDispatcher造成死循环
        if(_sessionIdRefRecverEvStatus.find(sessionId) == _sessionIdRefRecverEvStatus.end())
        {
            g_Log->w<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] is already removed from _sessionIdRefRecverEvStatus"), sessionId);
            break;
        }

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a cancel io to recver is coming sessionId[%llu] reason[%s], giveRes[%d]")
                                 , sessionId, CancelIoReason::GetStr(reason).c_str(), giveRes);

        _sessionIdRefRecverEvStatus.erase(sessionId);
        _sessionIdRefRecverEvIoData.erase(sessionId);
        if(giveRes)
            _PostRecvMsgToDispatcher(sessionId, 0, NULL);
    } while(0);
    _recvLock.Unlock();
}

void FS_EpollPoller::_OnBePostedCancleIoToSender(FS_EpollEvMessageBlock *messageBlock)
{
    g_Log->i<FS_EpollPoller>(_LOGFMT_("a cancel io to sender is coming msgtype[%d], generatorId[%u], compId[%u]")
                             , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostCancelIoToSenderMessageBlock *block = reinterpret_cast<PostCancelIoToSenderMessageBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const Int32 reason = block->_reason;
        const bool giveRes = block->_giveRes;

        // 2.判断是否在监听队列中避免重复Post到dispatcherDispatcher造成死循环
        if(_sessionIdRefSenderEvStatus.find(sessionId) == _sessionIdRefSenderEvStatus.end())
        {
            g_Log->w<FS_EpollPoller>(_LOGFMT_("sessionId[%llu] is already removed from _sessionIdRefSenderEvStatus"), sessionId);
            break;
        }

        g_Log->i<FS_EpollPoller>(_LOGFMT_("a cancel io to sender is coming sessionId[%llu] reason[%s], giveRes[%d]")
                                 , sessionId, CancelIoReason::GetStr(reason).c_str(), giveRes);

        _sessionIdRefSenderEvStatus.erase(sessionId);
        _sessionIdRefSenderIoData.erase(sessionId);
        if(giveRes)
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
    } while(0);
    _sendLock.Unlock();
}

Int32 FS_EpollPoller::_OnRecv(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode)
{
    sockErrorCode = StatusDefs::Success;
    Int32 ret = 0;
    Int32 err = -1;

    do 
    {
        if(buffLen <= 0)
        {
            sockErrorCode = StatusDefs::FS_SockError_BufferFull;
            g_Log->w<FS_EpollPoller>(_LOGFMT_("buff not enough sock[%d] buff address[0x%p] FS_SockError_BufferFull")
                                     , sock, buff);
            ret = StatusDefs::SockError;
            break;
        }

        // 信号中断需要继续recv
        while((ret = ::recv(sock,
                            buff,
                            buffLen,
                            0)) < 0 && errno == EINTR);
        if(ret == -1)
        {
            err = errno;
            if(err == EWOULDBLOCK)
            {
                sockErrorCode = StatusDefs::FS_SockError_EWOULDBLOCK;
                break;
            }
            else if(err == EAGAIN)
            {
                sockErrorCode = StatusDefs::FS_SockError_EAGAIN;
                break;
            }

            g_Log->w<FS_EpollPoller>(_LOGFMT_("unknown errno[%d:%s] sock[%d] buff[0x%p] buffLen[%lld]")
                                     , err, strerror(err), sock, buff, buffLen);
            sockErrorCode = StatusDefs::FS_SockError_UnknownError;
            break;
        }
    } while (0);

    g_Log->i<FS_EpollPoller>(_LOGFMT_("sock[%d], buff[0x%p], bufflen[%lld], sockErrorCode[%d] ret[%d], error[%d %s]")
                             , sock, buff, buffLen, sockErrorCode, ret, err, err != -1 ? (strerror(err)) : "no error");

    return ret;
}

Int32 FS_EpollPoller::_OnSend(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode)
{
    sockErrorCode = StatusDefs::Success;
    Int32 ret = 0;
    Int32 err = -1;
    do 
    {
        if(buffLen <= 0)
        {
            sockErrorCode = StatusDefs::FS_SockError_BufferFull;

            g_Log->w<FS_EpollPoller>(_LOGFMT_("buff not enough sock[%d] buff address[0x%p] FS_SockError_BufferFull")
                                     , sock, buff);
            ret = StatusDefs::SockError;
            break;
        }

        // 信号中断需要继续send
        while((ret = ::send(sock,
                            buff,
                            buffLen,
                            0)) < 0 && errno == EINTR);

        //     Int32 err = -1;
        //     if(ret < 0)
        //         err = errno;
        // 
        //     g_Log->i<FS_EpollPoller>(_LOGFMT_("buff not enough ret[%d] sock[%d] buff address[0x%p] buffLen[%lld] err:[%d:%s]")
        //                              , ret, sock, buff, buffLen, err, ret < 0 ? strerror(err) : "no error");

        if(ret == -1)
        {
            err = errno;
            if(err == EWOULDBLOCK)
            {
                sockErrorCode = StatusDefs::FS_SockError_EWOULDBLOCK;
                break;
            }
            else if(err == EAGAIN)
            {
                sockErrorCode = StatusDefs::FS_SockError_EAGAIN;
                break;
            }

            g_Log->w<FS_EpollPoller>(_LOGFMT_("unknown errno[%d:%s] sock[%d] buff[0x%p] buffLen[%lld]")
                                     , err, strerror(err), sock, buff, buffLen);
            sockErrorCode = StatusDefs::FS_SockError_UnknownError;
            break;
        }

    } while (0);

    g_Log->i<FS_EpollPoller>(_LOGFMT_("sock[%d], buff[0x%p], bufflen[%lld], sockErrorCode[%d] ret[%d], error[%d %s]")
                             , sock, buff, buffLen, sockErrorCode, ret, err, err != -1 ? (strerror(err)) : "no error");
    return ret;
}


void FS_EpollPoller::_OnRecvRemoveSession(SOCKET sock, UInt64 sessionId)
{
    // 打印日志
    g_Log->net<FS_EpollPoller>("_OnRecvRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove"
                               , _generatorId, sock, sessionId);
    g_Log->i<FS_EpollPoller>(_LOGFMT_("generatorId[%u] sock[%d] sessionid[%llu] will remove from poller")
                             ,_generatorId, sock, sessionId);

    _sessionIdRefRecverEvStatus.erase(sessionId);
    _sessionIdRefRecverEvIoData.erase(sessionId);
    _PostRecvMsgToDispatcher(sessionId, 0, NULL);
}

void FS_EpollPoller::_OnSendRemoveSession(SOCKET sock, UInt64 sessionId)
{
    // 打印日志
    g_Log->net<FS_EpollPoller>("_OnSendRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove"
                               , _generatorId, sock, sessionId);
    g_Log->i<FS_EpollPoller>(_LOGFMT_("_OnSendRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove")
                               , _generatorId, sock, sessionId);
    _sessionIdRefSenderEvStatus.erase(sessionId);
    _sessionIdRefSenderIoData.erase(sessionId);
    _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
}

void FS_EpollPoller::_PostCancelToRecver(UInt64 sessionId, Int32 reason, bool giveRes)
{
    PostCancelIoToRecverMessageBlock *newBlock = new PostCancelIoToRecverMessageBlock(sessionId, reason, giveRes);
    _recverMailBox->Push(newBlock);
}

void FS_EpollPoller::_PostCancelToSender(UInt64 sessionId, Int32 reason, bool giveRes)
{
    PostCancelIoToSenderMessageBlock *newBlock = new PostCancelIoToSenderMessageBlock(sessionId, reason, giveRes);
    _senderMailBox->Push(newBlock);
}

void FS_EpollPoller::_PostRecvMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData)
{
    EpollRecvDataMessageBlock *newMessageBlock = new EpollRecvDataMessageBlock;
    newMessageBlock->_compId = _engineComp->GetCompId();
    newMessageBlock->_generatorId = _generatorId;
    newMessageBlock->_sessionId = sessionId;
    newMessageBlock->_transferBytes = transferBytes;
    newMessageBlock->_ioData = ioData;
    newMessageBlock->_isSessionInRecverMonitor = _sessionIdRefRecverEvStatus.find(sessionId) != _sessionIdRefRecverEvStatus.end();
    _mq->Push(_generatorId, newMessageBlock);

    g_Log->i<FS_EpollPoller>(_LOGFMT_("has post recv msg to dispatcher compId[%u] _generatorId[%u] sessionId[%llu] transferBytes[%lld]")
                                    ,_engineComp->GetCompId(), _generatorId, sessionId, transferBytes);
}

void FS_EpollPoller::_PostSendSucMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData)
{
    EpollSendSucMessageBlock *newMessageBlock = new EpollSendSucMessageBlock;
    newMessageBlock->_compId = _engineComp->GetCompId();
    newMessageBlock->_generatorId = _generatorId;
    newMessageBlock->_sessionId = sessionId;
    newMessageBlock->_transferBytes = transferBytes;
    newMessageBlock->_ioData = ioData;
    newMessageBlock->_isSessionInSenderMonitor = _sessionIdRefSenderEvStatus.find(sessionId) != _sessionIdRefSenderEvStatus.end();
    _mq->Push(_generatorId, newMessageBlock);

    g_Log->i<FS_EpollPoller>(_LOGFMT_("has post send suc msg to dispatcher compId[%u] _generatorId[%u] sessionId[%llu] transferBytes[%lld]")
                                    ,_engineComp->GetCompId(), _generatorId, sessionId, transferBytes);
}

void FS_EpollPoller::_AddCount(const FS_String &name)
{
    auto tlsTable = FS_TlsUtil::GetUtilTlsTable();
    Tls_Counter *tlsCounter = tlsTable->GetElement<Tls_Counter>(TlsElemType::Tls_Counter);
    if(!tlsCounter)
        tlsCounter = tlsTable->AllocElement<Tls_Counter>(TlsElemType::Tls_Counter);

    auto &nameRefCount = tlsCounter->_nameRefCount;
    auto iterCount = nameRefCount.find(name);
    if(iterCount == nameRefCount.end())
        iterCount = nameRefCount.insert(std::make_pair(name, 0)).first;

    ++iterCount->second;
    g_Log->i<FS_EpollPoller>(_LOGFMT_("name[%s] count[%lld]"), name.c_str(), iterCount->second);
}

FS_NAMESPACE_END

#endif

