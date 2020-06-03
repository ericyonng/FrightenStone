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
 * @file  : FS_EpollTransferPoller.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/05
 * @brief :
 */

#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollTransferPoller.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"
#include "FrightenStone/common/net/Defs/EpollDefs.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"
#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"

#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/assist/utils/Impl/SystemUtil.h"
#include "FrightenStone/common/assist/utils/utils.h"

FS_NAMESPACE_BEGIN

FS_EpollTransferPoller::MessageBlockHandler FS_EpollTransferPoller::_msgBlockHandler[NetMessageType::End] = { NULL };

Int32 FS_EpollTransferPoller::BeforeStart()
{
    if (_isInit)
        return StatusDefs::Success;

    _InitMsgBlockHandlers();
    _pool = new FS_ThreadPool(0, 4);
    _epoll = new FS_Epoll;
    _innerEvThread2MsgPreHandleMq = new MessageQueueNoThread();
    _recverMailBox = new MessageQueueNoThread();
    _senderMailBox = new MessageQueueNoThread();

    Int32 st = _epoll->Create();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("create epoll fail st[%d]"), st);
        return st;
    }

    st = _innerEvThread2MsgPreHandleMq->BeforeStart();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_innerP2PMq BeforeStart fail st[%d]"), st);
        return st;
    }

    st = _innerEvThread2MsgPreHandleMq->Start();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_innerP2PMq Start fail st[%d]"), st);
        return st;
    }

    st = _recverMailBox->BeforeStart();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_recverMailBox BeforeStart fail st[%d]"), st);
        return st;
    }

    st = _senderMailBox->BeforeStart();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_senderMailBox BeforeStart fail st[%d]"), st);
        return st;
    }

    st = _recverMailBox->Start();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_recverMailBox Start fail st[%d]"), st);
        return st;
    }

    st = _senderMailBox->Start();
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_senderMailBox Start fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollTransferPoller::Start()
{
    if (_isInit)
        return StatusDefs::Success;

    return StatusDefs::Success;
}

void FS_EpollTransferPoller::AfterStart()
{
    if (_isInit)
        return;

    _generatorId = _engineComp->CastTo<IFS_MsgTransfer>()->GetGeneratorId();

    auto readPoller = DelegatePlusFactory::Create(this, &FS_EpollTransferPoller::_OnReadPoller);
    if (!_pool->AddTask(readPoller, true))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("add readPoller fail _monitorType[%d]"), _monitorType);
    }

    auto sendPoller = DelegatePlusFactory::Create(this, &FS_EpollTransferPoller::_OnSendPoller);
    if (!_pool->AddTask(sendPoller, true))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("add sendPoller fail _monitorType[%d]"), _monitorType);
    }

    // 先启动消息预处理线程
    auto preHandleTask = DelegatePlusFactory::Create(this, &FS_EpollTransferPoller::_OnPreDistributeEvPoller);
    if (!_pool->AddTask(preHandleTask, true))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("add preHandleTask fail _monitorType[%d]"), _monitorType);
    }

    // 再启动事件触发线程
    auto task = DelegatePlusFactory::Create(this, &FS_EpollTransferPoller::_OnTransferEvMonitorPoller);
    if (!_pool->AddTask(task, true))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("addtask fail _monitorType[%d]"), _monitorType);
    }

    _isInit = true;
}

void FS_EpollTransferPoller::WillClose()
{
    if (!_isInit)
        return;

    _isWillClose = true;
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll poller [%s] will close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollTransferPoller::BeforeClose()
{
    if (!_isInit)
        return;

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll poller [%s] before close..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
    _recverMailBox->BeforeClose();
    _senderMailBox->BeforeClose();

    _innerEvThread2MsgPreHandleMq->BeforeClose();

    if (!_epoll->IsDestroy())
        _epoll->Destroy();

    _preHandlerGuard.Broadcast();

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll poller [%s] before close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

void FS_EpollTransferPoller::Close()
{
    if (!_isInit)
        return;

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll poller [%s] Closing..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    _pool->Close();
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("_pool close suc [%s]..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());

    _recverMailBox->Close();
    _senderMailBox->Close();
    _innerEvThread2MsgPreHandleMq->Close();

    Fs_SafeFree(_pool);
    Fs_SafeFree(_epoll);
    Fs_SafeFree(_innerEvThread2MsgPreHandleMq);
    Fs_SafeFree(_recverMailBox);
    Fs_SafeFree(_senderMailBox);

    _isInit = false;

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll poller [%s] close suc..."), PollerDefs::GetMoniterTypeStr(_monitorType).c_str());
}

IFS_NetModule * FS_EpollTransferPoller::GetNetModuleObj()
{
    return _epoll;
}

bool FS_EpollTransferPoller::PushRecvMsg(EpollIoData *ioData)
{
    PostRecvMsgBlock *newMsgBlock = new PostRecvMsgBlock;
    newMsgBlock->_sessionId = ioData->_sessionId;
    newMsgBlock->_ioData = ioData;
    if (!_recverMailBox->Push(newMsgBlock))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_recverMailBox push msg fail sessionId[%llu]"), ioData->_sessionId);
        Fs_SafeFree(newMsgBlock);
        return false;
    }

    return true;
}

bool FS_EpollTransferPoller::PushSendMsg(EpollIoData *ioData)
{
    PostSendMsgBlock *newMsgBlock = new PostSendMsgBlock;
    newMsgBlock->_sessionId = ioData->_sessionId;
    newMsgBlock->_ioData = ioData;
    if (!_senderMailBox->Push(newMsgBlock))
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("_senderMailBox push msg fail sessionId[%llu]"), ioData->_sessionId);
        Fs_SafeFree(newMsgBlock);
        return false;
    }

    return true;
}

