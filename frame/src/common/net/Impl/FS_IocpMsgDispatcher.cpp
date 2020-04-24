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
#include "FrightenStone/common/net/Defs/EngineCompDefs.h"

#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/TimeWheel/TimeWheel.h"
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/component/Impl/TimeWheel/FS_Timer.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/memleak/memleak.h"

#ifdef _WIN32

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_IocpMsgDispatcher, 32);

FS_IocpMsgDispatcher::MessageQueueHandler FS_IocpMsgDispatcher::_msgBlockHandler[NetMessageBlockType::End] = 
{NULL,
&FS_IocpMsgDispatcher::_OnMsgBlockNetMsgArrived,
NULL,
NULL,
&FS_IocpMsgDispatcher::_OnMsgBlockNetSessionConnected,
NULL,
&FS_IocpMsgDispatcher::_OnMsgBlockConnectorConnectOpFinish,
};

FS_IocpMsgDispatcher::FS_IocpMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId)
    :IFS_MsgDispatcher(netEngine, compId)
    , _pool(NULL)
    , _isClose{false}
    ,_timeWheel(NULL)
    ,_logic(NULL)
    ,_recvMsgBlocks(NULL)
    ,_cfgs(NULL)
    ,_transferThreadId(0)
    ,_sessionBufferAlloctor(NULL)
    ,_printAlloctorOccupiedInfo(NULL)
    ,_curSessionQuatity{0}
    ,_msgDecoder(NULL)
{
}

FS_IocpMsgDispatcher::~FS_IocpMsgDispatcher()
{
    STLUtil::DelMapContainer(_sessions);
    Fs_SafeFree(_pool);
    Fs_SafeFree(_timeWheel);
    Fs_SafeFree(_cfgs);
    Fs_SafeFree(_printAlloctorOccupiedInfo);
    Fs_SafeFree(_sessionBufferAlloctor);
    if(_recvMsgBlocks)
        STLUtil::DelListContainer(*_recvMsgBlocks);
    Fs_SafeFree(_recvMsgBlocks);

    for(auto &iterDelegateInfo : _sessionIdRefUserDisconnected)
        STLUtil::DelListContainer(iterDelegateInfo.second);
    _sessionIdRefUserDisconnected.clear();
    Fs_SafeFree(_msgDecoder);

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
    _msgDecoder = new NetMsgDecoder;

    // 缓存分配器
    _sessionBufferAlloctor = new MemoryAlloctorWithLimit(FS_BUFF_SIZE_DEF, _cfgs->_prepareBufferPoolCnt, _cfgs->_maxAlloctorBytesPerDispatcher);
    _sessionBufferAlloctor->InitMemory();
    _printAlloctorOccupiedInfo = DelegatePlusFactory::Create(this, &FS_IocpMsgDispatcher::_PrintAlloctorOccupiedInfo);

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

    return StatusDefs::Success;
}

void FS_IocpMsgDispatcher::AfterStart()
{
    auto task = DelegatePlusFactory::Create(this, &FS_IocpMsgDispatcher::_OnBusinessProcessThread);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("add task fail"));
    }

    g_Log->sys<FS_IocpMsgDispatcher>(_LOGFMT_("dispatcher start suc..."));
}

void FS_IocpMsgDispatcher::WillClose()
{
    if(_isClose)
        return;

    _concurrentMq->NotifyConsumerByGenerator(_consumerId);

    // 线程退出
    _pool->Close();
}

void FS_IocpMsgDispatcher::BeforeClose()
{
    if(_isClose)
        return;

    // 需要清理sessions
    if(_logic)
    {
        _logic->WillClose();
        _logic->BeforeClose();
    }

    IFS_EngineComp::BeforeClose();
}

