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
#include "FrightenStone/common/net/Impl/FS_IocpMsgTransfer.h"

#include "FrightenStone/common/net/Defs/MessageBlockUtil.h"
///////////////////////////////////////以下需要清理
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Defs/BriefSessionInfo.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"'
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Impl/FS_IocpPoller.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"
#include "FrightenStone/common/socket/socket.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/memleak/memleak.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
FS_IocpMsgTransfer::FS_IocpMsgTransfer(UInt32 compId, IFS_NetEngine *netEngine)
    :IFS_MsgTransfer(compId, netEngine)
    , _threadPool(NULL)
    ,_netEngine(netEngine)
    ,_iocp(NULL)
    ,_ioEvent(NULL)
    ,_sessionCnt{0}
    ,_hasNewSessionLinkin(false)
    // ,_isSendCacheDirtied(false)
    ,_id(id)
    ,_messageQueue(NULL)
    ,_msgsFromDispatcher(NULL)
    ,_senderMessageQueue(NULL)
    ,_generatorId(0)
    ,_recvMsgList(NULL)
    ,_sessionBufferAlloctor(NULL)
    ,_canCreateNewNodeForAlloctor(true)
    ,_curAlloctorOccupiedBytes(0)
    ,_transferThreadId(0)
    ,_printAlloctorOccupiedInfo(NULL)
    ,_cfgs(NULL)
{
/*        _CrtMemCheckpoint(&s1);*/
}

FS_IocpMsgTransfer::~FS_IocpMsgTransfer()
{
    Fs_SafeFree(_threadPool);
    Fs_SafeFree(_iocp);
    Fs_SafeFree(_ioEvent);
    STLUtil::DelListContainer(*_msgsFromDispatcher);
    STLUtil::DelListContainer(*_recvMsgList);
    Fs_SafeFree(_msgsFromDispatcher);
    Fs_SafeFree(_recvMsgList);
    Fs_SafeFree(_sessionBufferAlloctor);
    Fs_SafeFree(_printAlloctorOccupiedInfo);
    Fs_SafeFree(_cfgs);

//     _CrtMemCheckpoint(&s2);
//     if(_CrtMemDifference(&s3, &s1, &s2))
//         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpMsgTransfer::BeforeStart(const NetEngineTotalCfgs &totalCfgs)
{
    _cfgs = new TransferCfgs;
    *_cfgs = totalCfgs._transferCfgs;
    size_t blockAmount = static_cast<size_t>(_cfgs->_prepareBufferPoolCnt);
    if(blockAmount <= 0)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("prepare buffer pool block amount is zero."));
        return StatusDefs::IocpMsgTransfer_CfgError;
    }

    _poller = new FS_IocpPoller(this, PollerDefs::MonitorType_Transfer);
    _poller->AttachMessageQueue(_concurrentMq);

    Int32 st = _poller->BeforeStart();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("_poller->BeforeStart fail st[%d]"), st);
        return st;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 旧模块
//     auto updateAlloctorOccupied = DelegatePlusFactory::Create(this, &FS_IocpMsgTransfer::_UpdateCanCreateNewNodeForAlloctor);
//     _sessionBufferAlloctor = new MemoryAlloctor(FS_BUFF_SIZE_DEF, blockAmount, updateAlloctorOccupied, &_canCreateNewNodeForAlloctor);
//     _sessionBufferAlloctor->InitMemory();
//     _printAlloctorOccupiedInfo = DelegatePlusFactory::Create(this, &FS_IocpMsgTransfer::_PrintAlloctorOccupiedInfo);
// 
//     _threadPool = new FS_ThreadPool(0, 1);
//     _iocp = new FS_Iocp;
//     _ioEvent = new IO_EVENT;
//     _msgsFromDispatcher = new std::list<FS_MessageBlock *>;
//     _recvMsgList = new std::list<FS_MessageBlock *>;
//     const Int32 st = _iocp->Create();
//     if(st != StatusDefs::Success)
//     {
//         g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("create iocp fail st[%d]"), st);
//         return st;
//     }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgTransfer::Start()
{
    Int32 st = _poller->Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("_poller->Start fail st[%d]"), st);
        return st;
    }
    
    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::BeforeClose()
{
    _poller->BeforeClose();
    ////////////////////////////////////////////////////////旧的模块
    // close所有会话，使得投递的消息马上返回
    for(auto &session : _sessions)
    {
        session.second->MaskClose();
        session.second->Close();
    }
}

