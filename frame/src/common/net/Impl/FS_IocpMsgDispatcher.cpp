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
#include "FrightenStone/common/net/Impl/FS_IocpMsgDispatcher.h"
#include "FrightenStone/common/net/Impl/FS_IocpSession.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Impl/IFS_BusinessLogic.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Defs/FS_NetMessageBlock.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"

#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/TimeWheel/TimeWheel.h"
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/component/Impl/TimeWheel/FS_Timer.h"
#include "FrightenStone/common/assist/utils/utils.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN

FS_IocpMsgDispatcher::FS_IocpMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId)
    :IFS_MsgDispatcher(netEngine, compId)
    , _pool(NULL)
    , _isClose{false}
    ,_timeWheel(NULL)
    ,_logic(NULL)
    ,_messgeQueue(NULL)
    ,_recvMsgBlocks(NULL)
    ,_cfgs(NULL)
{
}

FS_IocpMsgDispatcher::~FS_IocpMsgDispatcher()
{
    Fs_SafeFree(_pool);
    Fs_SafeFree(_timeWheel);
    Fs_SafeFree(_cfgs);

//     _CrtMemCheckpoint(&s2);
//     if(_CrtMemDifference(&s3, &s1, &s2))
//         _CrtMemDumpStatistics(&s3);
}

Int32 FS_IocpMsgDispatcher::BeforeStart(const NetEngineTotalCfgs &cfgs)
{
    Int32 st = IFS_EngineComp::BeforeStart(cfgs);
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("IFS_EngineComp::BeforeStart fail st[%d]"), st);
        return st;
    }

    _cfgs = new DispatcherCfgs;
    *_cfgs = cfgs._dispatcherCfgs;
    _timeWheel = new TimeWheel(_cfgs->_dispatcherResolutionInterval);
    _pool = new FS_ThreadPool(0, 1);

    _recvMsgBlocks = new std::vector<std::list<FS_MessageBlock *> *>;
    auto generatorQuatity = _concurrentMq->GetGeneratorQuality();
    _recvMsgBlocks->resize(generatorQuatity);
    for(UInt32 i = 0; i < generatorQuatity; ++i)
        (*_recvMsgBlocks)[i] = new std::list<FS_MessageBlock *>;

    /////////////////////////////////////////////////////////////////////////////////旧代码
    if(_logic)
    {
        _logic->SetDispatcher(this);
        _logic->SetTimeWheel(_timeWheel);
        auto st = _logic->BeforeStart();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("_logic->BeforeStart error st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_IocpMsgDispatcher::Start()
{
    Int32 st = IFS_EngineComp::Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("IFS_EngineComp::Start fail st[%d]"), st);
        return st;
    }

    if(_logic)
    {
        auto st = _logic->Start();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("_logic->Start error st[%d]"), st);
            return st;
        }
    }

    auto task = DelegatePlusFactory::Create(this, &FS_IocpMsgDispatcher::_OnBusinessProcessThread);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("add task fail"));
        return StatusDefs::FS_IocpMsgHandler_StartFailOfBusinessProcessThreadFailure;
    }

    return StatusDefs::Success;
}

void FS_IocpMsgDispatcher::BeforeClose()
{
    if(_isClose)
        return;

    _concurrentMq->NotifyConsumerByGenerator(_consumerId);

    if(_logic)
        _logic->BeforeClose();

    IFS_EngineComp::BeforeClose();
}

