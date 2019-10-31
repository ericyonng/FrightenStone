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
 * @file  : FS_IocpMsgTransfer.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "base/common/net/Impl/FS_IocpMsgTransfer.h"
#include "base/common/net/Impl/IFS_Session.h"
#include "base/common/net/Impl/FS_IocpSession.h"
#include "base/common/net/Defs/FS_IocpBuffer.h"
#include "base/common/net/Defs/IocpDefs.h"
#include "base/common/net/Impl/FS_Iocp.h"

#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/log/Log.h"
#include "base/common/crashhandle/CrashHandle.h"
#include "base/common/socket/socket.h"
#include "base/common/net/Impl/FS_Addr.h"

FS_NAMESPACE_BEGIN
FS_IocpMsgTransfer::FS_IocpMsgTransfer(Int32 id)
    :_threadPool(NULL)
    ,_iocp(NULL)
    ,_ioEvent(NULL)
    ,_serverCoreDisconnectedCallback(NULL)
    ,_serverCoreRecvSucCallback(NULL)
    ,_serverCoreRecvAmountCallback(NULL)
    ,_serverCoreSendSucCallback(NULL)
    ,_serverCoreHeartBeatTimeOutCallback(NULL)
    ,_sessionCnt{0}
    ,_hasNewSessionLinkin(false)
    ,_isSendCacheDirtied(false)
    ,_id(id)
{
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
    Fs_SafeFree(_serverCoreRecvAmountCallback);
    Fs_SafeFree(_serverCoreSendSucCallback);
    Fs_SafeFree(_serverCoreRecvSucCallback);
    Fs_SafeFree(_serverCoreDisconnectedCallback);
    Fs_SafeFree(_serverCoreHeartBeatTimeOutCallback);
    Fs_SafeFree(_threadPool);
    Fs_SafeFree(_iocp);
    Fs_SafeFree(_ioEvent);
}

Int32 FS_IocpMsgTransfer::BeforeStart()
{
    _threadPool = new FS_ThreadPool(0, 1);
    _iocp = new FS_Iocp;
    _ioEvent = new IO_EVENT;
    const Int32 st = _iocp->Create();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("create iocp fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransfer::Start()
{
    auto task = DelegatePlusFactory::Create(this, &FS_IocpMsgTransfer::_OnMoniterMsg);
    if(!_threadPool->AddTask(task, this))
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("addtask fail"));
        return StatusDefs::IocpMsgTransfer_StartFailOfMoniterMsgFailure;
    }

    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::BeforeClose()
{
    // close所有会话，使得投递的消息马上返回
    for(auto &session : _sessions)
        session.second->Close();
}

void FS_IocpMsgTransfer::Close()
{
    // 等待所有会话被移除
    while(true)
    {
        if(_sessionCnt <= 0)
            break;
        Sleep(1000);
    }

    // 投递iocpquit事件
    const auto st = _iocp->PostQuit();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("post quit error st[%d]"), st);
    }

    // 线程退出
    _threadPool->Clear();
    _iocp->Destroy();
}

void FS_IocpMsgTransfer::AfterClose()
{
    Fs_SafeFree(_serverCoreDisconnectedCallback);
}

void FS_IocpMsgTransfer::OnConnect(IFS_Session *session)
{
    _connectorGuard.Lock();
    _linkSessionCache.push_back(session);
    _hasNewSessionLinkin = true;
    _connectorGuard.Unlock();
    ++_sessionCnt;
}

void FS_IocpMsgTransfer::OnDestroy()
{

}

void FS_IocpMsgTransfer::OnHeartBeatTimeOut(IFS_Session *session)
{

}

void FS_IocpMsgTransfer::AsynSend(UInt64 sessionId, NetMsg_DataHeader *msg)
{
    _asynSendGuard.Lock();
    auto iterQueue = _asynSendMsgQueueCache.find(sessionId);
    if(iterQueue == _asynSendMsgQueueCache.end())
        iterQueue = _asynSendMsgQueueCache.insert(std::make_pair(sessionId, new std::list<NetMsg_DataHeader *>)).first;
    auto queueList = iterQueue->second;
    queueList->push_back(msg);
    _isSendCacheDirtied = true;
    _asynSendGuard.Unlock();
}

void FS_IocpMsgTransfer::RegisterDisconnected(IDelegate<void, IFS_Session *> *callback)
{
    Fs_SafeFree(_serverCoreDisconnectedCallback);
    _serverCoreDisconnectedCallback = callback;
}