void FS_IocpMsgTransfer::Close()
{
    _poller->Close();
    ////////////////////////////////////////////////////////////////////////////旧的模块
    // 等待所有会话被移除
    while(true)
    {
        if(_sessionCnt <= 0)
            break;

        SystemUtil::Sleep(1000);
    }

    // 投递iocpquit事件
    const auto st = _iocp->PostQuit();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("post quit error st[%d]"), st);
    }

    // 线程退出
    _threadPool->Close();

    if(!_msgsFromDispatcher->empty())
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sender has [%llu] unhandled msgs"), _msgsFromDispatcher->size());
        STLUtil::DelListContainer(*_msgsFromDispatcher);
        Fs_SafeFree(_msgsFromDispatcher);
    }
}

void FS_IocpMsgTransfer::AfterClose()
{
    _poller->AfterClose();
    ////////////////////////////////////////////////////////////////////////////旧的模块
    // 内存分配器占用情况打印结束
    g_MemleakMonitor->UnRegisterMemPoolPrintCallback(_transferThreadId);

    if(_sessionBufferAlloctor)
        _sessionBufferAlloctor->FinishMemory();
}

// void FS_IocpMsgTransfer::OnConnect(BriefSessionInfo *sessionInfo)
// {
//     _connectorGuard.Lock();
//     _pendingNewSessionInfos.push_back(sessionInfo);
//     _hasNewSessionLinkin = true;
//     ++_sessionCnt;
//     _connectorGuard.Unlock();
// }

void FS_IocpMsgTransfer::OnWillConnect(BriefSessionInfo *newSessionInfo)
{
    // TODO:连接消息消息
    auto newMessageBlock = MessageBlockUtil::BuildTransferWillConnectMessageBlock(_compId, _generatorId, _poller->GetNetModuleObj(), newSessionInfo);
    ++_sessionCnt;
#ifdef _DEBUG
    if(!_concurrentMq->Push(_generatorId, newMessageBlock))
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("concurrentmq push fail generatorId[%u] compid[%u] newSessionId[%llu] sock[%llu]")
                                     , _generatorId, _compId, newSessionInfo->_sessionId, newSessionInfo->_sock);
    }
#else
    _concurrentMq->Push(_generatorId, newMessageBlock);
#endif
}

void FS_IocpMsgTransfer::OnDestroy()
{

}

void FS_IocpMsgTransfer::OnHeartBeatTimeOut(IFS_Session *session)
{

}

void FS_IocpMsgTransfer::_OnMoniterMsg(FS_ThreadPool *pool)
{// iocp 在closesocket后会马上返回所有投递的事件，所以不可立即在post未结束时候释放session对象

    // 初始化配置
    _transferThreadId = SystemUtil::GetCurrentThreadId();
    g_MemleakMonitor->RegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    ULong waitTime = 1;   // TODO可以调节（主要用于心跳检测）
    Int32 ret = StatusDefs::Success;
    while(pool->IsPoolWorking() || _sessionCnt > 0)
    {
        // 1.将连入的session加入
        _LinkCacheToSessions();

        // 3.判断是否有session
        if(_sessions.empty())
        {
            _ClearSenderMessageQueue();
            SystemUtil::Sleep(1);
            continue;
        }

        // 4.心跳超时检测（过期的放入toremove队列）
        _CheckSessionHeartbeat();

        // postsend 消耗比较大
        _AsynHandleFromDispatcher();

        // 5.post session to iocp
        _PostEventsToIocp();

        // 6.donetevent 将断开的客户端放入toremove队列,将sessions放入torecv与tosend队列 到超时也有可能部分的post没完成！！！
        ret = _DoEvents();
        if(ret != StatusDefs::Success)
        {
            if(ret != StatusDefs::IocpMsgTransfer_IocpQuit)
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("do events error ret[%d]"), ret);
            else
            {
                g_Log->net<FS_IocpMsgTransfer>("transfer id[%d] threadId[%llu] iocp quit", _id, SystemUtil::GetCurrentThreadId());
            }

            _RemoveSessions(true);
            break;
        }

        // 6.domsg 消耗比较大，在高并发情况下可能会导致超时
        _OnMsgArrived();

        // 7.removesessions
        _RemoveSessions();
    }

    g_Log->sys<FS_IocpMsgTransfer>(_LOGFMT_("transfer id[%d] threadId[%llu] end"), _id, SystemUtil::GetCurrentThreadId());

    // 销毁sessions 此时要保证handler要还没关闭
    _iocp->Destroy();
    _ClearSessionsWhenClose();
}

