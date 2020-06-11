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
 * @file  : IFS_MsgDispatcher.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_MsgDispatcher.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Impl/IFS_Acceptor.h"
#include "FrightenStone/common/net/Defs/FS_ConnectInfo.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Defs/IFS_NetBuffer.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"

#include "FrightenStone/common/crashhandle/CrashHandle.h"

#pragma region platform relation
#ifdef _WIN32
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#else

#endif
#pragma endregion

#include "FrightenStone/common/memleak/memleak.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/TimeWheel/TimeWheel.h"

FS_NAMESPACE_BEGIN
IFS_MsgDispatcher::MessageQueueHandler IFS_MsgDispatcher::_msgBlockHandler[NetMessageType::End] = { NULL };

IFS_MsgDispatcher::~IFS_MsgDispatcher()
{
    STLUtil::DelMapContainer(_sessions);
    Fs_SafeFree(_pool);
    Fs_SafeFree(_timeWheel);
    Fs_SafeFree(_cfgs);
    Fs_SafeFree(_printAlloctorOccupiedInfo);
    Fs_SafeFree(_sessionBufferAlloctor);
    if (_recvMsgBlocks)
        STLUtil::DelListContainer(*_recvMsgBlocks);
    Fs_SafeFree(_recvMsgBlocks);

    for (auto &iterDelegateInfo : _sessionIdRefUserDisconnected)
        STLUtil::DelListContainer(iterDelegateInfo.second);
    _sessionIdRefUserDisconnected.clear();

    //     _CrtMemCheckpoint(&s2);
    //     if(_CrtMemDifference(&s3, &s1, &s2))
    //         _CrtMemDumpStatistics(&s3);
}