void FS_EpollTransferPoller::CancelRecvIo(UInt64 sessionId, Int32 reason, bool giveRes)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will cancel recv io sessionId[%llu] [reason:%s]"), sessionId, CancelIoReason::GetStr(reason).c_str());

    // 只需要分别向recver与sender发送cancel消息，recver与sender直接向dispatcher返回transfer为0的消息即可认定为会话断开
    // TODO:post recver cancel
    _PostCancelToRecver(sessionId, reason, giveRes);
}

void FS_EpollTransferPoller::CancelSendIo(UInt64 sessionId, Int32 reason, bool giveRes)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will cancel send io sessionId[%llu] [reason:%s]"), sessionId, CancelIoReason::GetStr(reason).c_str());

    // 只需要分别向recver与sender发送cancel消息，recver与sender直接向dispatcher返回transfer为0的消息即可认定为会话断开
    _PostCancelToSender(sessionId, reason, giveRes);
}

bool FS_EpollTransferPoller::OnWillConnect(SOCKET sock, UInt64 sessionId)
{// TRANSFER 与 recver sender之间可以等待，可直接加锁
    // 判断是否连接已创建
    _recvLock.Lock();
    if (_sessionIdRefRecverEvStatus.find(sessionId) != _sessionIdRefRecverEvStatus.end())
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("sock[%d] sessionId[%llu] has already connect or it is not free before"), sock, sessionId);
        _recvLock.Unlock();
        return false;
    }
    _recvLock.Unlock();

    _sendLock.Lock();
    if (_sessionIdRefSenderEvStatus.find(sessionId) != _sessionIdRefSenderEvStatus.end())
    {
        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("sock[%d] sessionId[%llu] has already connect or it is not free before"), sock, sessionId);
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

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_(" epoll poller type[%d] new sessionId[%llu] sock[%d] init status")
        , _monitorType, sessionId, sock);

    return true;

    //     const Int32 st = _epoll->AddEvent(sock, sessionId);
    //     if(st != StatusDefs::Success)
    //     {
    //         g_Log->e<FS_EpollPoller>(_LOGFMT_("_epoll->AddEvent fail epoll poller info[%s] sock[%d] sessionId[%llu] st[%d]")
    //                                  , ToString().c_str(), sock, sessionId, st);
    //     }
}

bool FS_EpollTransferPoller::IsIoFinish() const
{
    _recvLock.Lock();
    if (!_sessionIdRefRecverEvIoData.empty())
    {
        _recvLock.Unlock();
        return false;
    }
    _recvLock.Unlock();

    _sendLock.Lock();
    if (!_sessionIdRefSenderIoData.empty())
    {
        _sendLock.Unlock();
        return false;
    }
    _sendLock.Unlock();

    return true;
}