Int32 FS_IocpMsgTransfer::_HandleNetEvent()
{
    auto ret = _iocp->WaitForCompletion(*_ioEvent, 1);
    if(ret != StatusDefs::Success)
        return ret;

    // 处理iocp退出
    if(_ioEvent->_data._code == IocpDefs::IO_QUIT)
    {
        g_Log->sys<FS_IocpMsgTransfer>(_LOGFMT_("FS_IocpMsgTransfer iocp退出 threadId<%llu> code=%lld")
                                     , SystemUtil::GetCurrentThreadId(), _ioEvent->_data._code);
        return StatusDefs::IocpMsgTransfer_IocpQuit;
    }

    // 1.判断会话是否存在
    const UInt64 sessionId = _ioEvent->_data._sessionId;
    auto session = _GetSession(sessionId);
    if(!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
//         g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
//                                      , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return StatusDefs::Success;
    }

    // 2.处理接收与发送
    if(IocpDefs::IO_RECV == _ioEvent->_ioData->_ioType)
    {
        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%lu] will disconnect"
                                           , session->GetSessionId()
                                           , session->GetSocket(),
                                           _ioEvent->_bytesTrans);

             session->_ResetPostRecvMask();
             _OnDelayDisconnected(session);
             _toRemove.insert(session);
            return StatusDefs::Success;
        }

        session->OnRecvSuc(_ioEvent->_bytesTrans, _ioEvent->_ioData);

        // 消息接收回调
        _netEngine->_OnRecvMsg(session, _ioEvent->_bytesTrans);
        // _OnMsgArrived(session);

        _msgArriviedSessions.push_back(session);
        _UpdateSessionHeartbeat(session);

        // 重新接收数据
        if(session->CanPost())
            _toPostRecv.insert(session);
    }
    else if(IocpDefs::IO_SEND == _ioEvent->_ioData->_ioType)
    {
        if(_ioEvent->_bytesTrans <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] IO_TYPE::IO_SEND bytesTrans[%lu] disconnected"
                                           , session->GetSessionId()
                                           , session->GetSocket(),
                                           _ioEvent->_bytesTrans);
             session->_ResetPostSendMask();
             _OnDelayDisconnected(session);
             _toRemove.insert(session);
            return StatusDefs::Success;
        }

        session->OnSendSuc(_ioEvent->_bytesTrans, _ioEvent->_ioData);

        // 消息发送回调
        _netEngine->_OnSendMsg(session, _ioEvent->_bytesTrans);

        // TODO:有被疯狂发包的风险，解决方案：心跳包协议+频繁发包计数+发包成功时更新时间戳
        _UpdateSessionHeartbeat(session);

        // 重新投递接收
        if(session->CanPost() &&
           !session->IsPostSend() &&
           session->HasMsgToSend())
            _toPostSend.insert(session);
    }
    else
    {
        session->EnableDisconnect();
        session->Close();
        _toRemove.insert(session);
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("undefine io type[%d]."), _ioEvent->_ioData->_ioType);
    }

    // 客户端已经断开连接
//     if(session->IsClose())
//     {
//         g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("session is closed sessionId[%llu]."), sessionId);
//         // iocpSession->EnableDisconnect();
//         _toRemove.insert(session);
//     }

    return StatusDefs::Success;
}