Int32 IFS_MsgDispatcher::BeforeStart(const NetEngineTotalCfgs &cfgs)
{
    Int32 st = IFS_EngineComp::BeforeStart(cfgs);
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("IFS_EngineComp::BeforeStart fail st[%d]"), st);
        return st;
    }

    // 初始化消息处理函数
    _InitMsgHandlers();

    _cfgs = new DispatcherCfgs;
    *_cfgs = cfgs._dispatcherCfgs;
    _timeWheel = new TimeWheel(_cfgs->_dispatcherResolutionInterval);
    _pool = new FS_ThreadPool(0, 1);

    // 缓存分配器
    _sessionBufferAlloctor = new MemoryAlloctorWithLimit(FS_BUFF_SIZE_DEF
        , _cfgs->_prepareBufferPoolCnt
        , _cfgs->_maxAlloctorBytesPerDispatcher);
    _sessionBufferAlloctor->InitMemory();
    _printAlloctorOccupiedInfo = DelegatePlusFactory::Create(this, &IFS_MsgDispatcher::_PrintAlloctorOccupiedInfo);

    // 并发消息队列
    _recvMsgBlocks = new std::vector<std::list<FS_MessageBlock *> *>;
    auto generatorQuatity = _concurrentMq->GetGeneratorQuality();
    _recvMsgBlocks->resize(generatorQuatity);
    for (UInt32 i = 0; i < generatorQuatity; ++i)
        (*_recvMsgBlocks)[i] = new std::list<FS_MessageBlock *>;

    // 业务逻辑初始化
    if (_logic)
    {
        _logic->SetDispatcher(this);
        _logic->SetTimeWheel(_timeWheel);
        auto st = _logic->BeforeStart();
        if (st != StatusDefs::Success)
        {
            g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("_logic->BeforeStart error st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}


Int32 IFS_MsgDispatcher::Start()
{
    Int32 st = IFS_EngineComp::Start();
    if (st != StatusDefs::Success)
    {
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("IFS_EngineComp::Start fail st[%d]"), st);
        return st;
    }

    if (_logic)
    {
        auto st = _logic->Start();
        if (st != StatusDefs::Success)
        {
            g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("_logic->Start error st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}

void IFS_MsgDispatcher::AfterStart()
{
    auto task = DelegatePlusFactory::Create(this, &IFS_MsgDispatcher::_OnBusinessProcessThread);
    if (!_pool->AddTask(task, true))
    {
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("add task fail"));
    }

    g_Log->sys<IFS_MsgDispatcher>(_LOGFMT_("dispatcher start suc..."));
}

void IFS_MsgDispatcher::WillClose()
{
    if (_isClose)
        return;

    _concurrentMq->NotifyConsumerByGenerator(_consumerId);

    // 线程退出
    _pool->Close();
}

void IFS_MsgDispatcher::BeforeClose()
{
    if (_isClose)
        return;

    // 需要清理sessions
    if (_logic)
    {
        _logic->WillClose();
        _logic->BeforeClose();
    }

    IFS_EngineComp::BeforeClose();
}

void IFS_MsgDispatcher::Close()
{
    if (_isClose)
        return;

    _isClose = true;

    if (_logic)
        _logic->Close();

    if (!_recvMsgBlocks->empty())
    {
        UInt32 generatorSize = static_cast<UInt32>(_recvMsgBlocks->size());
        UInt64 blockCount = 0;
        for (UInt32 i = 0; i < generatorSize; ++i)
        {
            auto msgQueue = _recvMsgBlocks->at(i);
            if (!msgQueue)
                continue;

            for (auto &recvMsgBlock : *msgQueue)
            {
                FS_NetMsgBlock *netMsg = recvMsgBlock->CastTo<FS_NetMsgBlock>();
                g_Log->net<IFS_MsgDispatcher>("netmsg _compId[%u] _generatorId[%u] _netMessageBlockType[%d]is unhandled"
                    , netMsg->_compId
                    , netMsg->_generatorId
                    , netMsg->_messageType);
                ++blockCount;
                Fs_SafeFree(recvMsgBlock);
            }
            msgQueue->clear();
        }

        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("msg block queue has unhandled msgs[%llu]"), blockCount);
    }

    STLUtil::DelListContainer(*_recvMsgBlocks);
    Fs_SafeFree(_recvMsgBlocks);
    for (auto &iterDelegateInfo : _sessionIdRefUserDisconnected)
        STLUtil::DelListContainer(iterDelegateInfo.second);
    _sessionIdRefUserDisconnected.clear();

    IFS_EngineComp::Close();
}

void IFS_MsgDispatcher::AfterClose()
{
    // 内存分配器占用情况打印结束
    MemleakMonitor::GetInstance()->UnRegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    if (_sessionBufferAlloctor)
        _sessionBufferAlloctor->FinishMemory();
}
// 
// void IFS_MsgDispatcher::SendData(UInt64 sessionId, NetMsg_DataHeader *msg)
// {
//     if (_isClose)
//         return;
// 
//     auto session = GetSession(sessionId);
//     if (!session || !session->CanPost())
//         return;
// 
//     if (!session->PushMsgToSend(msg))
//     {
//         g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg cmd[%hu] len[%hu] fail")
//             , sessionId
//             , msg->_cmd);
//     }
// 
//     if (session->IsPostSend())
//     {
//         auto iterSendQueue = _toPostSend.find(session);
//         g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("session is already post send sessionId[%llu] %s")
//             , sessionId, iterSendQueue == _toPostSend.end() ? "not in post send queue" : "in post send queue");
//         return;
//     }
// 
//     //if(_DoPostSend(session))
//     // send延后发送,进行优化
//     _toPostSend.insert(session);
// }

Int32 IFS_MsgDispatcher::AsynConnect(const FS_ConnectInfo &connectInfo)
{
    auto comps = _engine->GetCompByType(EngineCompType::Connector);
    if (!comps || comps->empty())
    {
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("lose connectors"));
        return StatusDefs::NetModule_ConnectorsNotExists;
    }

    auto pos = _compId % comps->size();
    auto connector = comps->at(pos)->CastTo<IFS_Connector>();
    return connector->AsynConnect(_consumerId, connectInfo);
}

void IFS_MsgDispatcher::_OnBusinessProcessThread(FS_ThreadPool *pool)
{
    // _timeWheel->GetModifiedResolution(_resolutionInterval);
    const auto &resolutionIntervalSlice = _cfgs->_dispatcherResolutionInterval;
    bool hasMsg = false;

    // 初始化配置
    _transferThreadId = SystemUtil::GetCurrentThreadId();
    MemleakMonitor::GetInstance()->RegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    // 等待其他组件ready
    MaskReady(true);
    g_Log->sys<IFS_MsgDispatcher>(_LOGFMT_("business process monitor is working..."));
    EngineCompsMethods::WaitForAllCompsReady(_engine);
    // 本地服务器准备就绪
    _OnLocalServerReady();
    // startup
    _OnWillStartup();

    while (pool->IsPoolWorking() || hasMsg)
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
        if (hasMsg)
            _OnBusinessProcessing();

        // 5. 投递消息
        _PostEvents();

        // 6.移除sessions
        _WillRemoveSessions();

        // 投递业务产生的异步处理事件 TODO: 不合适修正，因为可能会错过跨天等重要节点
        // _timeWheel->GetModifiedResolution(_resolutionInterval);
    }

    _concurrentMq->PopImmediately(_consumerId, _recvMsgBlocks, hasMsg);
    _RunAFrame(hasMsg);

    MaskReady(false);
#ifndef _WIN32
    _CancelAllSessionIosAndWaitForTransferIoFinish();
#endif

    // 清理会话
     _ClearAllSessions();

    // 系统关服
    _CloseLogicSvc();
    g_Log->sys<IFS_MsgDispatcher>(_LOGFMT_("dispatcher process thread end compId[%u],threadId[%llu]")
        , _compId, SystemUtil::GetCurrentThreadId());
}

void IFS_MsgDispatcher::_RunAFrame(bool hasMsg)
{
    // 2.先执行定时器事件
    _timeWheel->RotateWheel();

    // 3.检查心跳
    _CheckHeartbeat();

    // 从异步消息队列取得异步处理完成返回事件 TODO: 需要有异步处理队列，其他线程塞入
    // 清理完成的异步事件 TODO:

    // 4.再执行业务事件
    if (hasMsg)
        _OnBusinessProcessing();

    // 5. 投递消息
    _PostEvents();

    // 6.移除sessions
    _WillRemoveSessions();
}

void IFS_MsgDispatcher::_CheckHeartbeat()
{
    _curTime.FlushTime();
    for (auto iterSession = _sessionHeartbeatQueue.begin(); iterSession != _sessionHeartbeatQueue.end();)
    {
        auto session = *iterSession;
        if (session->GetHeartBeatExpiredTime() > _curTime)
            break;

        _engine->HandleCompEv_HeartBeatTimeOut();
        iterSession = _sessionHeartbeatQueue.erase(iterSession);
        session->MaskClose();
        _toRemove.insert(session);

#ifndef _WIN32
      g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("session heartbeat time out: %s"), session->ToString().c_str());
#endif
    }
}

void IFS_MsgDispatcher::_OnBusinessProcessing()
{
    // 将网络数据转移到缓冲区
    FS_NetMsgBlock *netMsgBlock = NULL;
    UInt64 sessionId = 0;
    for (auto iterGeneratorQueue = _recvMsgBlocks->begin(); iterGeneratorQueue != _recvMsgBlocks->end(); ++iterGeneratorQueue)
    {
        auto *blockQueue = *iterGeneratorQueue;
        if (blockQueue->empty())
            continue;

        for (auto iterBlock = blockQueue->begin(); iterBlock != blockQueue->end();)
        {
            netMsgBlock = (*iterBlock)->CastTo<FS_NetMsgBlock>();
            (this->*_msgBlockHandler[netMsgBlock->_messageType])(netMsgBlock);
            Fs_SafeFree(netMsgBlock);
            iterBlock = blockQueue->erase(iterBlock);
        }
    }
}

void IFS_MsgDispatcher::_PostEvents()
{
    // post recv
    FS_Session *session = NULL;
    for (auto iterRecv = _toPostRecv.begin(); iterRecv != _toPostRecv.end();)
    {
        session = *iterRecv;
        if (session->CanPost() &&
            !session->IsPostRecv())
            _DoPostRecv(session);

        iterRecv = _toPostRecv.erase(iterRecv);
    }

    // post send
    for (auto iterSend = _toPostSend.begin(); iterSend != _toPostSend.end();)
    {
        session = *iterSend;
        if (!session->IsPostSend() &&
            session->HasMsgToSend())
            _DoPostSend(session);

        iterSend = _toPostSend.erase(iterSend);
    }
}

void IFS_MsgDispatcher::_WillRemoveSessions()
{
    FS_Session *session = NULL;
    for (auto iterSession = _toRemove.begin(); iterSession != _toRemove.end();)
    {
        session = *iterSession;
        _RemoveFromPostRecvQueue(session);
        _RemoveFromPostSendQueue(session);

        // 准备移除,等待transfer返回正式移除session
        _RemoveSessionGracefully(session);
        iterSession = _toRemove.erase(iterSession);
    }
}

void IFS_MsgDispatcher::_CloseLogicSvc()
{
    if (_logic)
    {
        _logic->WillClose();
        _logic->BeforeClose();
        _logic->Close();
        _logic->AfterClose();
    }
}

void IFS_MsgDispatcher::_ClearAllSessions()
{
#ifdef _WIN32
    for (auto iterSession = _sessions.begin(); iterSession != _sessions.end();)
    {
        auto session = iterSession->second;
        session->EnableDisconnect();
        _OnSessionDisconnected(session);
        iterSession = _sessions.erase(iterSession);
    }

#else
    // linux 下不用_OnSessionDisconnected因为其他地方已经做了
    for (auto iterSession = _sessions.begin(); iterSession != _sessions.end();)
    {
        Fs_SafeFree(iterSession->second);
        iterSession = _sessions.erase(iterSession);
    }
#endif
}

void IFS_MsgDispatcher::_OnLocalServerReady()
{
    if (_logic)
        _logic->OnLocalServerReady();
}

void IFS_MsgDispatcher::_OnWillStartup()
{
    if (_logic)
        _logic->WillStartup();
}

#pragma region platform relation
#ifndef _WIN32
void IFS_MsgDispatcher::_CancelAllSessionIosAndWaitForTransferIoFinish()
{
    for (auto iterSession = _sessions.begin(); iterSession != _sessions.end(); ++iterSession)
    {
        auto session = iterSession->second;
        session->EnableDisconnect();
        session->CancelRecvIoAndMaskClose(CancelIoReason::CloseDispatcher, false);
        session->CancelSendIoAndMaskClose(CancelIoReason::CloseDispatcher, false);
        _OnSessionDisconnectedNoFree(session);
    }

    // 不等待可能会出现缓存被释放,造成野指针
    EngineCompsMethods::WaitForAllTransferIoFinish(_engine);
}

#endif
#pragma endregion

bool IFS_MsgDispatcher::_DoPostRecv(FS_Session *session)
{// 外部必须判断canpost
    // session不可post或者已经post完成不可重复post
    if (session->IsPostRecv())
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] is already posted recv."), session->GetSessionId());
        return true;
    }

    Int32 st = session->PostRecv();
    if (st != StatusDefs::Success)
    {
        g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("PostRecv fail st[%d] sessionId[%llu]"), session->GetSessionId());
        session->MaskClose();
        _toRemove.insert(session);
        return false;
    }