void FS_EpollTransferPoller::_OnTransferEvMonitorPoller(FS_ThreadPool *pool)
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
    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll transfer monitor working..."));
    _engineComp->MaskReady(true);
    EngineCompsMethods::WaitForAllCompsReady(engine);
    Int64 loop = 0;
    Int64 trigCount = 0;
    while (pool->IsPoolWorking())
    {
        ret = _epoll->Wait(1000);
        if (_epoll->IsDestroy())
            break;

        if (ret == -1)
        {
            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("epoll wait fail ret[%d] errorno[%d]"), ret, errno);
        }

        if (ret <= 0)
        {
            ++loop;
            if (loop >= 5)
            {
                loop = 0;
                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epoll block"));
            }
            continue;
        }

        loop = 0;
        ++trigCount;
        evs = _epoll->GetEvs();
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("[%d] new transfer epoll poller ev on compid[%u] trigCount[%lld]")
            , ret, compId, trigCount);
        auto newEvMessageBlock = new EpollEvArriveMsgBlock;
        newEvMessageBlock->_triggerAmount = ret;
        ::memcpy(newEvMessageBlock->_ev, evs, sizeof(*evs)*ret);
        if (!_innerEvThread2MsgPreHandleMq->Push(newEvMessageBlock))
        {
            g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("push new ev message block fail, stack capture:\n%s")
                , CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
            Fs_SafeFree(newEvMessageBlock);
        }
    }

    _engineComp->MaskReady(false);
    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("Poller compId[%u] generatorId[%u] threadId[%llu] end")
        , compId, generatorId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollTransferPoller::_OnPreDistributeEvPoller(FS_ThreadPool *pool)
{
    EpollEvArriveMsgBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    auto compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll pre distribute ev poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while (true)
    {
        _innerEvThread2MsgPreHandleMq->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _innerEvThread2MsgPreHandleMq->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if (!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for (auto iterMsg = messageBlockList->begin(); iterMsg != messageBlockList->end();)
        {
            evMsgBlock = reinterpret_cast<EpollEvArriveMsgBlock *>(*iterMsg);
            auto &evs = evMsgBlock->_ev;
            for (Int32 i = 0; i < evMsgBlock->_triggerAmount; ++i)
            {
                auto &ev = evs[i];
                if (ev.events & (EPOLLHUP | EPOLLERR))
                {
                    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("hup or err ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                        , ev.data.u64, compId);

                    // 取消会话io
                    CancelRecvIo(ev.data.u64, CancelIoReason::EpollHupOrError, true);
                    CancelSendIo(ev.data.u64, CancelIoReason::EpollHupOrError, true);
                    //                     Fs_SafeFree(*iterMsg);
                    //                     iterMsg = messageBlockList->erase(iterMsg);
                    continue;
                }

                if (ev.events & EPOLLIN)
                {
                    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epollin ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                        , ev.data.u64, compId);
                    auto newMsgBlock = MessageBlockUtil::BuildEpollEvEpollInEvMsgBlock(ev.data.u64);
                    if (!_recverMailBox->Push(newMsgBlock))
                    {
                        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("push epoll in ev message block fail sessionId[%llu]")
                            , ev.data.u64);
                        Fs_SafeFree(newMsgBlock);
                    }
                }

                if (ev.events & EPOLLOUT)
                {
                    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("epollout ev of sock sessionId[%llu] trigs on epoll poller compid[%u]")
                        , ev.data.u64, compId);
                    // 先判断sessionId是否存在
                    auto newMsgBlock = MessageBlockUtil::BuildEpollEvEpollOutEvMsgBlock(ev.data.u64);
                    if (!_senderMailBox->Push(newMsgBlock))
                    {
                        g_Log->e<FS_EpollTransferPoller>(_LOGFMT_("push epoll out ev message block fail sessionId[%llu]")
                            , ev.data.u64);
                        Fs_SafeFree(newMsgBlock);
                    }
                }
            }

            Fs_SafeFree(evMsgBlock);
            iterMsg = messageBlockList->erase(iterMsg);
        }

        if (!isMqWorking)
            break;
    }

    // 销毁资源
    if (!messageBlockList->empty())
    {
        g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll pre distribute ev poller finished compId[%u]  threadId[%llu] end")
        , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollTransferPoller::_OnReadPoller(FS_ThreadPool *pool)
{
    FS_NetMsgBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll read poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while (true)
    {
        _recverMailBox->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _recverMailBox->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if (!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for (auto iterBlock = messageBlockList->begin(); iterBlock != messageBlockList->end();)
        {
            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("new recv will come "));
            evMsgBlock = reinterpret_cast<FS_NetMsgBlock *>(*iterBlock);
            (this->*_msgBlockHandler[evMsgBlock->_messageType])(evMsgBlock);
            Fs_SafeFree(evMsgBlock);
            iterBlock = messageBlockList->erase(iterBlock);
        }

        if (!isMqWorking)
            break;
    }

    // 销毁资源
    if (!messageBlockList->empty())
    {
        g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("_OnReadPoller has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll read poller finished compId[%u]  threadId[%llu] end")
        , _engineComp->GetCompId(), SystemUtil::GetCurrentThreadId());
}

void FS_EpollTransferPoller::_OnSendPoller(FS_ThreadPool *pool)
{
    FS_NetMsgBlock *evMsgBlock = NULL;
    std::list<FS_MessageBlock *> *messageBlockList = new std::list<FS_MessageBlock *>;

    const UInt32 compId = _engineComp->GetCompId();
    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll send poller working..."));
    ULong timeoutMilisec = INFINITE;
    bool isMqWorking = true;
    Int64 trigCount = 0;
    while (true)
    {
        _senderMailBox->WaitForPoping(messageBlockList, timeoutMilisec);
        isMqWorking = _senderMailBox->IsWorking();
        ++trigCount;
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("trigCount[%lld]"), trigCount);
        if (!pool->IsPoolWorking() && messageBlockList->empty())
            break;

        for (auto iterBlock = messageBlockList->begin(); iterBlock != messageBlockList->end();)
        {
            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("new data to send"));
            evMsgBlock = reinterpret_cast<FS_NetMsgBlock *>(*iterBlock);
            (this->*_msgBlockHandler[evMsgBlock->_messageType])(evMsgBlock);
            Fs_SafeFree(evMsgBlock);
            iterBlock = messageBlockList->erase(iterBlock);
        }

        if (!isMqWorking)
            break;
    }

    // 销毁资源
    if (!messageBlockList->empty())
    {
        g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("_OnSendPoller has %llu message blocks unhandled"), messageBlockList->size());
        STLUtil::DelListContainer(*messageBlockList);
    }
    Fs_SafeFree(messageBlockList);

    g_Log->sys<FS_EpollTransferPoller>(_LOGFMT_("epoll send poller finished compId[%u]  threadId[%llu] end")
        , compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollTransferPoller::_OnEpollReadableEv(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a readable msg is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("EpollReadable");
    _recvLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        EpollEvEpollInEvMsgBlock *block = reinterpret_cast<EpollEvEpollInEvMsgBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a readable msg is coming sessionId[%llu]")
            , sessionId);
        auto iterStatus = _sessionIdRefRecverEvStatus.find(sessionId);
        if (iterStatus == _sessionIdRefRecverEvStatus.end())
            break;

        // 2.设置_sessionIdRefRecverEvStatus为触发状态
        iterStatus->second = true;

        // 3.获取_sessionIdRefRecverEvIoData,iodata，若没有说明dispatcher还没post
        auto iterIoData = _sessionIdRefRecverEvIoData.find(sessionId);
        if (iterIoData == _sessionIdRefRecverEvIoData.end())
            break;

        // 4.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if (!ioData) // ioData在刚刚连接上时候为空
            break;

        const SOCKET sock = ioData->_sock;

        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while ((len = _OnRecv(sock,
            buff + bytesOffset,
            leftBufferSize - bytesOffset,
            sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will post recv suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
            , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if (bytesOffset)
        {
            _sessionIdRefRecverEvIoData.erase(iterIoData);
            _PostRecvMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        // 7.处理错误
        if (len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if (sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
                sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 接收结束
                iterStatus->second = false;
                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv eagain or ewouldblock")
                    , sessionId, len, sockErrorCode);
            }
            else if (sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接

                _sessionIdRefRecverEvStatus.erase(iterStatus);
                _sessionIdRefRecverEvIoData.erase(sessionId);
                _PostRecvMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv not buffer full and remove session")
                    , sessionId, len, sockErrorCode);
            }

            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] eagain or ewouldblock")
                , sessionId, len, sockErrorCode);
            break;
        }
        else if (len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefRecverEvStatus.erase(iterStatus);
            _sessionIdRefRecverEvIoData.erase(sessionId);
            _PostRecvMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv len is zero and remove session")
                , sessionId, len, sockErrorCode);
            break;
        }
    } while (0);
    _recvLock.Unlock();
}

