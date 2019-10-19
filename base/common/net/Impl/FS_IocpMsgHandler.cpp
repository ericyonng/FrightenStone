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
 * @file  : FS_IocpMsgHandler.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgHandler.h"
#include "base/common/net/Impl/FS_IocpSession.h"
#include "base/common/net/Defs/FS_IocpBuffer.h"
#include "base/common/net/Impl/IFS_MsgTransfer.h"

#include "base/common/memorypool/memorypool.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/TimeWheel/TimeWheel.h"
#include "base/common/component/Impl/TimeSlice.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/component/Impl/TimeWheel/FS_Timer.h"
#include "base/common/assist/utils/utils.h"

FS_NAMESPACE_BEGIN

FS_IocpMsgHandler::FS_IocpMsgHandler()
    :_pool(NULL)
    ,_isClose(false)
    ,_timeWheel(NULL)
{

}

FS_IocpMsgHandler::~FS_IocpMsgHandler()
{
}

Int32 FS_IocpMsgHandler::BeforeStart()
{
    _resolutionInterval = 100 * Time::_microSecondPerMilliSecond;
    _timeWheel = new TimeWheel(_resolutionInterval);
    g_BusinessTimeWheel = _timeWheel;
    _pool = new FS_ThreadPool(0, 1);
    return StatusDefs::Success;
}

Int32 FS_IocpMsgHandler::Start()
{
    return StatusDefs::Success;
}

void FS_IocpMsgHandler::BeforeClose()
{
    _locker.Lock();
    _isClose = true;
    _sessionIdRefTransfer.clear();
    _locker.Unlock();

    // 线程退出
    _pool->Clear();
}

void FS_IocpMsgHandler::Close()
{
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
}

void FS_IocpMsgHandler::OnRecv(IFS_Session *session)
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

    // 更新心跳
    iocpSession->UpdateHeartBeatExpiredTime();
}

void FS_IocpMsgHandler::OnDisconnected(IFS_Session *session)
{
    _locker.Lock();
    _sessionIdRefTransfer.erase(session->GetSessionId());
    _delayDisconnectedSessions.insert(session->GetSessionId());
    _locker.Unlock();
}

void FS_IocpMsgHandler::OnConnect(UInt64 sessionId, IFS_MsgTransfer *transfer)
{
    _locker.Lock();
    if(!_isClose)
    {
        auto iterTransfer = _sessionIdRefTransfer.find(sessionId);
        if(iterTransfer == _sessionIdRefTransfer.end())
            _sessionIdRefTransfer.insert(std::make_pair(sessionId, transfer));

        _sessionIdRefMsgs.find(sessionId)
    }
    _locker.Unlock();
}

void FS_IocpMsgHandler::OnDestroy()
{
}

void FS_IocpMsgHandler::OnHeartBeatTimeOut()
{

}

void FS_IocpMsgHandler::SendData(UInt64 sessionId, NetMsg_DataHeader *msg)
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

void FS_IocpMsgHandler::_MoveToBusinessLayer(IFS_Session *session, NetMsg_DataHeader *msgData)
{
    // TODO:转移消息到业务处理层
    // 只需要sessionId与数据拷贝到业务处理层即可
    const UInt64 sessionId = session->GetSessionId();
    auto iterMsgs = _sessionIdRefMsgs.find(sessionId);
    if(iterMsgs == _sessionIdRefMsgs.end())
        iterMsgs = _sessionIdRefMsgs.insert(std::make_pair(sessionId, new std::list<NetMsg_DataHeader *>)).first;
    
    // 拷贝数据
    g_MemoryPool->Lock();
    char *buffer = g_MemoryPool->Alloc<char>(msgData->_packetLength);
    g_MemoryPool->Unlock();
    ::memcpy(buffer, msgData, msgData->_packetLength);
    iterMsgs->second->push_back(reinterpret_cast<NetMsg_DataHeader *>(buffer));
}

void FS_IocpMsgHandler::_OnBusinessProcessThread(const FS_ThreadPool *pool)
{// 业务层可以不用很频繁唤醒，只等待网络层推送消息过来

    _timeWheel->GetModifiedResolution(_resolutionInterval);
    while(!pool->IsClearingPool())
    {
        _locker.Lock();
        _locker.Wait(_resolutionInterval.GetTotalMilliSeconds());
        _locker.Unlock();

        // 先执行定时器事件
        _timeWheel->RotateWheel();

        // 从异步消息队列取得异步处理完成返回事件 TODO:
        // 清理完成的异步事件 TODO:

        // 再执行业务事件
        _OnBusinessProcessing();

        // 投递业务产生的异步处理事件 TODO:
        _timeWheel->GetModifiedResolution(_resolutionInterval);
    }
}

void FS_IocpMsgHandler::_OnBusinessProcessing()
{
    // 将网络数据转移到缓冲区
    _locker.Lock();
    UInt64 sessionId = 0;
    std::list<NetMsg_DataHeader *> *temp = NULL;
    for(auto iterMsgList = _sessionIdRefMsgs.begin(); 
        iterMsgList != _sessionIdRefMsgs.end(); 
        ++iterMsgList)
    {
        sessionId = iterMsgList->first;
        auto iterMsgCahceList = _sessionIdRefMsgCache.find(sessionId);
        if(iterMsgCahceList == _sessionIdRefMsgCache.end())
            iterMsgCahceList = _sessionIdRefMsgCache.insert(std::make_pair(sessionId, new std::list<NetMsg_DataHeader *>)).first;
        
        temp = iterMsgCahceList->second;
        iterMsgCahceList->second = iterMsgList->second;
        iterMsgList->second = temp;
    }
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

void FS_IocpMsgHandler::_DoBusinessProcess(UInt64 sessionId, NetMsg_DataHeader *msgData)
{

}

void FS_IocpMsgHandler::_OnDelaySessionDisconnect(UInt64 sessionId)
{
    // TODO:真实的session断开
}

FS_NAMESPACE_END