void FS_IocpMsgTransfer::RegisterRecvSucCallback(IDelegate<void, IFS_Session *, Int64> *callback)
{
    Fs_SafeFree(_serverCoreRecvSucCallback);
    _serverCoreRecvSucCallback = callback;
}

void FS_IocpMsgTransfer::RegisterRecvAmountCallback(IDelegate<void, std::list<IFS_Session *> &> *callback)
{
    Fs_SafeFree(_serverCoreRecvAmountCallback);
    _serverCoreRecvAmountCallback = callback;
}

void FS_IocpMsgTransfer::RegisterSendSucCallback(IDelegate<void, IFS_Session *, Int64> *callback)
{
    Fs_SafeFree(_serverCoreSendSucCallback);
    _serverCoreSendSucCallback = callback;
}

void FS_IocpMsgTransfer::RegisterHeatBeatTimeOutCallback(IDelegate<void, IFS_Session *> *callback)
{
    Fs_SafeFree(_serverCoreHeartBeatTimeOutCallback);
    _serverCoreHeartBeatTimeOutCallback = callback;
}

Int32 FS_IocpMsgTransfer::GetSessionCnt()
{
    return _sessionCnt;
}

void FS_IocpMsgTransfer::_OnMoniterMsg(FS_ThreadPool *pool)
{// iocp 在closesocket后会马上返回所有投递的事件，所以不可立即在post未结束时候释放session对象

    ULong waitTime = 1;   // TODO可以调节（主要用于心跳检测）
    Int32 ret = StatusDefs::Success;
    while(!pool->IsClearingPool() || _sessionCnt > 0)
    {
        // 1.将连入的session加入
        _LinkCacheToSessions();

        // 3.判断是否有session
        if(_sessions.empty())
        {
            SocketUtil::Sleep(1);
            continue;
        }

        // 4.心跳超时检测（过期的放入toremove队列）
        _CheckSessionHeartbeat();

        // 5.post session to iocp
        // _PostEventsToIocp();

        // 6.donetevent 将断开的客户端放入toremove队列,将sessions放入torecv与tosend队列
        ret = _DoEvents();
        if(ret != StatusDefs::Success)
        {
            g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("do events error ret[%d]"), ret);
            _RemoveSessions();
            break;
        }

        // 6.domsg
        _OnMsgArrived();

        // 7.removesessions
        _RemoveSessions();
    }

    g_Log->sys<FS_IocpMsgTransfer>(_LOGFMT_("transfer thread end"));

    // 销毁sessions 此时要保证handler要还没关闭
    _ClearSessionsWhenClose();
}

Int32 FS_IocpMsgTransfer::_DoEvents()
{
    Int32 ret = StatusDefs::Success;
    FS_IocpSession *session = NULL;
    for(auto iter = _sessions.begin(); iter != _sessions.end(); )
    {
        session = iter->second->CastTo<FS_IocpSession>();
        //需要写数据的客户端,才postSend
        if(session->HasMsgToSend())
        {
            auto ioData = session->MakeSendIoData();
            if(ioData)
            {
                if(_iocp->PostSend(session->GetSocket(), ioData) != StatusDefs::Success)
                {
                    _OnDisconnected(session);
                    iter = _sessions.erase(iter);
                    continue;
                }
            }

            //--
            ioData = session->MakeRecvIoData();
            if(ioData)
            {
                if(_iocp->PostRecv(session->GetSocket(), ioData) != StatusDefs::Success)
                {
                    _OnDisconnected(session);
                    iter = _sessions.erase(iter);
                    continue;
                }
            }
        }
        else {
            auto ioData = session->MakeRecvIoData();
            if(ioData)
            {
                if(_iocp->PostRecv(session->GetSocket(), ioData) != StatusDefs::Success)
                {
                    _OnDisconnected(session);
                    iter = _sessions.erase(iter);
                    continue;
                }
            }
        }
        iter++;
    }

    while(true)
    {
        ret = _HandleNetEvent();
        if(ret == StatusDefs::IOCP_WaitTimeOut)
        {
            return StatusDefs::Success;
        }
        else if(ret != StatusDefs::Success)
        {
            return ret;
        }
    }

    return ret;
}