void FS_IocpMsgTransfer::_OnMsgArrived()
{
    if(_msgArriviedSessions.empty())
        return;

    // 处理消息到达
    FS_IocpSession *session = NULL;
    UInt64 sessionId = 0;
    for(auto iterSession = _msgArriviedSessions.begin(); iterSession != _msgArriviedSessions.end();)
    {
        session = *iterSession;
        sessionId = session->GetSessionId();
        auto recvBuffer = session->CastToRecvBuffer();
        while(recvBuffer->HasMsg())
        {
            auto frontMsg = recvBuffer->CastToData<NetMsg_DataHeader>();

            // 创建消息
            FS_NetMsgBufferBlock *newBlock = new FS_NetMsgBufferBlock;
            newBlock->_generatorId = _id;
            newBlock->_sessionId = sessionId;
            newBlock->_mbType = MessageBlockType::MB_NetMsgArrived;
            g_MemoryPool->Lock();
            newBlock->_buffer = g_MemoryPool->Alloc<Byte8>(frontMsg->_packetLength);
            g_MemoryPool->Unlock();
            ::memcpy(newBlock->_buffer, frontMsg, frontMsg->_packetLength);

            _recvMsgList->push_back(newBlock);
            _netEngine->_OnRecvMsgAmount(frontMsg);
            recvBuffer->PopFront(frontMsg->_packetLength);
        }

        iterSession = _msgArriviedSessions.erase(iterSession);
    }

    if(!_messageQueue->Push(_generatorId, _recvMsgList))
    {
        if(!_recvMsgList->empty())
            g_Log->memleak("_OnMsgArrived mem leak FS_MessageBlock cnt[%llu]", _recvMsgList->size());
        STLUtil::DelListContainer(*_recvMsgList);
    }
}

void FS_IocpMsgTransfer::_OnMsgArrived(FS_IocpSession *session)
{
    // TODO:断开了但同时收到消息处理不？目前暂时不处理
    if(_toRemove.find(session) != _toRemove.end())
        return;

    {
        auto sessionId = session->GetSessionId();
        auto recvBuffer = session->CastToRecvBuffer();
        while(recvBuffer->HasMsg())
        {
            auto frontMsg = recvBuffer->CastToData<NetMsg_DataHeader>();

            // 创建消息
            FS_NetMsgBufferBlock *newBlock = new FS_NetMsgBufferBlock;
            newBlock->_generatorId = _id;
            newBlock->_sessionId = sessionId;
            newBlock->_mbType = MessageBlockType::MB_NetMsgArrived;
            g_MemoryPool->Lock();
            newBlock->_buffer = g_MemoryPool->Alloc<Byte8>(frontMsg->_packetLength);
            g_MemoryPool->Unlock();
            ::memcpy(newBlock->_buffer, frontMsg, frontMsg->_packetLength);

            _recvMsgList->push_back(newBlock);
            _netEngine->_OnRecvMsgAmount(frontMsg);
            recvBuffer->PopFront(frontMsg->_packetLength);
        }
    }

    if(!_messageQueue->Push(_generatorId, _recvMsgList))
    {
        if(!_recvMsgList->empty())
            g_Log->memleak("_OnMsgArrived mem leak FS_MessageBlock cnt[%llu]", _recvMsgList->size());
        STLUtil::DelListContainer(*_recvMsgList);
    }
}

void FS_IocpMsgTransfer::_OnDelayDisconnected(FS_IocpSession *session)
{
    // 这个时候也有可能post刚好完成，所以不宜用closesocket，而是cacelio，使得已完成的不会被取消，且到此session即将关闭
    session->MaskClose();
    CancelIoEx(HANDLE(session->GetSocket()), NULL);
}

void FS_IocpMsgTransfer::_OnDisconnected(FS_IocpSession *session)
{
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] sock[%llu] disconnected address<%s>"
                                   , sessionId
                                   , session->GetSocket(), session->GetAddr()->ToString().c_str());

    // servercore收到断开回调 不要回调到本模块线程安全的接口避免死锁
    _sessionHeartbeatQueue.erase(session);
    _netEngine->_OnDisconnected(session);

    // 推送会话断开消息
    _NtySessionDisConnectMsg(sessionId);

    // 移除会话
     _sessions.erase(sessionId);
    _toPostRecv.erase(session);
    _toPostSend.erase(session);
    _DestroySession(session);
    --_sessionCnt;
}

void FS_IocpMsgTransfer::_DestroySession(FS_IocpSession *session)
{
    session->Close();
    session->OnDisconnect();
    session->OnDestroy();
    Fs_SafeFree(session);
}