#ifndef _WIN32
   // g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("suc post a recv msg sessionId[%llu]"), session->GetSessionId());
#endif
    return true;
}

bool IFS_MsgDispatcher::_DoPostSend(FS_Session *session)
{// 外部必须判断canpost
    // 判断是否可以post,是否已经postsend,是否有消息发送 外部去判断,提高性能
//     if (session->IsPostSend()
//         || !session->HasMsgToSend())
//     {
//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] already posted or has no msg to send"), session->GetSessionId());
//         return true;
//     }

    Int32 st = session->PostSend();
    if (st != StatusDefs::Success)
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("PostSend fail st[%d] sessionId[%llu]"), session->GetSessionId());
        session->MaskClose();
        _toRemove.insert(session);
        return false;
    }

#ifndef _WIN32
    // g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("suc post a send msg sessionId[%llu]"), session->GetSessionId());
#endif
    return true;
}

void IFS_MsgDispatcher::_RemoveSessionGracefully(FS_Session *session)
{// 仅仅close掉socket，并等待transfer返回, ondisconnected才是真正的断开
    const auto sessionId = session->GetSessionId();
    g_Log->net<IFS_MsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s> prepare remove"
        , sessionId
        , session->GetSocket()
        , session->GetAddr()->ToString().c_str());