Int32 FS_IocpMsgTransfer::_HandleNetEvent()
{
    auto ret = _iocp->WaitForCompletion(*_ioEvent, 20);
    if(ret != StatusDefs::Success)
        return ret;

    // 1.判断会话是否存在
    const UInt64 sessionId = _ioEvent->_data._sessionId;
    auto session = _GetSession(sessionId);
    if(!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
        g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
                                     , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return StatusDefs::Success;
    }

    // 2.处理接收与发送
    auto iocpSession = session->CastTo<FS_IocpSession>();
    if(IocpDefs::IO_RECV == _ioEvent->_ioData->_ioType)
    {
        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%lu] will disconnect"
                                           , iocpSession->GetSessionId()
                                           , iocpSession->GetSocket(),
                                           _ioEvent->_bytesTrans);

            iocpSession->ResetPostRecvMask();
            _OnGracefullyDisconnect(session);
            return StatusDefs::Success;
        }

        iocpSession->OnRecvSuc(_ioEvent->_bytesTrans, _ioEvent->_ioData);

        // 消息接收回调
        _serverCoreRecvSucCallback->Invoke(session, _ioEvent->_bytesTrans);
        _msgArriviedSessions.push_back(session);
        _UpdateSessionHeartbeat(session);

//        g_Log->any<FS_IocpMsgTransfer>("sessionId[%llu] transfer[%lu] recv msg", sessionId, _ioEvent->_bytesTrans);

//         // 重新投递接收
//         if(iocpSession->CanPost())
//         {
//             _toPostRecv.insert(session);
//         }
//         else
//         {
//         }
    }
    else if(IocpDefs::IO_SEND == _ioEvent->_ioData->_ioType)
    {
        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] IO_TYPE::IO_SEND bytesTrans[%lu] disconnected"
                                           , session->GetSessionId()
                                           , session->GetSocket(),
                                           _ioEvent->_bytesTrans);
            iocpSession->ResetPostSendMask();
            _OnGracefullyDisconnect(session);
            return StatusDefs::Success;
        }

        iocpSession->OnSendSuc(_ioEvent->_bytesTrans, _ioEvent->_ioData);
//        g_Log->any<FS_IocpMsgTransfer>("send suc sessionId[%llu] transfer[%lu] ", sessionId, _ioEvent->_bytesTrans);
        // 消息发送回调
        _serverCoreSendSucCallback->Invoke(session, _ioEvent->_bytesTrans);

        _UpdateSessionHeartbeat(session);

        // 重新投递接收
//         if(iocpSession->CanPost() && iocpSession->HasMsgToSend())
//         {
//             _toPostSend.insert(session);
//         }
    }
    else
    {
        iocpSession->EnableDisconnect();
        _toRemove.insert(session);
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("undefine io type[%d]."), _ioEvent->_ioData->_ioType);
    }

    // 客户端已经断开连接
    if(iocpSession->IsClose())
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("session is closed sessionId[%llu]."), sessionId);
        // iocpSession->EnableDisconnect();
        _toRemove.insert(session);
    }

    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::_OnMsgArrived()
{
    // 处理消息到达
    // _serverCoreRecvAmountCallback->Invoke(_msgArriviedSessions);
    g_ServerCore->_OnRecvMsgAmount(_msgArriviedSessions, _toPostSend, _id);
}

void FS_IocpMsgTransfer::_RemoveSession(IFS_Session *session)
{
    _sessions.erase(session->GetSessionId());
    _OnDisconnected(session);
}

void FS_IocpMsgTransfer::_OnDelayDisconnected(IFS_Session *session)
{
    session->MaskClose();
    CancelIoEx(HANDLE(session->GetSocket()), NULL);
//     if(!session->IsClose())
//         session->Close();
}

void FS_IocpMsgTransfer::_OnDisconnected(IFS_Session *session)
{
//     g_Log->any<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] disconnected"
//                                    , session->GetSessionId()
//                                    , session->GetSocket());
    g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] disconnected address<%s>"
                                   , session->GetSessionId()
                                   , session->GetSocket(), session->GetAddr()->ToString().c_str());

    // 取消所有未决的io请求
    //CancelIoEx(HANDLE(session->GetSocket()), NULL);
    session->Close();
    _sessionHeartbeatQueue.erase(session);

    // servercore收到断开回调 不要回调到本模块线程安全的接口避免死锁
    _serverCoreDisconnectedCallback->Invoke(session);

    session->OnDisconnect();
    session->OnDestroy();

    // 移除会话