bool FS_IocpMsgTransfer::_DoPostSend(FS_IocpSession *session)
{
    auto ioData = session->MakeSendIoData();
    if(ioData)
    {
        Int32 st = _iocp->PostSend(session->GetSocket(), ioData);
        if(st != StatusDefs::Success)
        {
            session->_ResetPostSendMask();
            _OnDelayDisconnected(session);
            if(st != StatusDefs::IOCP_ClientForciblyClosed)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] socket[%llu] post send fail st[%d]")
                                             , session->GetSessionId(), session->GetSocket(), st);
            }

            _toRemove.insert(session);
            return false;
        }
    }

    return true;
}

bool FS_IocpMsgTransfer::_DoPostRecv(FS_IocpSession *session)
{
    auto ioData = session->MakeRecvIoData();
    if(ioData)
    {
        Int32 st = _iocp->PostRecv(session->GetSocket(), ioData);
        if(st != StatusDefs::Success)
        {
            session->_ResetPostRecvMask();
            _OnDelayDisconnected(session);
            if(st != StatusDefs::IOCP_ClientForciblyClosed)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] socket[%llu] post recv fail st[%d]")
                                             , session->GetSessionId(), session->GetSocket(), st);
            }
            
            _toRemove.insert(session);
                //_OnGracefullyDisconnect(session);
            return false;
        }
    }

    return true;
}

void FS_IocpMsgTransfer::_NtySessionConnectedMsg(UInt64 sessionId, BriefSessionInfo *newSessionInfo)
{
    FS_NetMsgBufferBlock *newMsgBlock = new FS_NetMsgBufferBlock;
    newMsgBlock->_generatorId = _id;
    newMsgBlock->_mbType = MessageBlockType::MB_NetSessionConnected;
    newMsgBlock->_sessionId = sessionId;
    newMsgBlock->_newUserRes = newSessionInfo->_newUserRes;
    newSessionInfo->_newUserRes = NULL;
    newMsgBlock->_userDisconnected = newSessionInfo->_userDisconnectedRes;
    newSessionInfo->_userDisconnectedRes = NULL;
    if(!_messageQueue->Push(_id, newMsgBlock))
        Fs_SafeFree(newMsgBlock);
}

void FS_IocpMsgTransfer::_NtySessionDisConnectMsg(UInt64 sessionId)
{
    FS_NetMsgBufferBlock *newMsgBlock = new FS_NetMsgBufferBlock;
    newMsgBlock->_generatorId = _id;
    newMsgBlock->_mbType = MessageBlockType::MB_NetSessionDisconnect;
    newMsgBlock->_sessionId = sessionId;
    if(!_messageQueue->Push(_id, newMsgBlock))
        Fs_SafeFree(newMsgBlock);
}

void FS_IocpMsgTransfer::_ClearSessionsWhenClose()
{
    for(auto &iterSession : _sessions)
    {
        g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("sessionId[%llu] not destroy when thread ending")
                                     , iterSession.first);
        _DestroySession(iterSession.second);
    }
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

        _netEngine->_OnHeartBeatTimeOut(session);
        iterSession = _sessionHeartbeatQueue.erase(iterSession);
        session->MaskClose();
        _toRemove.insert(session);
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
        session = *iterRecv;
        if(session->CanPost())
            _DoPostRecv(session);

        iterRecv = _toPostRecv.erase(iterRecv);
    }

    for(auto iterSend = _toPostSend.begin(); iterSend != _toPostSend.end();)
    {
        session = *iterSend;
        if(!session->CanPost()||session->IsPostSend())
        {
            iterSend = _toPostSend.erase(iterSend);
            continue;
        }
        
        _DoPostSend(session);
        iterSend = _toPostSend.erase(iterSend);
    }
}