void FS_IocpMsgDispatcher::Close()
{
    if(_isClose)
        return;

    _isClose = true;

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

void FS_IocpMsgDispatcher::AfterClose()
{
    // 内存分配器占用情况打印结束
    g_MemleakMonitor->UnRegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    if(_sessionBufferAlloctor)
        _sessionBufferAlloctor->FinishMemory();
}

void FS_IocpMsgDispatcher::SendData(UInt64 sessionId, NetMsg_DataHeader *msg)
{
    if(_isClose)
        return;

    auto session = _GetSession(sessionId);
    if(!session || !session->CanPost())
        return;

    if(!session->PushMsgToSend(msg))
    {
        g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg cmd[%hu] len[%hu] fail")
                                     , sessionId
                                     , msg->_cmd);
    }

    //if(_DoPostSend(session))
    // send延后发送,进行优化
    _toPostSend.insert(session);
}

void FS_IocpMsgDispatcher::SendData(UInt64 sessionId, const Byte8 *msgBuffer, UInt64 msgLen)
{
    if(_isClose)
        return;

    auto session = _GetSession(sessionId);
    if(!session || !session->CanPost())
        return;

    if(!session->PushMsgToSend(msgBuffer, msgLen))
    {
        g_Log->w<FS_IocpMsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg len[%llu] fail")
                                       , sessionId
                                       , msgLen);
    }

    // send延后发送,进行优化
    _toPostSend.insert(session);

    // if(_DoPostSend(session))
}

void FS_IocpMsgDispatcher::CloseSession(UInt64 sessionId)
{
    auto session = _GetSession(sessionId);
    session->CancelPostedEventsAndMaskClose();
}

void FS_IocpMsgDispatcher::_OnBusinessProcessThread(FS_ThreadPool *pool)
{// 业务层可以不用很频繁唤醒，只等待网络层推送消息过来

    // _timeWheel->GetModifiedResolution(_resolutionInterval);
    const auto &resolutionIntervalSlice = _cfgs->_dispatcherResolutionInterval;
    bool hasMsg = false;

    // 初始化配置
    _transferThreadId = SystemUtil::GetCurrentThreadId();
    g_MemleakMonitor->RegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    // 等待其他组件ready
    MaskReady(true);
    g_Log->sys<FS_IocpMsgDispatcher>(_LOGFMT_("business process monitor is working..."));
    EngineCompsMethods::WaitForAllCompsReady(_engine);

    while(pool->IsPoolWorking() || hasMsg)
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
        _WillRemoveSessions();

        // 投递业务产生的异步处理事件 TODO: 不合适修正，因为可能会错过跨天等重要节点
        // _timeWheel->GetModifiedResolution(_resolutionInterval);
    }
    
    MaskReady(false);
    _concurrentMq->PopImmediately(_consumerId, _recvMsgBlocks, hasMsg);
    _RunAFrame(hasMsg);

    _ClearAllSessions();

    // 系统关服
    _CloseLogicSvc();
    g_Log->sys<FS_IocpMsgDispatcher>(_LOGFMT_("dispatcher process thread end compId[%u],threadId[%llu]")
                                     , _compId, SystemUtil::GetCurrentThreadId());
}