void FS_IocpMsgDispatcher::Close()
{
    if(_isClose)
        return;

    _isClose = true;

    // 线程退出
    _pool->Close();

    if(_logic)
        _logic->Close();

    if(!_recvMsgBlocks->empty())
    {
        UInt32 generatorSize = static_cast<UInt32>(_recvMsgBlocks->size());
        UInt64 blockCount = 0;
        for(UInt32 i = 0; i < generatorSize; ++i)
        {
            auto msgQueue = _recvMsgBlocks->at(i);
            if(!msgQueue)
                continue;

            for(auto &recvMsgBlock : *msgQueue)
            {
                FS_NetMsgBufferBlock *netMsg = recvMsgBlock->CastTo<FS_NetMsgBufferBlock>();
                g_Log->net<FS_IocpMsgDispatcher>("netmsg _compId[%u] _generatorId[%u] _netMessageBlockType[%d]is unhandled"
                                                 , netMsg->_compId
                                                 , netMsg->_generatorId
                                                 , netMsg->_netMessageBlockType);
                ++blockCount;
                Fs_SafeFree(recvMsgBlock);
            }
            msgQueue->clear();
        }

        g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("msg block queue has unhandled msgs[%llu]"), blockCount);
    }

    STLUtil::DelListContainer(*_recvMsgBlocks);
    Fs_SafeFree(_recvMsgBlocks);
    for(auto &iterDelegateInfo : _sessionIdRefUserDisconnected)
        STLUtil::DelListContainer(iterDelegateInfo.second);
    _sessionIdRefUserDisconnected.clear();

    IFS_EngineComp::Close();
}


void FS_IocpMsgDispatcher::_OnBusinessProcessThread(FS_ThreadPool *pool)
{// 业务层可以不用很频繁唤醒，只等待网络层推送消息过来

    // _timeWheel->GetModifiedResolution(_resolutionInterval);
    const auto &resolutionIntervalSlice = _cfgs->_dispatcherResolutionInterval;
    bool hasMsg = false;
    while(pool->IsPoolWorking())
    {
        // 1.等待消息队列
        _concurrentMq->WaitForPoping(_consumerId, _recvMsgBlocks, hasMsg, static_cast<ULong>(resolutionIntervalSlice.GetTotalMilliSeconds()));

        // 2.先执行定时器事件
        _timeWheel->RotateWheel();

        // 3.检查心跳
        _CheckHeartbeat();

        // 从异步消息队列取得异步处理完成返回事件 TODO: 需要有异步处理队列，其他线程塞入
        // 清理完成的异步事件 TODO:

        // 4.再执行业务事件
        if(hasMsg)
            _OnBusinessProcessing();

        // 5. 投递消息
        _PostEvents();

        // 6.移除sessions
        _DelayRemoveSessions();

        // 投递业务产生的异步处理事件 TODO: 不合适修正，因为可能会错过跨天等重要节点
        // _timeWheel->GetModifiedResolution(_resolutionInterval);
    }

    g_Log->sys<FS_IocpMsgDispatcher>(_LOGFMT_("dispatcher process thread end"));
}

void FS_IocpMsgDispatcher::_CheckHeartbeat()
{
    _curTime.FlushTime();
    for(auto iterSession = _sessionHeartbeatQueue.begin(); iterSession != _sessionHeartbeatQueue.end();)
    {
        auto session = *iterSession;
        if(session->GetHeartBeatExpiredTime() > _curTime)
            break;

        _engine->_HandleCompEv_HeartBeatTimeOut();
        iterSession = _sessionHeartbeatQueue.erase(iterSession);
        session->MaskClose();
        _toRemove.insert(session->CastTo<FS_IocpSession>());
    }
}