#ifndef _WIN32
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] sock[%llu] disconnected address<%s> prepare remove")
//         , sessionId
//         , session->GetSocket()
//         , session->GetAddr()->ToString().c_str());
#endif

    session->MaskClose();
    if (!session->CanDisconnect())
    {// 还有post event还没返回不可立即关闭应该先cancel,保证已经完成的io事件正常返回
#ifdef _WIN32
        session->CancelPostedEventsAndMaskClose();
#else
        // 还有post event还没返回不可立即关闭应该先cancel,保证已经完成的io事件正常返回
        session->CancelRecvIoAndMaskClose(CancelIoReason::ForceClosed, true);
        session->CancelSendIoAndMaskClose(CancelIoReason::ForceClosed, true);
#endif
    }
    else
    {
        // close会把在缓冲完全清除,若有post还未返回则会出现丢数据情况
        _OnSessionDisconnected(session);
        _sessions.erase(sessionId);
    }
}

#pragma region net msg handle

void IFS_MsgDispatcher::_OnMsgBlockConnectionFailure(FS_NetMsgBlock *msgBlock)
{
    auto connectedMsgBlock = msgBlock->CastTo<PostConnectionFailureToDispatcherMsgBlock>();
    if (_logic)
        _logic->OnConnectFailureRes(connectedMsgBlock->_connectInfo, connectedMsgBlock->_resultCode);
}