void FS_EpollTransferPoller::_OnEpollWritableEv(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a writable msg is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("EpollWritable");
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        EpollEvEpollOutEvMsgBlock *block = reinterpret_cast<EpollEvEpollOutEvMsgBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a writable msg is coming sessionId[%llu]")
            , sessionId);

        auto iterStatus = _sessionIdRefSenderEvStatus.find(sessionId);
        if (iterStatus == _sessionIdRefSenderEvStatus.end())
            break;

        // 2.设置_sessionIdRefRecverEvStatus为触发状态
        iterStatus->second = true;

        // 3.获取_sessionIdRefRecverEvIoData,iodata，若没有说明dispatcher还没post
        auto iterIoData = _sessionIdRefSenderIoData.find(sessionId);
        if (iterIoData == _sessionIdRefSenderIoData.end())
            break;

        // 4.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if (!ioData)
            break;

        const SOCKET sock = ioData->_sock;

        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while ((len = _OnSend(sock,
            buff + bytesOffset,
            leftBufferSize - bytesOffset,
            sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will post send suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
            , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if (bytesOffset)
        {
            _sessionIdRefSenderIoData.erase(iterIoData);
            _PostSendSucMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        // 7.处理错误
        if (len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if (sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
                sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可写设置触发状态为false
             // 发送结束
                iterStatus->second = false;

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send eagain or ewouldblock")
                    , sessionId, len, sockErrorCode);
            }
            else if (sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send not buffer full and remove session")
                    , sessionId, len, sockErrorCode);

                // 其他错误则断开网络连接
                _sessionIdRefSenderIoData.erase(sessionId);
                _sessionIdRefSenderEvStatus.erase(iterStatus);
                _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
            }

            break;
        }
        else if (len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            // _OnSendRemoveSession(sock, sessionId);
            _sessionIdRefSenderIoData.erase(sessionId);
            _sessionIdRefSenderEvStatus.erase(iterStatus);
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send len is zeror and remove session")
                , sessionId, len, sockErrorCode);
            break;
        }
    } while (0);
    _sendLock.Unlock();
}