void FS_IocpMsgDispatcher::_OnBusinessProcessing()
{
    // 将网络数据转移到缓冲区
    FS_NetMsgBufferBlock *netMsgBlock = NULL;
    UInt64 sessionId = 0;
    for(auto iterGeneratorQueue = _recvMsgBlocks->begin(); iterGeneratorQueue != _recvMsgBlocks->end(); ++iterGeneratorQueue)
    {
        auto *blockQueue = *iterGeneratorQueue;
        if(blockQueue->empty())
            continue;

        for(auto iterBlock = blockQueue->begin(); iterBlock != blockQueue->end();)
        {
            netMsgBlock = (*iterBlock)->CastTo<FS_NetMsgBufferBlock>();
            sessionId = netMsgBlock->_sessionId;
            if(netMsgBlock->_netMessageBlockType == NetMessageBlockType::Net_NetSessionDisconnect)
            {// 会话断开
                _delayDisconnectedSessions.insert(sessionId);
            }
            else if(netMsgBlock->_netMessageBlockType == NetMessageBlockType::Net_NetMsgArrived)
            {// 收到网络包
                auto arrivedMsg = netMsgBlock->CastTo<FS_NetArrivedMsg>();
                _DoBusinessProcess(sessionId, netMsgBlock->_generatorId, netMsg);
            }
            else if(netMsgBlock->_netMessageBlockType == NetMessageBlockType::Met_NetSessionConnected)
            {// 会话连入
                auto connectedMsgBlock = netMsgBlock->CastTo<FS_NetSessionWillConnectMsg>();
                _OnSessionConnected(connectedMsgBlock);
            }

            Fs_SafeFree(netMsgBlock);
            iterBlock = blockQueue->erase(iterBlock);
        }
    }

    // 延迟断开连接
    for(auto iterSession = _delayDisconnectedSessions.begin();
        iterSession != _delayDisconnectedSessions.end();)
    {
        _OnDelaySessionDisconnect(*iterSession);
        iterSession = _delayDisconnectedSessions.erase(iterSession);
    }
}

void FS_IocpMsgDispatcher::_PostEvents()
{
    // post recv
    for(auto iterRecv = _toPostRecv.begin(); iterRecv != _toPostRecv.end();)
    {
        _DoPostRecv( *iterRecv);
        iterRecv = _toPostRecv.erase(iterRecv);
    }

    // post send
    for(auto iterSend = _toPostSend.begin(); iterSend != _toPostSend.end();)
    {
        _DoPostSend(*iterSend);
        iterSend = _toPostSend.erase(iterSend);
    }
}

void FS_IocpMsgDispatcher::_DelayRemoveSessions()
{
    FS_IocpSession *session = NULL;
    for(auto iterSession = _toRemove.begin(); iterSession!=_toRemove.end();)
    {
        session = *iterSession;
        _RemoveFromPostRecvQueue(session);
        _RemoveFromPostSendQueue(session);

        // 准备移除,等待transfer返回正式移除session
        _PrepareRemoveSession(session);
        iterSession = _toRemove.erase(iterSession);
    }
}

bool FS_IocpMsgDispatcher::_DoPostRecv(FS_IocpSession *session)
{
    // session不可post或者已经post完成不可重复post
    if(!session->CanPost() || session->IsPostRecv())
        return true;

    Int32 st = session->PostRecv();
    if(st != StatusDefs::Success)
    {
        _toRemove.insert(session);
        return false;
    }

    return true;
}

bool FS_IocpMsgDispatcher::_DoPostSend(FS_IocpSession *session)
{
    // 判断是否可以post,是否已经postsend,是否有消息发送
    if(!session->CanPost()
       || session->IsPostSend()
       || !session->HasMsgToSend())
        return true;

    Int32 st = session->PostSend();
    if(st != StatusDefs::Success)
    {
        _toRemove.insert(session);
        return false;
    }

    return true;
}

void FS_IocpMsgDispatcher::_PrepareRemoveSession(FS_IocpSession *session)
{// 仅仅close掉socket，并等待transfer返回,ondisconnected才是真正的断开
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s> prepare remove"
                                     , sessionId
                                     , session->GetSocket(), session->GetAddr()->ToString().c_str());

    session->MaskClose();
    if(!session->CanDisconnect())
    {// 还有post event还没返回不可立即关闭应该先cancel,保证已经完成的io事件正常返回
        session->CancelPostedEventsAndMaskClose();
    }
    else
    {
        // close会把在缓冲完全清除,若有post还未返回则会出现丢数据情况
        session->Close();
    }
}