void IFS_MsgDispatcher::_OnSessionDisconnectedNoFree(FS_Session *session)
{
    // 从session返回正式移除session
    const auto sessionId = session->GetSessionId();
    g_Log->net<IFS_MsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s>"
        , sessionId
        , session->GetSocket()
        , session->GetAddr()->ToString().c_str());

#ifndef _WIN32
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] sock[%llu] disconnected address<%s>")
//         , sessionId
//         , session->GetSocket()
//         , session->GetAddr()->ToString().c_str());
#endif

    // 断连回调
    std::list<IDelegate<void, UInt64> *> *delagates = NULL;
    auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
    if (iterDisconnected != _sessionIdRefUserDisconnected.end())
        delagates = &iterDisconnected->second;

    // 业务层断开会话
    if (_logic)
        _logic->OnSessionDisconnected(sessionId, delagates);

    if (delagates)
    {
        if (!delagates->empty())
            STLUtil::DelListContainer(*delagates);

        _sessionIdRefUserDisconnected.erase(iterDisconnected);
    }

    // 引擎回调
    _engine->HandleCompEv_Disconnected(sessionId
        , session->GetSrcAcceptorCompId()
        , session->GetTransferCompId());

    // session断开
    session->Close();
    session->OnDisconnect();
    session->OnDestroy();

    // 移除资源
    _sessionHeartbeatQueue.erase(session);
    _toPostRecv.erase(session);
    _toPostSend.erase(session);
    --_curSessionQuatity;
}

void IFS_MsgDispatcher::_OnSessionConnected(SessionConnectedNetMsgBlock *connectedMsg)
{
#ifndef _WIN32
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_(" new session is coming. _OnSessionConnected connectedMsg info:\n[%s]")
//         , connectedMsg->ToString().c_str());
#endif

    const UInt64 sessionId = connectedMsg->_sessionId;
    ++_curSessionQuatity;

    // 创建session对象并初始化
    BuildSessionInfo buldData;
    buldData._sessionId = sessionId;
    buldData._transferCompId = connectedMsg->_compId;
    buldData._acceptorCompId = connectedMsg->_acceptorCompId;
    buldData._connectorCompId = connectedMsg->_connectorCompId;
    buldData._protocolPort = connectedMsg->_protocolPort;
    buldData._srcCompType = connectedMsg->_srcCompType;
    buldData._sock = connectedMsg->_sock;
    buldData._addrInfo = &(connectedMsg->_addrInfo);
    buldData._memAlloctor = _sessionBufferAlloctor;
    buldData._heartbeatIntervalMicroSeconds = _cfgs->_heartbeatDeadTimeMsInterval * Time::_microSecondPerMilliSecond;
    auto newSession = FS_SessionFactory::Create(buldData);
    _sessions.insert(std::make_pair(sessionId, newSession));
    _UpdateSessionHeartbeat(newSession);

    // 设置参数
#ifdef _WIN32
    newSession->Bind(reinterpret_cast<FS_Iocp *>(connectedMsg->_iocp));
#else
    newSession->Bind(reinterpret_cast<FS_EpollTransferPoller *>(connectedMsg->_poller));
#endif

    newSession->OnConnect();
    auto addr = newSession->GetAddr();
    g_Log->net<IFS_MsgDispatcher>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
        " protocol port[%hu], src comp type[%d:%s]"
        , sessionId
        , newSession->GetSocket()
        , addr->GetAddr().c_str()
        , addr->GetPort()
        , newSession->GetProtocolPort()
        , newSession->GetSrcCompType()
        , EngineCompType::GetStr(newSession->GetSrcCompType()).c_str());