void FS_EpollTransferPoller::_OnBePostedRecv(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a recv msg posted from dispatcher is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("BePostedRecv");
    _recvLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostRecvMsgBlock *block = reinterpret_cast<PostRecvMsgBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const SOCKET sock = block->_ioData->_sock;

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a recv msg posted from dispatcher is coming sock[%d] sessionId[%llu]")
            , sock, sessionId);

        auto iterStatus = _sessionIdRefRecverEvStatus.find(sessionId);
        if (iterStatus == _sessionIdRefRecverEvStatus.end())
            break;

        // 2. 插入iodata
        auto iterIoData = _sessionIdRefRecverEvIoData.find(sessionId);
        if (iterIoData == _sessionIdRefRecverEvIoData.end())
            iterIoData = _sessionIdRefRecverEvIoData.insert(std::make_pair(sessionId, block->_ioData)).first;
        else
            iterIoData->second = block->_ioData;

        // 2.判断sock的触发状态
        if (!iterStatus->second)
            break;

        // 3.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if (!ioData)
            break;

        // 4. 读取数据
        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while ((len = _OnRecv(sock,
            buff + bytesOffset,
            leftBufferSize - bytesOffset,
            sockErrorCode)) > 0)
        {
            bytesOffset += len;
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will post recv suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
            , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if (bytesOffset)
        {
            _sessionIdRefRecverEvIoData.erase(iterIoData);
            _PostRecvMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        //         if(bytesOffset)
        //         {
        // 
        //         }

                // 7.处理错误
        if (len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if (sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
                sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 接收结束
                iterStatus->second = false;

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv eagain or ewouldblock")
                    , sessionId, len, sockErrorCode);
            }
            else if (sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接
                _sessionIdRefRecverEvStatus.erase(iterStatus);
                _sessionIdRefRecverEvIoData.erase(sessionId);
                _PostRecvMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv not buffer full and remove session")
                    , sessionId, len, sockErrorCode);
            }

            break;
        }
        else if (len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefRecverEvStatus.erase(iterStatus);
            _sessionIdRefRecverEvIoData.erase(sessionId);
            _PostRecvMsgToDispatcher(sessionId, 0, NULL);
            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] recv len is zeror and remove session")
                , sessionId, len, sockErrorCode);
            break;
        }
    } while (0);
    _recvLock.Unlock();
}