void FS_IocpMsgDispatcher::_OnSessionDisconnected(FS_IocpSession *session)
{
    // 从session返回正式移除session
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s>"
                                   , sessionId
                                   , session->GetSocket(), session->GetAddr()->ToString().c_str());

    _sessionHeartbeatQueue.erase(session);
    _engine->_HandleCompEv_Disconnected(sessionId);
    _sessions.erase(sessionId);
    _toPostRecv.erase(session);
    _toPostSend.erase(session);

    session->Close();
    session->OnDisconnect();
    session->OnDestroy();
    Fs_SafeFree(session);
}

void FS_IocpMsgDispatcher::_OnSessionConnected(FS_NetSessionWillConnectMsg *connectedMsg)
{
    const UInt64 sessionId = connectedMsg->_sessionId;

    // 创建session对象并初始化
    auto newSession = FS_SessionFactory::Create(sessionId, connectedMsg->_sock, &connectedMsg->_addrInfo, _sessionBufferAlloctor, _cfgs->_heartbeatDeadTimeMsInterval * Time::_microSecondPerMilliSecond)->CastTo<FS_IocpSession>();
    _sessions.insert(std::make_pair(sessionId, newSession));
    newSession->Bind(reinterpret_cast<FS_Iocp *>(connectedMsg->_netModule));
    newSession->OnConnect();
    auto addr = newSession->GetAddr();
    g_Log->net<FS_IocpMsgDispatcher>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
                                   , sessionId
                                   , newSession->GetSocket()
                                   , addr->GetAddr().c_str()
                                   , addr->GetPort());

    auto newUser = _logic->OnSessionConnected(sessionId, connectedMsg->_generatorId);
    auto newUserRes = connectedMsg->_onNewUserRes;
    if(newUserRes)
        newUserRes->Invoke(newUser);

    auto userDisconnectedDelegates = connectedMsg->_onUserDisconnectedRes;
    if(userDisconnectedDelegates)
    {
        auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
        if(iterDisconnected == _sessionIdRefUserDisconnected.end())
            iterDisconnected = _sessionIdRefUserDisconnected.insert(std::make_pair(sessionId, std::list<IDelegate<void, IUser *>*>())).first;
        iterDisconnected->second.push_back(userDisconnectedDelegates);
        connectedMsg->_onUserDisconnectedRes = NULL;
    }

    // post recv 刷新心跳时间
    if(_DoPostRecv(newSession))
        newSession->UpdateHeartBeatExpiredTime();
}

void FS_IocpMsgDispatcher::_OnMsgArrived(FS_NetArrivedMsg *arrivedMsg)
{
    IoEvent *ioEv = arrivedMsg->_ioEv;
    IoDataBase *ioData = reinterpret_cast<IoDataBase *>(ioEv->_ioData);
    Int32 errorCode = arrivedMsg->_errorCode;
    const Int64 transferBytes = ioEv->_bytesTrans;
    if(errorCode != StatusDefs::Success)
    {
        g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("arrived msg errorCode[%d]"), errorCode);
        return;
    }

    if(ioData->_sessionId == static_cast<UInt64>(IocpDefs::IO_QUIT))
    {// transfer程序退出
        return;
    }

    const UInt64 sessionId = ioEv->_sessionId;
    auto session = _GetSession(sessionId);
    if(!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
//         g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
//                                      , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }


    // 2.处理接收与发送
    if(IocpDefs::IO_RECV == ioData->_ioType)
    {
        if(transferBytes <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%llu] will disconnect"
                                             , session->GetSessionId()
                                             , session->GetSocket(),
                                             transferBytes);

            session->ResetPostRecvMask();
            session->CancelPostedEventsAndMaskClose();

            _toRemove.insert(session);
            return;
        }

        session->OnRecvSuc(transferBytes, ioData);

        // 消息接收回调
        _engine->_HandleCompEv_RecvMsg(transferBytes);
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
            session->ResetPostSendMask();
            session->CancelPostedEventsAndMaskClose();
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
}