#ifndef _WIN32
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
//         " protocol port[%hu], src comp type[%d:%s]")
//         , sessionId
//         , newSession->GetSocket()
//         , addr->GetAddr().c_str()
//         , addr->GetPort()
//         , newSession->GetProtocolPort()
//         , newSession->GetSrcCompType()
//         , EngineCompType::GetStr(newSession->GetSrcCompType()).c_str());
#endif

    // 设置断开回调
    auto userDisconnectedDelegates = connectedMsg->_onUserDisconnectedRes;
    if (userDisconnectedDelegates)
    {
        auto iterDisconnected = _sessionIdRefUserDisconnected.find(sessionId);
        if (iterDisconnected == _sessionIdRefUserDisconnected.end())
            iterDisconnected = _sessionIdRefUserDisconnected.insert(std::make_pair(sessionId, std::list<IDelegate<void, UInt64>*>())).first;
        iterDisconnected->second.push_back(userDisconnectedDelegates);
        connectedMsg->_onUserDisconnectedRes = NULL;
    }

    const Int32 st = _logic->OnSessionConnected(sessionId, connectedMsg->_stub);
    if (st != StatusDefs::Success)
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("_logic->OnSessionConnected fail st[%d] sessionId[%llu]"), st, sessionId);

        // 失败则移除session
        _DelayCloseSession(newSession);
        return;
    }

    // post recv有可能会断开连接
    if (newSession->IsValid())
    {
        if (!_DoPostRecv(newSession))
            g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("_DoPostRecv fail sessionId[%llu]"), sessionId);
    }
}

void IFS_MsgDispatcher::_OnSessionMsgHandle(FS_Session *session)
{
    session->InvokeMsgHandler();
    if(session->IsValid())
    {
        // 消息处理完后若有消息需要发送则及时发送
        if (!session->IsPostSend() &&
            session->HasMsgToSend())
            _DoPostSend(session);
    }

//     auto recvBuffer = session->CastToRecvBuffer();
//     const Byte8 *buffer = NULL;
//     const UInt64 sessionId = session->GetSessionId();
// 
//     // TODO:可以在logic层做,因为协议跟应用层息息相关，底层不建议做处理
//     _logic->OnSessionMsgHandle(session);
//     while (recvBuffer->HasMsg())
//     {
//         if (!_msgDecoder->Decode(recvBuffer->GetData()))
//         {
//             g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("Decode error sessionId[%llu]")
//                 , sessionId);
//             _DelayCloseSession(session);
//             break;
//         }
// 
//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("cmd[%d] msg iscoming len[%u]")
//             , _msgDecoder->GetCmd(), _msgDecoder->GetMsgLen());
// 
//         // TODO:处理单一消息业务逻辑部分
//         if (_logic)
//             _logic->OnMsgDispatch(sessionId, _msgDecoder);
// 
//         _engine->HandleCompEv_RecvMsgAmount();
//         recvBuffer->PopFront(_msgDecoder->GetMsgLen());
// 
//         if (session->IsDelayClose())
//             break;
//     }
}
#ifdef _WIN32