void FS_EpollTransferPoller::_OnBePostedSend(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a send msg posted from dispatcher is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);

    _AddCount("BePostedSend");
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostSendMsgBlock *block = reinterpret_cast<PostSendMsgBlock *>(messageBlock);
        const SOCKET sock = block->_ioData->_sock;
        const UInt64 sessionId = block->_sessionId;

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a send msg posted from dispatcher is coming sessionId[%llu]")
            , sessionId);

        auto iterStatus = _sessionIdRefSenderEvStatus.find(sessionId);
        if (iterStatus == _sessionIdRefSenderEvStatus.end())
            break;

        // 2. 插入iodata
        auto iterIoData = _sessionIdRefSenderIoData.find(sessionId);
        if (iterIoData == _sessionIdRefSenderIoData.end())
            iterIoData = _sessionIdRefSenderIoData.insert(std::make_pair(sessionId, block->_ioData)).first;
        else
            iterIoData->second = block->_ioData;

        // 2.判断sock的触发状态
        if (!iterStatus->second)
            break;

        // 3.持续的读取网络数据直到eagain或者EWOULDBLOCK,或者iodata的缓冲满为止
        EpollIoData *ioData = iterIoData->second;
        if (!ioData)
            break;

        // 4. 读取数据
        Int32 len = 0;
        Byte8 *buff = ioData->_data;
        Int64 bytesOffset = 0;
        Int64 leftBufferSize = ioData->_dateLen;
        Int32 sockErrorCode = StatusDefs::Success;
        while ((len = _OnSend(sock,
            buff + bytesOffset,
            leftBufferSize - bytesOffset,
            sockErrorCode)) > 0)
        {
            bytesOffset += static_cast<Int64>(len);
        }

        // 5.post 数据收发结束message到dispatcher
        //（收到数据post，解除iodata的托管状态）并移除_sessionIdRefRecverEvIoData映射关系
        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("will post send suc msg to dispatcher buff[0x%p] buffsize[%lld] bytesOffset[%lld] sessionId[%llu] len[%d] sockErrorCode[%d]")
            , buff, leftBufferSize, bytesOffset, sessionId, len, sockErrorCode);

        if (bytesOffset)
        {
            _sessionIdRefSenderIoData.erase(iterIoData);
            _PostSendSucMsgToDispatcher(sessionId, bytesOffset, ioData);
        }

        //         if(bytesOffset)
        //         {
        // 
        //         }

                // 7.处理错误
        if (len < 0)
        {
            // 如果不是eagain 或者 WBLOCK则表示是其他错误，打印日志并关闭套接字,并post关闭事件
            if (sockErrorCode == StatusDefs::FS_SockError_EAGAIN ||
                sockErrorCode == StatusDefs::FS_SockError_EWOULDBLOCK)
            {// 若是因为eagain或者EWOULDBLOCK原因导致结束的则说明没有数据可读设置触发状态为false
             // 发送结束
                iterStatus->second = false;

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send eagain or ewouldblock")
                    , sessionId, len, sockErrorCode);
            }
            else if (sockErrorCode != StatusDefs::Success && sockErrorCode != StatusDefs::FS_SockError_BufferFull)
            {// 若是因为iodata缓冲满导致结束，说明可能还有数据，则触发状态仍然为true
                // 其他错误则断开网络连接
                _sessionIdRefSenderEvStatus.erase(iterStatus);
                _sessionIdRefSenderIoData.erase(sessionId);
                _PostSendSucMsgToDispatcher(sessionId, 0, NULL);

                g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send not buffer full and remove session")
                    , sessionId, len, sockErrorCode);
            }

            break;
        }
        else if (len == 0)
        {
            // 断开链接（优雅的关闭，poller不允许直接closesocket，需要从dispatcher来调用关闭）
            _sessionIdRefSenderEvStatus.erase(iterStatus);
            _sessionIdRefSenderIoData.erase(sessionId);
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);

            g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] len[%d] sockerrorcode[%d] send len is zeror and remove session")
                , sessionId, len, sockErrorCode);
            break;
        }
    } while (0);
    _sendLock.Unlock();
}

void FS_EpollTransferPoller::_OnBePostedCancleIoToRecver(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a cancel io to recver is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);
    _recvLock.Lock();
    do
    {

        // 1.判断sessionId是否被移除
        PostCancelIoToRecverMsgBlock *block = reinterpret_cast<PostCancelIoToRecverMsgBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const Int32 reason = block->_reason;
        const bool giveRes = block->_giveRes;

        // 2.判断是否在监听队列中避免重复Post到dispatcherDispatcher造成死循环
        if (_sessionIdRefRecverEvStatus.find(sessionId) == _sessionIdRefRecverEvStatus.end())
        {
            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] is already removed from _sessionIdRefRecverEvStatus"), sessionId);
            break;
        }

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a cancel io to recver is coming sessionId[%llu] reason[%s], giveRes[%d]")
            , sessionId, CancelIoReason::GetStr(reason).c_str(), giveRes);

        _sessionIdRefRecverEvStatus.erase(sessionId);
        _sessionIdRefRecverEvIoData.erase(sessionId);
        if (giveRes)
            _PostRecvMsgToDispatcher(sessionId, 0, NULL);
    } while (0);
    _recvLock.Unlock();
}

void FS_EpollTransferPoller::_OnBePostedCancleIoToSender(FS_NetMsgBlock *messageBlock)
{
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a cancel io to sender is coming msgtype[%d], generatorId[%u], compId[%u]")
        , messageBlock->_messageType, messageBlock->_generatorId, messageBlock->_compId);
    _sendLock.Lock();
    do
    {
        // 1.判断sessionId是否被移除
        PostCancelIoToSenderMsgBlock *block = reinterpret_cast<PostCancelIoToSenderMsgBlock *>(messageBlock);
        const UInt64 sessionId = block->_sessionId;
        const Int32 reason = block->_reason;
        const bool giveRes = block->_giveRes;

        // 2.判断是否在监听队列中避免重复Post到dispatcherDispatcher造成死循环
        if (_sessionIdRefSenderEvStatus.find(sessionId) == _sessionIdRefSenderEvStatus.end())
        {
            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("sessionId[%llu] is already removed from _sessionIdRefSenderEvStatus"), sessionId);
            break;
        }

        g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("a cancel io to sender is coming sessionId[%llu] reason[%s], giveRes[%d]")
            , sessionId, CancelIoReason::GetStr(reason).c_str(), giveRes);

        _sessionIdRefSenderEvStatus.erase(sessionId);
        _sessionIdRefSenderIoData.erase(sessionId);
        if (giveRes)
            _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
    } while (0);
    _sendLock.Unlock();
}