void FS_IocpMsgDispatcher::_RunAFrame(bool hasMsg)
{
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
    _WillRemoveSessions();
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
    Int32 messageBlockType = NetMessageBlockType::Net_None;
    for(auto iterGeneratorQueue = _recvMsgBlocks->begin(); iterGeneratorQueue != _recvMsgBlocks->end(); ++iterGeneratorQueue)
    {
        auto *blockQueue = *iterGeneratorQueue;
        if(blockQueue->empty())
            continue;

        for(auto iterBlock = blockQueue->begin(); iterBlock != blockQueue->end();)
        {
            netMsgBlock = (*iterBlock)->CastTo<FS_NetMsgBufferBlock>();
            messageBlockType = netMsgBlock->_netMessageBlockType;
            (this->*_msgBlockHandler[messageBlockType])(netMsgBlock);
            Fs_SafeFree(netMsgBlock);
            iterBlock = blockQueue->erase(iterBlock);
        }
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

void FS_IocpMsgDispatcher::_WillRemoveSessions()
{
    FS_IocpSession *session = NULL;
    for(auto iterSession = _toRemove.begin(); iterSession!=_toRemove.end();)
    {
        session = *iterSession;
        _RemoveFromPostRecvQueue(session);
        _RemoveFromPostSendQueue(session);

        // 准备移除,等待transfer返回正式移除session
        _RemoveSessionGracefully(session);
        iterSession = _toRemove.erase(iterSession);
    }
}

void FS_IocpMsgDispatcher::_ClearAllSessions()
{
    for(auto iterSession = _sessions.begin(); iterSession != _sessions.end();)
    {
        auto session = iterSession->second;
        session->EnableDisconnect();
        _OnSessionDisconnected(session);
        iterSession = _sessions.erase(iterSession);
    }
}

void FS_IocpMsgDispatcher::_CloseLogicSvc()
{
    if(_logic)
    {
        _logic->WillClose();
        _logic->BeforeClose();
        _logic->Close();
        _logic->AfterClose();
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

void FS_IocpMsgDispatcher::_RemoveSessionGracefully(FS_IocpSession *session)
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
        _OnSessionDisconnected(session);
        _sessions.erase(sessionId);
    }
}

void FS_IocpMsgDispatcher::_PrintAlloctorOccupiedInfo()
{
    FS_String memInfo;
    memInfo.AppendFormat("\n【iocp dispatcher alloctor occupied info】\n");
    memInfo.AppendFormat("dispatcher compId[%u] threadId[%llu] alloctor occupied info:[", _compId, _transferThreadId);
    _sessionBufferAlloctor->MemInfoToString(memInfo);
    memInfo.AppendFormat(" ]");
    memInfo.AppendFormat("\n【+++++++++++++++++++++++++ End ++++++++++++++++++++++++++】\n");
    g_Log->mempool("%s", memInfo.c_str());
}

void FS_IocpMsgDispatcher::_OnMsgBlockNetMsgArrived(FS_NetMsgBufferBlock *msgBlock)
{
    auto arrivedMsg = msgBlock->CastTo<FS_NetArrivedMsg>();
    _OnMsgArrived(arrivedMsg);
}

void FS_IocpMsgDispatcher::_OnMsgBlockNetSessionConnected(FS_NetMsgBufferBlock *msgBlock)
{
    auto connectedMsgBlock = msgBlock->CastTo<FS_NetSessionWillConnectMsg>();
    _OnSessionConnected(connectedMsgBlock);
}

void FS_IocpMsgDispatcher::_OnMsgBlockConnectorConnectOpFinish(FS_NetMsgBufferBlock *msgBlock)
{
    if(_logic)
        _logic->OnConnectorConnectOpFinish();
}

void FS_IocpMsgDispatcher::_OnSessionDisconnected(FS_IocpSession *session)
{
    // 从session返回正式移除session
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s>"
                                   , sessionId
                                   , session->GetSocket(), session->GetAddr()->ToString().c_str());

    // 断连回调
    std::list<IDelegate<void, UInt64> *> *delagates = NULL;
    auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
    if(iterDisconnected != _sessionIdRefUserDisconnected.end())
        delagates = &iterDisconnected->second;

    // 业务层断开会话
    if(_logic)
        _logic->OnSessionDisconnected(sessionId, delagates);

    if(delagates)
    {
        if(!delagates->empty())
            STLUtil::DelListContainer(*delagates);

        _sessionIdRefUserDisconnected.erase(iterDisconnected);
    }

    _engine->_HandleCompEv_Disconnected(sessionId
                                        , session->GetAcceptorCompId()
                                        , session->GetTransferCompId());
    session->Close();
    session->OnDisconnect();
    session->OnDestroy();
    _sessionHeartbeatQueue.erase(session);
    _toPostRecv.erase(session);
    _toPostSend.erase(session);
    Fs_SafeFree(session);
    --_curSessionQuatity;
}

void FS_IocpMsgDispatcher::_OnSessionConnected(FS_NetSessionWillConnectMsg *connectedMsg)
{
    const UInt64 sessionId = connectedMsg->_sessionId;
    ++_curSessionQuatity;

    // 创建session对象并初始化
    auto newSession = FS_SessionFactory::Create(sessionId
                                                ,connectedMsg->_compId
                                                ,connectedMsg->_acceptorCompId
                                                , connectedMsg->_sock
                                                , &connectedMsg->_addrInfo
                                                , _sessionBufferAlloctor
                                                , _cfgs->_heartbeatDeadTimeMsInterval * Time::_microSecondPerMilliSecond)->CastTo<FS_IocpSession>();
    
    _sessions.insert(std::make_pair(sessionId, newSession));
    newSession->Bind(reinterpret_cast<FS_Iocp *>(connectedMsg->_netModule));
    newSession->OnConnect();
    auto addr = newSession->GetAddr();
    g_Log->net<FS_IocpMsgDispatcher>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
                                   , sessionId
                                   , newSession->GetSocket()
                                   , addr->GetAddr().c_str()
                                   , addr->GetPort());

    const Int32 st = _logic->OnSessionConnected(sessionId);
    if(st != StatusDefs::Success)
    {
        // 失败则移除session
        _MaskCloseSession(newSession);
        return;
    }

    auto userDisconnectedDelegates = connectedMsg->_onUserDisconnectedRes;
    if(userDisconnectedDelegates)
    {
        auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
        if(iterDisconnected == _sessionIdRefUserDisconnected.end())
            iterDisconnected = _sessionIdRefUserDisconnected.insert(std::make_pair(sessionId, std::list<IDelegate<void, UInt64>*>())).first;
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
    const Int32 ioType = ioData->_ioType;
    if(IocpDefs::IO_RECV == ioType)
    {
        if(transferBytes <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%llu] will disconnect IsDelayClose[%d]"
                                             , sessionId
                                             , session->GetSocket()
                                             , transferBytes
                                             , session->IsDelayClose());

            session->ResetPostRecvMask();
            session->CancelPostedEventsAndMaskClose();
            _toRemove.insert(session);
            return;
        }

        session->OnRecvSuc(transferBytes, ioData);

        // 收到消息回调
        _engine->_HandleCompEv_RecvMsg(transferBytes);
        if(session->IsValid())
        {
            _OnSessionMsgHandle(session);
            _UpdateSessionHeartbeat(session);
        }

        // 重新接收数据
        if(session->CanPost())
            _toPostRecv.insert(session);
    }
    else if(IocpDefs::IO_SEND == ioType)
    {
        if(transferBytes <= 0)
        {// 客户端断开处理
            g_Log->net<FS_IocpMsgDispatcher>("sessionId[%llu] sock[%llu] IO_TYPE::IO_SEND bytesTrans[%lld] disconnected IsDelayClose[%d]"
                                             , sessionId
                                             , session->GetSocket()
                                             , transferBytes
                                             , session->IsDelayClose());
            session->ResetPostSendMask();
            session->CancelPostedEventsAndMaskClose();
            _toRemove.insert(session);
            return;
        }

        session->OnSendSuc(transferBytes, ioData);

        // 消息发送回调
        _engine->_HandleCompEv_SendMsg(transferBytes);

        if(session->IsValid())
        {
            // TODO:有被疯狂发包的风险，解决方案：心跳包协议+频繁发包计数+发包成功时更新时间戳
            _UpdateSessionHeartbeat(session);
        }

        // 重新投递发送
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
        g_Log->e<FS_IocpMsgDispatcher>(_LOGFMT_("undefine io type[%d]."), ioType);
    }
}

void FS_IocpMsgDispatcher::_OnSessionMsgHandle(FS_IocpSession *session)
{
    auto recvBuffer = session->CastToRecvBuffer();
    const Byte8 *buffer = NULL;
    const UInt64 sessionId = session->GetSessionId();

    while(recvBuffer->HasMsg())
    {
        _msgDecoder->Decode(recvBuffer->GetData());
        // TODO:处理单一消息业务逻辑部分
        if(_logic)
            _logic->OnMsgDispatch(sessionId, _msgDecoder);

        _engine->_HandleCompEv_RecvMsgAmount();
        recvBuffer->PopFront(_msgDecoder->GetMsgLen());
    }
}

void FS_IocpMsgDispatcher::_MaskCloseSession(FS_IocpSession *session)
{
    session->EnableDisconnect();
    session->MaskClose();
    _toRemove.insert(session);
}

FS_NAMESPACE_END

#endif