// 
// void FS_IocpMsgDispatcher::OnRecv(std::list<IFS_Session *> &sessions, Int64 &incPacketsCnt)
// {
//     incPacketsCnt = 0;
// 
//     _locker.Lock();
//     if(!_isClose)
//     {
//         IFS_Session *session = NULL;
//         bool hasMsg = false;
//         for(auto iterSession = sessions.begin(); iterSession != sessions.end();)
//         {
//             session = *iterSession;
//             auto iocpSession = session->CastTo<FS_IocpSession>();
//             auto recvBuffer = iocpSession->GetRecvBuffer()->CastToBuffer<FS_IocpBuffer>();
//             
//             hasMsg |= recvBuffer->HasMsg();
//             while(recvBuffer->HasMsg())
//             {
//                 auto frontMsg = recvBuffer->CastToData<NetMsg_DataHeader>();
//                 _MoveToBusinessLayer(session, frontMsg);
//                 recvBuffer->PopFront(frontMsg->_packetLength);
//                 ++incPacketsCnt;
//             }
// 
//             iterSession = sessions.erase(iterSession);
//         }
// 
//         if(hasMsg)
//         {
//             _isDataDirtied = true;
//             _locker.Sinal();
//         }
//     }    
//     _locker.Unlock();
// }

void FS_IocpMsgDispatcher::OnDestroy()
{
}

void FS_IocpMsgDispatcher::OnHeartBeatTimeOut()
{

}

void FS_IocpMsgDispatcher::SendData(UInt64 sessionId,  UInt64 consumerId,  NetMsg_DataHeader *msg)
{
    if(_isClose)
        return;

    // sender是一对一的需要找到对应的收到网络包的那个transfer 的id
    auto &senderMq = _netEngine->_GetSenderMq();

    // 新的待发送的消息
    FS_NetMsgBufferBlock *newMsgBlock = new FS_NetMsgBufferBlock;
    newMsgBlock->_mbType = MessageBlockType::MB_NetMsgSended;
    g_MemoryPool->Lock();
    newMsgBlock->_buffer = g_MemoryPool->Alloc<Byte8>(msg->_packetLength);
    g_MemoryPool->Unlock();
    ::memcpy(newMsgBlock->_buffer, msg, msg->_packetLength);
    newMsgBlock->_generatorId = 0;
    newMsgBlock->_sessionId = sessionId;

    // 送入消息队列
    if(!senderMq[consumerId]->Push(newMsgBlock))
        Fs_SafeFree(newMsgBlock);
}

void FS_IocpMsgDispatcher::CloseSession(UInt64 sessionId, UInt64 consumerId)
{
    if(_isClose)
        return;

    // sender是一对一的需要找到对应的收到网络包的那个transfer 的id
    auto &senderMq = _netEngine->_GetSenderMq();

    // 新的待发送的消息
    FS_NetMsgBufferBlock *newMsgBlock = new FS_NetMsgBufferBlock;
    newMsgBlock->_mbType = MessageBlockType::MB_NetCloseSession;
    newMsgBlock->_generatorId = 0;
    newMsgBlock->_sessionId = sessionId;

    // 送入消息队列
    if(!senderMq[consumerId]->Push(newMsgBlock))
        Fs_SafeFree(newMsgBlock);
}



void FS_IocpMsgDispatcher::_DoBusinessProcess(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData)
{
    // TODO:处理单一消息业务逻辑部分
    if(_logic)
        _logic->OnMsgDispatch(sessionId, generatorId, msgData);
}

void FS_IocpMsgDispatcher::_OnDelaySessionDisconnect(UInt64 sessionId)
{
    std::list<IDelegate<void, IUser *> *> *delagates = NULL;
    auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
    if(iterDisconnected != _sessionIdRefUserDisconnected.end())
        delagates = &iterDisconnected->second;

    // TODO:真实的session断开
    if(_logic)
        _logic->OnSessionDisconnected(sessionId, delagates);

    if(delagates)
    {
        if(!delagates->empty())
            STLUtil::DelListContainer(*delagates);

        _sessionIdRefUserDisconnected.erase(iterDisconnected);
    }
}

FS_NAMESPACE_END

#endif