Int32 FS_EpollTransferPoller::_OnRecv(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode)
{
    sockErrorCode = StatusDefs::Success;
    Int32 ret = 0;
    Int32 err = -1;

    do
    {
        if (buffLen <= 0)
        {
            sockErrorCode = StatusDefs::FS_SockError_BufferFull;
            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("buff not enough sock[%d] buff address[0x%p] FS_SockError_BufferFull")
                , sock, buff);
            ret = StatusDefs::SockError;
            break;
        }

        // 信号中断需要继续recv
        while ((ret = ::recv(sock,
            buff,
            buffLen,
            0)) < 0 && errno == EINTR);
        if (ret == -1)
        {
            err = errno;
            if (err == EWOULDBLOCK)
            {
                sockErrorCode = StatusDefs::FS_SockError_EWOULDBLOCK;
                break;
            }
            else if (err == EAGAIN)
            {
                sockErrorCode = StatusDefs::FS_SockError_EAGAIN;
                break;
            }

            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("unknown errno[%d:%s] sock[%d] buff[0x%p] buffLen[%lld]")
                , err, strerror(err), sock, buff, buffLen);
            sockErrorCode = StatusDefs::FS_SockError_UnknownError;
            break;
        }
    } while (0);

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sock[%d], buff[0x%p], bufflen[%lld], sockErrorCode[%d] ret[%d], error[%d %s]")
        , sock, buff, buffLen, sockErrorCode, ret, err, err != -1 ? (strerror(err)) : "no error");

    return ret;
}

Int32 FS_EpollTransferPoller::_OnSend(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode)
{
    sockErrorCode = StatusDefs::Success;
    Int32 ret = 0;
    Int32 err = -1;
    do
    {
        if (buffLen <= 0)
        {
            sockErrorCode = StatusDefs::FS_SockError_BufferFull;

            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("buff not enough sock[%d] buff address[0x%p] FS_SockError_BufferFull")
                , sock, buff);
            ret = StatusDefs::SockError;
            break;
        }

        // 信号中断需要继续send
        while ((ret = ::send(sock,
            buff,
            buffLen,
            0)) < 0 && errno == EINTR);

        //     Int32 err = -1;
        //     if(ret < 0)
        //         err = errno;
        // 
        //     g_Log->i<FS_EpollPoller>(_LOGFMT_("buff not enough ret[%d] sock[%d] buff address[0x%p] buffLen[%lld] err:[%d:%s]")
        //                              , ret, sock, buff, buffLen, err, ret < 0 ? strerror(err) : "no error");

        if (ret == -1)
        {
            err = errno;
            if (err == EWOULDBLOCK)
            {
                sockErrorCode = StatusDefs::FS_SockError_EWOULDBLOCK;
                break;
            }
            else if (err == EAGAIN)
            {
                sockErrorCode = StatusDefs::FS_SockError_EAGAIN;
                break;
            }

            g_Log->w<FS_EpollTransferPoller>(_LOGFMT_("unknown errno[%d:%s] sock[%d] buff[0x%p] buffLen[%lld]")
                , err, strerror(err), sock, buff, buffLen);
            sockErrorCode = StatusDefs::FS_SockError_UnknownError;
            break;
        }

    } while (0);

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("sock[%d], buff[0x%p], bufflen[%lld], sockErrorCode[%d] ret[%d], error[%d %s]")
        , sock, buff, buffLen, sockErrorCode, ret, err, err != -1 ? (strerror(err)) : "no error");
    return ret;
}

void FS_EpollTransferPoller::_OnRecvRemoveSession(SOCKET sock, UInt64 sessionId)
{
    // 打印日志
    g_Log->net<FS_EpollTransferPoller>("_OnRecvRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove"
        , _generatorId, sock, sessionId);
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("generatorId[%u] sock[%d] sessionid[%llu] will remove from poller")
        , _generatorId, sock, sessionId);

    _sessionIdRefRecverEvStatus.erase(sessionId);
    _sessionIdRefRecverEvIoData.erase(sessionId);
    _PostRecvMsgToDispatcher(sessionId, 0, NULL);
}