void FS_IocpMsgTransfer::_AsynHandleFromDispatcher()
{
    _senderMessageQueue->PopImmediately(_msgsFromDispatcher);

    // TODO:消息过多时候会死在循环中，严重拖慢速度（需要先按sessionId分类组成一个包队列）（需要在消息队列中定制分类方法回调（使用组合方式））
    // 返回：std::map<UInt64, std::list<FS_NetMsgBufferBlock *>> *队列
    for(auto iterBlock = _msgsFromDispatcher->begin(); iterBlock != _msgsFromDispatcher->end();)
    {
        FS_NetMsgBufferBlock *sendMsgBufferBlock = static_cast<FS_NetMsgBufferBlock *>(*iterBlock);
        if(sendMsgBufferBlock->_mbType == MessageBlockType::MB_NetMsgSended)
        {// 发送消息给客户端
            auto session = _GetSession(sendMsgBufferBlock->_sessionId);
            if(session && session->CanPost())
            {
                NetMsg_DataHeader *header = reinterpret_cast<NetMsg_DataHeader *>(sendMsgBufferBlock->_buffer);
                if(!session->PushMsgToSend(header))
                {
                    g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("sessionid[%llu] send msg cmd[%hu] len[%hu] fail")
                                                 , sendMsgBufferBlock->_sessionId
                                                 , header->_cmd, header->_packetLength);
                }

                if(!session->IsPostSend())
                {
                    _DoPostSend(session);
                    _toPostSend.erase(session);
                }
            }
        }
        else if(sendMsgBufferBlock->_mbType == MessageBlockType::MB_NetCloseSession)
        {// 关闭会话
            auto session = _GetSession(sendMsgBufferBlock->_sessionId);
            if(session)
            {
                session->MaskClose();
                _toRemove.insert(session);
            }
        }
        
        Fs_SafeFree(sendMsgBufferBlock);
        iterBlock = _msgsFromDispatcher->erase(iterBlock);
    }
}

void FS_IocpMsgTransfer::_ClearSenderMessageQueue()
{
    _senderMessageQueue->PopImmediately(_msgsFromDispatcher);

    // 提示有n条消息由于对端关闭而无法发出去
    if(!_msgsFromDispatcher->empty())
    {
        g_Log->net<FS_IocpMsgTransfer>("it has %llu messages unhandled which will send to remote session!", _msgsFromDispatcher->size());
        STLUtil::DelListContainer(*_msgsFromDispatcher);
    }
}

void FS_IocpMsgTransfer::_FreeSendList(std::list<NetMsg_DataHeader *> *sendQueue)
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
        for(auto iterSession = _pendingNewSessionInfos.begin(); iterSession != _pendingNewSessionInfos.end();)
        {
            auto &newSessionInfo = *iterSession;
            auto newSession = FS_SessionFactory::Create(*newSessionInfo, _sessionBufferAlloctor, _cfgs->_heartbeatDeadTimeMsInterval * Time::_microSecondPerMilliSecond)->CastTo<FS_IocpSession>();
            _sessions.insert(std::make_pair(newSessionInfo->_sessionId, newSession));
            newSession->OnConnect();
            auto addr = newSession->GetAddr();
            g_Log->net<FS_IocpMsgTransfer>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
                                         , newSession->GetSessionId()
                                         , newSession->GetSocket()
                                         , addr->GetAddr().c_str()
                                         , addr->GetPort());
            // 绑定iocp
            auto st = _iocp->Reg(newSessionInfo->_sock, newSessionInfo->_sessionId);
            if(st != StatusDefs::Success)
            {
                g_Log->e<FS_IocpMsgTransfer>(_LOGFMT_("reg socket[%llu] sessionId[%llu] fail st[%d]")
                                             , newSessionInfo->_sock, newSessionInfo->_sessionId, st);
            }

            // 推送会话连入消息
            _NtySessionConnectedMsg(newSessionInfo->_sessionId, newSessionInfo);

            // 投递接收数据
            if(!_DoPostRecv(newSession))
            {
                g_Log->w<FS_IocpMsgTransfer>(_LOGFMT_("post recv fail sessionId[%llu] serverId[%d]"), newSessionInfo->_sessionId, _id);
                _toRemove.insert(newSession);
            }
            else
            {
                _UpdateSessionHeartbeat(newSession);
            }

            Fs_SafeFree(newSessionInfo);
            iterSession = _pendingNewSessionInfos.erase(iterSession);
        }
        _hasNewSessionLinkin = false;
        _connectorGuard.Unlock();
    }
}


FS_NAMESPACE_END

#endif