//     _sessions.erase(session->GetSessionId());
//     _toPostRecv.erase(session);
//     _toPostSend.erase(session);
    Fs_SafeFree(session);
    --_sessionCnt;
}

bool FS_IocpMsgTransfer::_DoPostSend(FS_IocpSession *session, bool removeIfFail)
{
    auto ioData = session->MakeSendIoData();
    if(ioData)
    {
        Int32 st = _iocp->PostSend(session->GetSocket(), ioData);
        if(st != StatusDefs::Success)
        {
            session->ResetPostSendMask();
            if(st != StatusDefs::IOCP_ClientForciblyClosed)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] socket[%llu] post send fail st[%d]")
                                             , session->GetSessionId(), session->GetSocket(), st);
            }

            if(removeIfFail)
                _OnGracefullyDisconnect(session);
            return false;
        }
    }

    return true;
}

bool FS_IocpMsgTransfer::_DoPostRecv(FS_IocpSession *session, bool removeIfFail)
{
    auto ioData = session->MakeRecvIoData();
    if(ioData)
    {
        Int32 st = _iocp->PostRecv(session->GetSocket(), ioData);
        if(st != StatusDefs::Success)
        {
            session->ResetPostRecvMask();
            if(st != StatusDefs::IOCP_ClientForciblyClosed)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] socket[%llu] post recv fail st[%d]")
                                             , session->GetSessionId(), session->GetSocket(), st);
            }

            if(removeIfFail)
                _OnGracefullyDisconnect(session);
            return false;
        }
    }

    return true;
}

IFS_Session *FS_IocpMsgTransfer::_GetSession(UInt64 sessionId)
{
    auto iterSession = _sessions.find(sessionId);
    if(iterSession == _sessions.end())
        return NULL;

    return iterSession->second;
}

void FS_IocpMsgTransfer::_ClearSessionsWhenClose()
{
    for(auto &iterSession : _sessions)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] not destroy when thread ending")
                                     , iterSession.first);
        _OnDisconnected(iterSession.second);
    }
}

void FS_IocpMsgTransfer::_UpdateSessionHeartbeat(IFS_Session *session)
{
    _sessionHeartbeatQueue.erase(session);
    session->UpdateHeartBeatExpiredTime();
    _sessionHeartbeatQueue.insert(session);
}

void FS_IocpMsgTransfer::_CheckSessionHeartbeat()
{
    _curTime.FlushTime();
    for(auto iterSession = _sessionHeartbeatQueue.begin(); 
        iterSession != _sessionHeartbeatQueue.end(); 
        )
    {
        auto session = (*iterSession)->CastTo<FS_IocpSession>();
        if(session->GetHeartBeatExpiredTime() > _curTime)
            break;

        _serverCoreHeartBeatTimeOutCallback->Invoke(session);
        iterSession = _sessionHeartbeatQueue.erase(iterSession);
        if(session->IsPostIoChange())
        {
            session->Close();
        }
        else
        {
            _RemoveSession(session);
        }
//        g_Log->any<FS_IocpMsgTransfer>("nowTime[%lld][%s] sessionId[%llu] expiredtime[%llu][%s] heartbeat time out."
//                                       , _curTime.GetMicroTimestamp(), _curTime.ToStringOfMillSecondPrecision().c_str()
//                                       ,session->GetSessionId()
//                                       , session->GetHeartBeatExpiredTime().GetMicroTimestamp()
//                                       , session->GetHeartBeatExpiredTime().ToStringOfMillSecondPrecision().c_str());

        // 心跳过期 到ondisconnected才真正移除
        // OnHeartBeatTimeOut(session);
    }
}

void FS_IocpMsgTransfer::_PostEventsToIocp()
{
    FS_IocpSession *session = NULL;
    for(auto iterRecv = _toPostRecv.begin(); iterRecv != _toPostRecv.end();)
    {
        session = (*iterRecv)->CastTo<FS_IocpSession>();
        if(!session->CanPost())
        {
            session->MaskClose();
            session->Close();
            session->ResetAllIoMask();
            _toRemove.insert(session);
            iterRecv = _toPostRecv.erase(iterRecv);
            continue;
        }

        if(!_DoPostRecv(session, false))
        {
            session->MaskClose();
            session->Close();
            session->ResetAllIoMask();
            _toRemove.insert(session);
            g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("post recv fail sessionId[%llu]"), session->GetSessionId());
        }

        iterRecv = _toPostRecv.erase(iterRecv);
    }

    for(auto iterSend = _toPostSend.begin(); iterSend != _toPostSend.end();)
    {
        session = (*iterSend)->CastTo<FS_IocpSession>();
        if(!session->CanPost())
        {
            session->MaskClose();
            session->Close();
            session->ResetAllIoMask();
            _toRemove.insert(session);
            iterSend = _toPostSend.erase(iterSend);
            continue;
        }

        if(!_DoPostSend(session, false))
        {
            session->MaskClose();
            session->Close();
            session->ResetAllIoMask();
            _toRemove.insert(session);
            g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("post send fail sessionId[%llu]"), session->GetSessionId());
        }
        iterSend = _toPostSend.erase(iterSend);
    }
}