void FS_EpollTransferPoller::_OnSendRemoveSession(SOCKET sock, UInt64 sessionId)
{
    // 打印日志
    g_Log->net<FS_EpollTransferPoller>("_OnSendRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove"
        , _generatorId, sock, sessionId);
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("_OnSendRemoveSession generatorId[%u] sock[%d] sessionId[%llu] will remove")
        , _generatorId, sock, sessionId);
    _sessionIdRefSenderEvStatus.erase(sessionId);
    _sessionIdRefSenderIoData.erase(sessionId);
    _PostSendSucMsgToDispatcher(sessionId, 0, NULL);
}

void FS_EpollTransferPoller::_PostCancelToRecver(UInt64 sessionId, Int32 reason, bool giveRes)
{
    PostCancelIoToRecverMsgBlock *newBlock = new PostCancelIoToRecverMsgBlock(sessionId, reason, giveRes);
    _recverMailBox->Push(newBlock);
}

void FS_EpollTransferPoller::_PostCancelToSender(UInt64 sessionId, Int32 reason, bool giveRes)
{
    PostCancelIoToSenderMsgBlock *newBlock = new PostCancelIoToSenderMsgBlock(sessionId, reason, giveRes);
    _senderMailBox->Push(newBlock);
}

void FS_EpollTransferPoller::_PostRecvMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData)
{
    EpollRecvDataMsgBlock *newMessageBlock = new EpollRecvDataMsgBlock(_engineComp->GetCompId(), _generatorId);
    newMessageBlock->_sessionId = sessionId;
    newMessageBlock->_transferBytes = transferBytes;
    newMessageBlock->_ioData = ioData;
    newMessageBlock->_isSessionInRecverMonitor = _sessionIdRefRecverEvStatus.find(sessionId) != _sessionIdRefRecverEvStatus.end();
    _mq->Push(_generatorId, newMessageBlock);

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("has post recv msg to dispatcher compId[%u] _generatorId[%u] sessionId[%llu] transferBytes[%lld]")
        , _engineComp->GetCompId(), _generatorId, sessionId, transferBytes);
}

void FS_EpollTransferPoller::_PostSendSucMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData)
{
    EpollSendSucMsgBlock *newMessageBlock = new EpollSendSucMsgBlock(_engineComp->GetCompId(), _generatorId);
    newMessageBlock->_sessionId = sessionId;
    newMessageBlock->_transferBytes = transferBytes;
    newMessageBlock->_ioData = ioData;
    newMessageBlock->_isSessionInSenderMonitor = _sessionIdRefSenderEvStatus.find(sessionId) != _sessionIdRefSenderEvStatus.end();
    _mq->Push(_generatorId, newMessageBlock);

    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("has post send suc msg to dispatcher compId[%u] _generatorId[%u] sessionId[%llu] transferBytes[%lld]")
        , _engineComp->GetCompId(), _generatorId, sessionId, transferBytes);
}

void FS_EpollTransferPoller::_AddCount(const FS_String &name)
{
    auto tlsTable = FS_TlsUtil::GetUtilTlsTable();
    Tls_Counter *tlsCounter = tlsTable->GetElement<Tls_Counter>(TlsElemType::Tls_Counter);
    if (!tlsCounter)
        tlsCounter = tlsTable->AllocElement<Tls_Counter>(TlsElemType::Tls_Counter);

    auto &nameRefCount = tlsCounter->_nameRefCount;
    auto iterCount = nameRefCount.find(name);
    if (iterCount == nameRefCount.end())
        iterCount = nameRefCount.insert(std::make_pair(name, 0)).first;

    ++iterCount->second;
    g_Log->i<FS_EpollTransferPoller>(_LOGFMT_("name[%s] count[%lld]"), name.c_str(), iterCount->second);
}

void FS_EpollTransferPoller::_InitMsgBlockHandlers()
{
    _msgBlockHandler[NetMessageType::EpollInTrigger] = &FS_EpollTransferPoller::_OnEpollReadableEv;
    _msgBlockHandler[NetMessageType::EpollOutTrigger] = &FS_EpollTransferPoller::_OnEpollWritableEv;
    _msgBlockHandler[NetMessageType::PostRecv] = &FS_EpollTransferPoller::_OnBePostedRecv;
    _msgBlockHandler[NetMessageType::PostSend] = &FS_EpollTransferPoller::_OnBePostedSend;
    _msgBlockHandler[NetMessageType::PostCancelIoToRecver] = &FS_EpollTransferPoller::_OnBePostedCancleIoToRecver;
    _msgBlockHandler[NetMessageType::PostCancelIoToSender] = &FS_EpollTransferPoller::_OnBePostedCancleIoToSender;
}
FS_NAMESPACE_END

#endif