void IFS_MsgDispatcher::_OnMsgArrived(IocpMsgArrivedMsgBlock *arrivedMsg)
{
    IoEvent *ioEv = arrivedMsg->_ioEv;
    IoDataBase *ioData = reinterpret_cast<IoDataBase *>(ioEv->_ioData);
    Int32 errorCode = arrivedMsg->_errorCode;
    const Int64 transferBytes = ioEv->_bytesTrans;
    if (errorCode != StatusDefs::Success)
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("arrived msg errorCode[%d]"), errorCode);
        return;
}

    if (ioData->_sessionId == static_cast<UInt64>(IocpDefs::IO_QUIT))
    {// transfer程序退出
        return;
    }

    const UInt64 sessionId = ioEv->_sessionId;
    auto session = GetSession(sessionId);
    if (!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
//         g_Log->net<FS_IocpMsgTransfer>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
//                                      , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }

    // 2.处理接收与发送 TODO:可以做成回调数组，提高性能
    const Int32 ioType = ioData->_ioType;
    if (IocpDefs::IO_RECV == ioType)
    {
        if (transferBytes <= 0)
        {// 客户端断开处理
            g_Log->net<IFS_MsgDispatcher>("sessionId[%llu] sock[%llu] IO_TYPE::RECV bytesTrans[%llu] will disconnect IsDelayClose[%d]"
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
        _engine->HandleCompEv_RecvMsg(transferBytes);
        if (session->IsValid())
        {
            _OnSessionMsgHandle(session);
            _UpdateSessionHeartbeat(session);
        }

        // 重新接收数据
        if (session->CanPost())
            _toPostRecv.insert(session);
    }
    else if (IocpDefs::IO_SEND == ioType)
    {
        if (transferBytes <= 0)
        {// 客户端断开处理
            g_Log->net<IFS_MsgDispatcher>("sessionId[%llu] sock[%llu] IO_TYPE::IO_SEND bytesTrans[%lld] disconnected IsDelayClose[%d]"
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
        _engine->HandleCompEv_SendMsg(transferBytes);

        if (session->IsValid())
        {
            // TODO:有被疯狂发包的风险，解决方案：心跳包协议+频繁发包计数+发包成功时更新时间戳
            _UpdateSessionHeartbeat(session);
        }

        // 重新投递发送
        if (session->CanPost() &&
            !session->IsPostSend() &&
            session->HasMsgToSend())
            _toPostSend.insert(session);
    }
    else
    {
        session->EnableDisconnect();
        session->Close();
        _toRemove.insert(session);
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("undefine io type[%d]."), ioType);
    }
}



#else
void IFS_MsgDispatcher::_OnMsgRecv(EpollRecvDataMsgBlock *recvMsg)
{
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("a recv io msg from transfer generatorId[%u] compId[%u] is coming handle sessionId[%llu] transferBytes[%lld]")
//         , recvMsg->_generatorId, recvMsg->_compId, recvMsg->_sessionId, recvMsg->_transferBytes);

    EpollIoData *ioData = reinterpret_cast<EpollIoData *>(recvMsg->_ioData);
    const UInt64 sessionId = recvMsg->_sessionId;
    const Int64 transferBytes = recvMsg->_transferBytes;
    const bool isRegisterInMonitor = recvMsg->_isSessionInRecverMonitor;

    auto session = GetSession(sessionId);
    if (!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
        g_Log->net<IFS_MsgDispatcher>("_OnMsgRecv sessionId[%llu] is removed before.\n stack trace back:\n%s"
            , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());

//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("_OnMsgRecv sessionId[%llu] is removed before.\n stack trace back:\n%s")
//             , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }

    session->ResetPostRecvMask();
    if (session->IsDelayClose() || transferBytes <= 0)
    {// 客户端断开处理
        g_Log->net<IFS_MsgDispatcher>("_OnMsgRecv sessionId[%llu] sock[%llu] bytesTrans[%llu] will disconnect IsDelayClose[%d] sessionInfo:\n%s"
            , sessionId
            , session->GetSocket(),
            transferBytes,
            session->IsDelayClose()
            , session->ToString().c_str());
//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("_OnMsgRecv sessionId[%llu] sock[%llu] bytesTrans[%llu] will disconnect IsDelayClose[%d] sessionInfo:\n%s")
//             , sessionId
//             , session->GetSocket(),
//             transferBytes,
//             session->IsDelayClose()
//             , session->ToString().c_str());

        if (!isRegisterInMonitor) // 已经不在recv注册队列中可以不用cancel
            session->SetRecvIoCanceled();
        _toRemove.insert(session);
        return;
    }

    session->OnRecvSuc(transferBytes, ioData);

    // 收到消息回调
    _engine->HandleCompEv_RecvMsg(transferBytes);

    if (session->IsValid())
    {
        _OnSessionMsgHandle(session);
        _UpdateSessionHeartbeat(session);
    }

    // 重新接收数据
    if (session->CanPost())
        _toPostRecv.insert(session);
}

void IFS_MsgDispatcher::_OnMsgSendSuc(EpollSendSucMsgBlock *sendSucMsg)
{
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("a send suc io msg from transfer generatorId[%u] compId[%u] is coming handle sessionId[%llu] transferBytes[%lld]")
//         , sendSucMsg->_generatorId, sendSucMsg->_compId, sendSucMsg->_sessionId, sendSucMsg->_transferBytes);

    EpollIoData *ioData = reinterpret_cast<EpollIoData *>(sendSucMsg->_ioData);
    const UInt64 sessionId = sendSucMsg->_sessionId;
    const Int64 transferBytes = sendSucMsg->_transferBytes;
    const bool isRegisterInMonitor = sendSucMsg->_isSessionInSenderMonitor;

    auto session = GetSession(sessionId);
    if (!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
        g_Log->net<IFS_MsgDispatcher>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
            , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());

//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] is removed before.\n stack trace back:\n%s")
//             , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }

    session->ResetPostSendMask();
    if (session->IsDelayClose() || transferBytes <= 0)
    {// 客户端断开处理
        g_Log->net<IFS_MsgDispatcher>("_OnMsgSendSuc sessionId[%llu] sock[%llu] bytesTrans[%lld] disconnected IsDelayClose[%d], sessionInfo:\n%s"
            , sessionId
            , session->GetSocket()
            , transferBytes
            , session->IsDelayClose()
            , session->ToString().c_str());

//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("_OnMsgSendSuc sessionId[%llu] sock[%llu] bytesTrans[%lld] disconnected IsDelayClose[%d], sessionInfo:\n%s")
//             , sessionId
//             , session->GetSocket()
//             , transferBytes
//             , session->IsDelayClose()
//             , session->ToString().c_str());

        if (!isRegisterInMonitor)
            session->SetSendIoCanceled();

        _toRemove.insert(session);
        return;
    }

    session->OnSendSuc(transferBytes, ioData);

    // 消息发送回调
    _engine->HandleCompEv_SendMsg(transferBytes);

    if (session->IsValid())
    {
        // TODO:有被疯狂发包的风险，解决方案：心跳包协议+频繁发包计数+发包成功时更新时间戳
        _UpdateSessionHeartbeat(session);
    }

    // 重新投递发送
    if (session->CanPost() &&
        !session->IsPostSend() &&
        session->HasMsgToSend())
        _toPostSend.insert(session);
}
#endif
#pragma endregion

void IFS_MsgDispatcher::_PrintAlloctorOccupiedInfo()
{
    FS_String memInfo;
    memInfo.AppendFormat("\n【dispatcher alloctor occupied info】\n");
    memInfo.AppendFormat("dispatcher compId[%u] threadId[%llu] alloctor occupied info:[", _compId, _transferThreadId);
    _sessionBufferAlloctor->MemInfoToString(memInfo);
    memInfo.AppendFormat(" ]");
    memInfo.AppendFormat("\n【+++++++++++++++++++++++++ End ++++++++++++++++++++++++++】\n");
    g_Log->mempool("%s", memInfo.c_str());
}

void IFS_MsgDispatcher::_InitMsgHandlers()
{
    _msgBlockHandler[NetMessageType::SessionConnected] = &IFS_MsgDispatcher::_OnMsgBlockNetSessionConnected;
    _msgBlockHandler[NetMessageType::PostConnectionFailureToDispatcher] = &IFS_MsgDispatcher::_OnMsgBlockConnectionFailure;

#ifdef _WIN32
    _msgBlockHandler[NetMessageType::IocpMsgArrived] = &IFS_MsgDispatcher::_OnMsgBlockNetMsgArrived;
#else
    _msgBlockHandler[NetMessageType::RecvDataEv] = &IFS_MsgDispatcher::_OnMsgBlockRecvData;
    _msgBlockHandler[NetMessageType::SendSucEv] = &IFS_MsgDispatcher::_OnMsgBlockSendSuc;
#endif
}

FS_NAMESPACE_END