void FS_IocpMsgTransfer::_AsynSendFromDispatcher(std::set<IFS_Session *> &toRemove)
{
    // 主动向客户端发送数据
    if(_isSendCacheDirtied)
    {
        std::list<NetMsg_DataHeader *> *msgs = NULL;
        _asynSendGuard.Lock();
        _isSendCacheDirtied = false;
        for(auto iterQueueCache = _asynSendMsgQueueCache.begin();iterQueueCache!= _asynSendMsgQueueCache.end();)
        {
            _asynSendMsgQueue.insert(std::make_pair(iterQueueCache->first, iterQueueCache->second));
            iterQueueCache = _asynSendMsgQueueCache.erase(iterQueueCache);
        }
        _asynSendGuard.Unlock();

        msgs = NULL;
        for(auto iterMsgs= _asynSendMsgQueue.begin();iterMsgs!=_asynSendMsgQueue.end();)
        {
            auto iterSession = _sessions.find(iterMsgs->first);
            if(iterSession == _sessions.end())
            {
                g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] has already disconnected or is not existed before"), iterMsgs->first);
                _FreeSendList(iterMsgs->second);
                iterMsgs = _asynSendMsgQueue.erase(iterMsgs);
                continue;
            }

            auto iocpSession = iterSession->second->CastTo<FS_IocpSession>();
            auto msgList = iterMsgs->second;
            for(auto &msg : *msgList)
            {
                if(iocpSession->CanPost())
                {
                    if(!iocpSession->Send(msg))
                    {
                        g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("sessionid[%llu] send msg fail"), iterMsgs->first);
                    }
                }

                // 释放
                g_MemoryPool->Lock();
                g_MemoryPool->Free(msg);
                g_MemoryPool->Unlock();
            }

            // post失败后iocpSession 立即会被释放或者延迟释放
            if(!_DoPostSend(iocpSession, false))
            {
                toRemove.insert(iocpSession);
                g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("post send fail sessionId[%llu]"), iterMsgs->first);
            }

            Fs_SafeFree(msgList);
            iterMsgs = _asynSendMsgQueue.erase(iterMsgs);
        }
    }
}

void fs::FS_IocpMsgTransfer::_FreeSendList(std::list<NetMsg_DataHeader *> *sendQueue)
{
    for(auto &msg : *sendQueue)
    {
        g_MemoryPool->Lock();
        g_MemoryPool->Free(msg);
        g_MemoryPool->Unlock();
    }
    Fs_SafeFree(sendQueue);
}

void FS_IocpMsgTransfer::_LinkCacheToSessions()
{
    if(_hasNewSessionLinkin)
    {
        _connectorGuard.Lock();
        for(auto iterSession = _linkSessionCache.begin(); iterSession != _linkSessionCache.end();)
        {
            auto session = *iterSession;
            _sessions.insert(std::make_pair(session->GetSessionId(), session));
            auto iocpSession = session->CastTo<FS_IocpSession>();

            // 绑定iocp
            auto st = _iocp->Reg(iocpSession->GetSocket(), iocpSession->GetSessionId());
            if(st != StatusDefs::Success)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("reg socket[%llu] sessionId[%llu] fail st[%d]")
                                             , session->GetSocket(), session->GetSessionId(), st);
            }

            // 投递接收数据
            if(!_DoPostRecv(iocpSession))
            {
                g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("post recv fail sessionId[%llu] serverId[%d]", session->GetSessionId(), _id));
                _RemoveSession(session);
            }
            else
            {
                _UpdateSessionHeartbeat(session);
            }

            iterSession = _linkSessionCache.erase(iterSession);
        }
        _hasNewSessionLinkin = false;
        _connectorGuard.Unlock();
    }
}

FS_NAMESPACE_END

