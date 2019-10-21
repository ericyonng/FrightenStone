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
 * @file  : FS_IocpMsgDispatcher.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgDispatcher.h"
#include "base/common/net/Impl/FS_IocpSession.h"
#include "base/common/net/Defs/FS_IocpBuffer.h"
#include "base/common/net/Impl/IFS_MsgTransfer.h"
#include "base/common/net/Impl/IFS_BusinessLogic.h"

#include "base/common/memorypool/memorypool.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/TimeWheel/TimeWheel.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/component/Impl/TimeWheel/FS_Timer.h"
#include "base/common/assist/utils/utils.h"

FS_NAMESPACE_BEGIN

FS_IocpMsgDispatcher::FS_IocpMsgDispatcher()
    :_pool(NULL)
    , _isClose{false}
    ,_timeWheel(NULL)
    ,_logic(NULL)
{

}

FS_IocpMsgDispatcher::~FS_IocpMsgDispatcher()
{
    Fs_SafeFree(_pool);
    Fs_SafeFree(_timeWheel);
    g_BusinessTimeWheel = NULL;
}

Int32 FS_IocpMsgDispatcher::BeforeStart()
{
    _resolutionInterval = 100 * Time::_microSecondPerMilliSecond;
    _timeWheel = new TimeWheel(_resolutionInterval);
    g_BusinessTimeWheel = _timeWheel;
    _pool = new FS_ThreadPool(0, 1);

    auto st = _logic->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("_logic->BeforeStart error st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgDispatcher::Start()
{
    auto task = DelegatePlusFactory::Create(this, &FS_IocpMsgDispatcher::_OnBusinessProcessThread);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("add task fail"));
        return StatusDefs::FS_IocpMsgHandler_StartFailOfBusinessProcessThreadFailure;
    }

    auto st = _logic->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("_logic->Start error st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

void FS_IocpMsgDispatcher::BeforeClose()
{
    _locker.Lock();
    _sessionIdRefTransfer.clear();
    _locker.Unlock();

    _logic->BeforeClose();
}

void FS_IocpMsgDispatcher::Close()
{
    _isClose = true;

    // 线程退出
    _pool->Clear();

    g_BusinessTimeWheel = NULL;

    // 清理数据
    for(auto &msgList : _sessionIdRefMsgs)
    {
        for(auto &msg : *msgList.second)
        {
            g_MemoryPool->Lock();
            g_MemoryPool->Free(msg);
            g_MemoryPool->Unlock();
        }
    }
    STLUtil::DelMapContainer(_sessionIdRefMsgs);

    for(auto &msgList : _sessionIdRefMsgCache)
    {
        for(auto &msg : *msgList.second)
        {
            g_MemoryPool->Lock();
            g_MemoryPool->Free(msg);
            g_MemoryPool->Unlock();
        }
    }
    STLUtil::DelMapContainer(_sessionIdRefMsgCache);

    _logic->Close();
}

void FS_IocpMsgDispatcher::OnRecv(IFS_Session *session)
{
    auto iocpSession = session->CastTo<FS_IocpSession>();
    auto recvBuffer = iocpSession->GetRecvBuffer()->CastToBuffer<FS_IocpBuffer>();

    _locker.Lock();
    if(!_isClose)
    {
        bool hasMsg = recvBuffer->HasMsg();
        while(recvBuffer->HasMsg())
        {
            auto frontMsg = recvBuffer->CastToData<NetMsg_DataHeader>();
            _MoveToBusinessLayer(session, frontMsg);
            recvBuffer->PopFront(frontMsg->_packetLength);
        }

        if(hasMsg)
            _locker.Sinal();
    }
    
    _locker.Unlock();
}

void FS_IocpMsgDispatcher::OnDisconnected(IFS_Session *session)
{
    _locker.Lock();
    auto sessionId = session->GetSessionId();
    _sessionIdRefTransfer.erase(session->GetSessionId());
    _delayDisconnectedSessions.insert(session->GetSessionId());
    _locker.Unlock();
}

void FS_IocpMsgDispatcher::OnConnect(UInt64 sessionId, IFS_MsgTransfer *transfer)
{
    _locker.Lock();
    if(!_isClose)
    {
        auto iterTransfer = _sessionIdRefTransfer.find(sessionId);
        if(iterTransfer == _sessionIdRefTransfer.end())
            _sessionIdRefTransfer.insert(std::make_pair(sessionId, transfer));
    }
    _locker.Unlock();
}

void FS_IocpMsgDispatcher::OnDestroy()
{
}

void FS_IocpMsgDispatcher::OnHeartBeatTimeOut()
{

}

void FS_IocpMsgDispatcher::SendData(UInt64 sessionId, NetMsg_DataHeader *msg)
{ 
    // 数据拷贝
    g_MemoryPool->Lock();
    char *buffer = g_MemoryPool->Alloc<char>(msg->_packetLength);
    g_MemoryPool->Unlock();
    ::memcpy(buffer, msg, msg->_packetLength);

    // 发送
    IFS_MsgTransfer *transfer = NULL;
    _locker.Lock();
    if(!_isClose)
    {
        auto iterTransfer = _sessionIdRefTransfer.find(sessionId);
        if(iterTransfer != _sessionIdRefTransfer.end())
            transfer = iterTransfer->second;

        if(transfer)
            transfer->OnSendData(sessionId, reinterpret_cast<NetMsg_DataHeader *>(buffer));
    }
    _locker.Unlock();

    // 未发出去（会话断开）
    if(!transfer)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(buffer);
        g_MemoryPool->Unlock();
    }
}

void FS_IocpMsgDispatcher::_OnBusinessProcessThread(const FS_ThreadPool *pool)
{// 业务层可以不用很频繁唤醒，只等待网络层推送消息过来

    _timeWheel->GetModifiedResolution(_resolutionInterval);
    while(!pool->IsClearingPool())
    {
        _locker.Lock();
        // g_Log->any<FS_IocpMsgDispatcher>("resolution interval [%lld]", _resolutionInterval.GetTotalMilliSeconds());
        _locker.Wait(static_cast<ULong>(_resolutionInterval.GetTotalMilliSeconds())); // 内部有lock若，发生死锁wait出不来
        _locker.Unlock();

        // Sleep(100);
        // 先执行定时器事件
        _timeWheel->RotateWheel();

        // 从异步消息队列取得异步处理完成返回事件 TODO: 需要有异步处理队列，其他线程塞入
        // 清理完成的异步事件 TODO:

        // 再执行业务事件
        _OnBusinessProcessing();

        // 投递业务产生的异步处理事件 TODO:
        _timeWheel->GetModifiedResolution(_resolutionInterval);
    }
}

void FS_IocpMsgDispatcher::_MoveToBusinessLayer(IFS_Session *session, NetMsg_DataHeader *msgData)
{
    // TODO:转移消息到业务处理层
    // 只需要sessionId与数据拷贝到业务处理层即可
    const UInt64 sessionId = session->GetSessionId();
    auto iterMsgs = _sessionIdRefMsgs.find(sessionId);
    if(iterMsgs == _sessionIdRefMsgs.end())
        iterMsgs = _sessionIdRefMsgs.insert(std::make_pair(sessionId, new std::list<NetMsg_DataHeader *>)).first;
    _newMsgSessionIds.insert(sessionId);

    // 拷贝数据
    g_MemoryPool->Lock();
    char *buffer = g_MemoryPool->Alloc<char>(msgData->_packetLength);
    g_MemoryPool->Unlock();
    ::memcpy(buffer, msgData, msgData->_packetLength);
    iterMsgs->second->push_back(reinterpret_cast<NetMsg_DataHeader *>(buffer));
}

void FS_IocpMsgDispatcher::_OnBusinessProcessing()
{
    // 将网络数据转移到缓冲区
    _locker.Lock();
    UInt64 sessionId = 0;
    std::list<NetMsg_DataHeader *> *temp = NULL;
    for(auto &sessionId : _newMsgSessionIds)
    {
        auto iterMsgList = _sessionIdRefMsgs.find(sessionId);
        if(iterMsgList == _sessionIdRefMsgs.end())
            continue;

        auto iterMsgCahceList = _sessionIdRefMsgCache.find(sessionId);
        if(iterMsgCahceList == _sessionIdRefMsgCache.end())
            iterMsgCahceList = _sessionIdRefMsgCache.insert(std::make_pair(sessionId, new std::list<NetMsg_DataHeader *>)).first;

        temp = iterMsgCahceList->second;
        iterMsgCahceList->second = iterMsgList->second;
        iterMsgList->second = temp;
    }
    _newMsgSessionIds.clear();
    _delayDisconnectedSessionsCache = _delayDisconnectedSessions;
    _delayDisconnectedSessions.clear();
    _locker.Unlock();

    // 处理业务逻辑
    temp = NULL;
    for(auto iterMsgList = _sessionIdRefMsgCache.begin(); 
        iterMsgList != _sessionIdRefMsgCache.end(); 
        ++iterMsgList)
    {
        temp = iterMsgList->second;
        for(auto &msg : *temp)
        {
            // 真正的业务处理，单线程执行，线程安全
            _DoBusinessProcess(iterMsgList->first, msg);
            g_MemoryPool->Lock();
            g_MemoryPool->Free(msg);
            g_MemoryPool->Unlock();
        }
        temp->clear();
    }

    // 延迟断开连接
    for(auto &sessionId : _delayDisconnectedSessionsCache)
        _OnDelaySessionDisconnect(sessionId);
    _delayDisconnectedSessionsCache.clear();
}

void FS_IocpMsgDispatcher::_DoBusinessProcess(UInt64 sessionId, NetMsg_DataHeader *msgData)
{
    // TODO:处理单一消息业务逻辑部分
    _logic->OnMsgDispatch(sessionId, msgData);
}

void FS_IocpMsgDispatcher::_OnDelaySessionDisconnect(UInt64 sessionId)
{
    // TODO:真实的session断开
    _logic->OnSessionDisconnected(sessionId);

    // 移除消息
    _locker.Lock();
    {// 真实数据链表
        auto iterMsgs = _sessionIdRefMsgs.find(sessionId);
        if(iterMsgs != _sessionIdRefMsgs.end())
        {
            bool hasMsgNotHandle = false;
            for(auto &msg : *iterMsgs->second)
            {
                hasMsgNotHandle = true;
                g_MemoryPool->Lock();
                g_MemoryPool->Free(msg);
                g_MemoryPool->Unlock();
            }

            if(hasMsgNotHandle)
                g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("sessionId[%llu] has msg not handle in msg cache when disconnect"), sessionId);

            Fs_SafeFree(iterMsgs->second);
            _sessionIdRefMsgs.erase(iterMsgs);
        }
    }

    {
        auto iterMsgs = _sessionIdRefMsgCache.find(sessionId);
        if(iterMsgs != _sessionIdRefMsgCache.end())
        {
            bool hasMsgNotHandle = false;
            for(auto &msg : *iterMsgs->second)
            {
                hasMsgNotHandle = true;
                g_MemoryPool->Lock();
                g_MemoryPool->Free(msg);
                g_MemoryPool->Unlock();
            }

            if(hasMsgNotHandle)
                g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("sessionId[%llu] has msg not handle in msg cache when disconnect"), sessionId);

            Fs_SafeFree(iterMsgs->second);
            _sessionIdRefMsgCache.erase(iterMsgs);
        }
    }
    _locker.Unlock();
}

FS_NAMESPACE_END


