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
 * @file  : FS_EpollMsgDispatcher.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/30
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollMsgDispatcher.h"
#include "FrightenStone/common/net/Impl/FS_EpollSession.h"

#include "FrightenStone/common/net/Defs/FS_EpollBuffer.h"
#include "FrightenStone/common/net/Impl/IFS_MsgTransfer.h"
#include "FrightenStone/common/net/Impl/IFS_BusinessLogic.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/net/Defs/FS_NetMessageBlock.h"
#include "FrightenStone/common/net/Impl/IFS_NetEngine.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Impl/FS_SessionFactory.h"
#include "FrightenStone/common/net/Defs/EngineCompDefs.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"

#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/TimeWheel/TimeWheel.h"
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/component/Impl/TimeWheel/FS_Timer.h"
#include "FrightenStone/common/assist/utils/utils.h"
#include "FrightenStone/common/memleak/memleak.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_EpollMsgDispatcher, 32);

FS_EpollMsgDispatcher::MessageQueueHandler FS_EpollMsgDispatcher::_msgBlockHandler[FS_EpollEvMessageType::End] =
{
NULL
, NULL  // 1. 不可用
, NULL  // 2. 不可用
, NULL  // 3. 不可用
, NULL  // 4. 不可用
, NULL  // 5. 不可用
, &FS_EpollMsgDispatcher::_OnMsgBlockRecvData                   // 6. 收到数据
, &FS_EpollMsgDispatcher::_OnMsgBlockSendSuc                    // 7. 发送成功
, &FS_EpollMsgDispatcher::_OnMsgBlockNetSessionConnected        // 8. 会话连接成功
, &FS_EpollMsgDispatcher::_OnMsgBlockConnectorConnectOpFinish   // 9. 连接器连接成功
};


FS_EpollMsgDispatcher::FS_EpollMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId)
    :IFS_MsgDispatcher(netEngine, compId)
    , _pool(NULL)
    , _isClose{false}
    , _timeWheel(NULL)
    , _logic(NULL)
    , _recvMsgBlocks(NULL)
    , _cfgs(NULL)
    , _transferThreadId(0)
    , _sessionBufferAlloctor(NULL)
    , _printAlloctorOccupiedInfo(NULL)
    , _curSessionQuatity{0}
    , _msgDecoder(NULL)
{
}

FS_EpollMsgDispatcher::~FS_EpollMsgDispatcher()
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
}

Int32 FS_EpollMsgDispatcher::BeforeStart(const NetEngineTotalCfgs &cfgs)
{
    Int32 st = IFS_EngineComp::BeforeStart(cfgs);
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgDispatcher>(_LOGFMT_("IFS_EngineComp::BeforeStart fail st[%d]"), st);
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
    _printAlloctorOccupiedInfo = DelegatePlusFactory::Create(this, &FS_EpollMsgDispatcher::_PrintAlloctorOccupiedInfo);

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
            g_Log->e<FS_EpollMsgDispatcher>(_LOGFMT_("_logic->BeforeStart error st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}

Int32 FS_EpollMsgDispatcher::Start()
{
    Int32 st = IFS_EngineComp::Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollMsgDispatcher>(_LOGFMT_("IFS_EngineComp::Start fail st[%d]"), st);
        return st;
    }

    if(_logic)
    {
        auto st = _logic->Start();
        if(st != StatusDefs::Success)
        {
            g_Log->e<FS_EpollMsgDispatcher>(_LOGFMT_("_logic->Start error st[%d]"), st);
            return st;
        }
    }

    return StatusDefs::Success;
}


void FS_EpollMsgDispatcher::AfterStart()
{
    auto task = DelegatePlusFactory::Create(this, &FS_EpollMsgDispatcher::_OnBusinessProcessThread);
    if(!_pool->AddTask(task, true))
    {
        g_Log->e<FS_EpollMsgDispatcher>(_LOGFMT_("add task fail"));
    }

    g_Log->sys<FS_EpollMsgDispatcher>(_LOGFMT_("dispatcher start suc..."));
}

void FS_EpollMsgDispatcher::WillClose()
{
    if(_isClose)
        return;

    _concurrentMq->NotifyConsumerByGenerator(_consumerId);

    // 线程退出
    _pool->Close();
}

void FS_EpollMsgDispatcher::BeforeClose()
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

void FS_EpollMsgDispatcher::Close()
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
                FS_EpollEvMessageBlock *netMsg = recvMsgBlock->CastTo<FS_EpollEvMessageBlock>();
                g_Log->net<FS_EpollMsgDispatcher>("netmsg _compId[%u] _generatorId[%u] _messageType[%d]is unhandled"
                                                  , netMsg->_compId
                                                  , netMsg->_generatorId
                                                  , netMsg->_messageType);
                ++blockCount;
                Fs_SafeFree(recvMsgBlock);
            }
            msgQueue->clear();
        }

        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("msg block queue has unhandled msgs[%llu]"), blockCount);
    }

    STLUtil::DelListContainer(*_recvMsgBlocks);
    Fs_SafeFree(_recvMsgBlocks);
    for(auto &iterDelegateInfo : _sessionIdRefUserDisconnected)
        STLUtil::DelListContainer(iterDelegateInfo.second);
    _sessionIdRefUserDisconnected.clear();

    IFS_EngineComp::Close();
}

void FS_EpollMsgDispatcher::AfterClose()
{
    // 内存分配器占用情况打印结束
    g_MemleakMonitor->UnRegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    if(_sessionBufferAlloctor)
        _sessionBufferAlloctor->FinishMemory();
}

void FS_EpollMsgDispatcher::SendData(UInt64 sessionId, NetMsg_DataHeader *msg)
{
    if(_isClose)
        return;

    auto session = _GetSession(sessionId);
    if(!session || !session->CanPost())
    {
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("session is missing or cant post sessionid[%llu] cmd[%u], len[%u]")
                                        , sessionId, msg->_cmd, msg->_packetLength);
        return;
    }

    if(!session->PushMsgToSend(msg))
    {
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg cmd[%hu] len[%hu] fail")
                                        , sessionId
                                        , msg->_cmd);
    }

    if(session->IsPostSend())
    {
        auto iterSendQueue = _toPostSend.find(session);
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("session is already post send sessionId[%llu] %s")
                                        , sessionId, iterSendQueue == _toPostSend.end() ? "not in post send queue" : "in post send queue");
        return;
    }

    _toPostSend.insert(session);
}

void FS_EpollMsgDispatcher::SendData(UInt64 sessionId, const Byte8 *msgBuffer, UInt64 msgLen)
{
    if(_isClose)
        return;

    auto session = _GetSession(sessionId);
    if(!session || !session->CanPost())
    {
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("session is missing or cant post sessionid[%llu] len[%llu]")
                                        , sessionId, msgLen);
        return;
    }

    if(!session->PushMsgToSend(msgBuffer, msgLen))
    {
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg len[%llu] fail")
                                       , sessionId
                                       , msgLen);
    }

    if(session->IsPostSend())
    {
        auto iterSendQueue = _toPostSend.find(session);
        g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("session is already post send sessionId[%llu] %s")
                                        , sessionId, iterSendQueue == _toPostSend.end() ? "not in post send queue" : "in post send queue");
        return;
    }

    _toPostSend.insert(session);
}

void FS_EpollMsgDispatcher::CloseSession(UInt64 sessionId)
{
    auto session = _GetSession(sessionId);
    session->CancelRecvIoAndMaskClose(CancelIoReason::ForceClosed, true);
    session->CancelSendIoAndMaskClose(CancelIoReason::ForceClosed, true);
}

void FS_EpollMsgDispatcher::_OnBusinessProcessThread(FS_ThreadPool *pool)
{// 业务层可以不用很频繁唤醒，只等待网络层推送消息过来

    // _timeWheel->GetModifiedResolution(_resolutionInterval);
    const auto &resolutionIntervalSlice = _cfgs->_dispatcherResolutionInterval;
    bool hasMsg = false;

    // 初始化配置
    _transferThreadId = SystemUtil::GetCurrentThreadId();
    g_MemleakMonitor->RegisterMemPoolPrintCallback(_transferThreadId, _printAlloctorOccupiedInfo);

    // 等待其他组件ready
    MaskReady(true);
    g_Log->sys<FS_EpollMsgDispatcher>(_LOGFMT_("business process monitor is working..."));
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

    _CancelAllSessionIosAndWaitForTransferIoFinish();
    _ClearAllSessions();

    // 系统关服
    _CloseLogicSvc();
    g_Log->sys<FS_EpollMsgDispatcher>(_LOGFMT_("dispatcher process thread end compId[%u],threadId[%llu]")
                                      , _compId, SystemUtil::GetCurrentThreadId());
}

void FS_EpollMsgDispatcher::_RunAFrame(bool hasMsg)
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

void FS_EpollMsgDispatcher::_CheckHeartbeat()
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
        _toRemove.insert(session->CastTo<FS_EpollSession>());
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("session heartbeat time out: %s"), session->ToString().c_str());
    }
}

void FS_EpollMsgDispatcher::_OnBusinessProcessing()
{
    // 将网络数据转移到缓冲区
    FS_EpollEvMessageBlock *netMsgBlock = NULL;
    UInt64 sessionId = 0;
    Int32 messageBlockType = FS_EpollEvMessageType::None;
    for(auto iterGeneratorQueue = _recvMsgBlocks->begin(); iterGeneratorQueue != _recvMsgBlocks->end(); ++iterGeneratorQueue)
    {
        auto *blockQueue = *iterGeneratorQueue;
        if(blockQueue->empty())
            continue;

        for(auto iterBlock = blockQueue->begin(); iterBlock != blockQueue->end();)
        {
            netMsgBlock = (*iterBlock)->CastTo<FS_EpollEvMessageBlock>();
            messageBlockType = netMsgBlock->_messageType;
            (this->*_msgBlockHandler[messageBlockType])(netMsgBlock);
            Fs_SafeFree(netMsgBlock);
            iterBlock = blockQueue->erase(iterBlock);
        }
    }
}

void FS_EpollMsgDispatcher::_PostEvents()
{
    // post recv
    for(auto iterRecv = _toPostRecv.begin(); iterRecv != _toPostRecv.end();)
    {
        _DoPostRecv(*iterRecv);
        iterRecv = _toPostRecv.erase(iterRecv);
    }

    // post send
    for(auto iterSend = _toPostSend.begin(); iterSend != _toPostSend.end();)
    {
        _DoPostSend(*iterSend);
        iterSend = _toPostSend.erase(iterSend);
    }
}

void FS_EpollMsgDispatcher::_WillRemoveSessions()
{
    FS_EpollSession *session = NULL;
    for(auto iterSession = _toRemove.begin(); iterSession != _toRemove.end();)
    {
        session = *iterSession;
        _RemoveFromPostRecvQueue(session);
        _RemoveFromPostSendQueue(session);

        // 准备移除,等待transfer返回正式移除session
        _RemoveSessionGracefully(session);
        iterSession = _toRemove.erase(iterSession);
    }
}

void FS_EpollMsgDispatcher::_CancelAllSessionIosAndWaitForTransferIoFinish()
{
    for(auto iterSession = _sessions.begin(); iterSession != _sessions.end();++iterSession)
    {
        auto session = iterSession->second;
        session->EnableDisconnect();
        session->CancelRecvIoAndMaskClose(CancelIoReason::CloseDispatcher, false);
        session->CancelSendIoAndMaskClose(CancelIoReason::CloseDispatcher, false);
        _OnSessionDisconnectedNoFree(session);
    }

    EngineCompsMethods::WaitForAllTransferIoFinish(_engine);
}

void FS_EpollMsgDispatcher::_ClearAllSessions()
{
    for(auto iterSession = _sessions.begin(); iterSession != _sessions.end();)
    {
        Fs_SafeFree(iterSession->second);
        iterSession = _sessions.erase(iterSession);
    }
}

void FS_EpollMsgDispatcher::_CloseLogicSvc()
{
    if(_logic)
    {
        _logic->WillClose();
        _logic->BeforeClose();
        _logic->Close();
        _logic->AfterClose();
    }
}

bool FS_EpollMsgDispatcher::_DoPostRecv(FS_EpollSession *session)
{
    // session不可post或者已经post完成不可重复post
    if(!session->CanPost() || session->IsPostRecv())
    {
         g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("cant post a recv msg sessionId[%llu]"), session->GetSessionId());
         return true;
    }

    Int32 st = session->PostRecv();
    if(st != StatusDefs::Success)
    {
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("PostRecv fail st[%d] sessionId[%llu]"), session->GetSessionId());
        session->MaskClose();
        _toRemove.insert(session);
        return false;
    }

    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("suc post a recv msg sessionId[%llu]"), session->GetSessionId());
    return true;
}

bool FS_EpollMsgDispatcher::_DoPostSend(FS_EpollSession *session)
{
    // 判断是否可以post,是否已经postsend,是否有消息发送
    if(!session->CanPost()
       || session->IsPostSend()
       || !session->HasMsgToSend())
    {
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("cant post a send msg sessionId[%llu]"), session->GetSessionId());
        return true;
    }

    Int32 st = session->PostSend();
    if(st != StatusDefs::Success)
    {
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("PostSend fail st[%d] sessionId[%llu]"), session->GetSessionId());
        session->MaskClose();
        _toRemove.insert(session);
        return false;
    }

    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("suc post a send msg sessionId[%llu]"), session->GetSessionId());
    return true;
}

void FS_EpollMsgDispatcher::_RemoveSessionGracefully(FS_EpollSession *session)
{// 仅仅close掉socket，并等待transfer返回,ondisconnected才是真正的断开
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_EpollMsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s> prepare remove"
                                      , sessionId
                                      , session->GetSocket(), session->GetAddr()->ToString().c_str());
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("sessionId[%llu] sock[%llu] disconnected address<%s> prepare remove")
                                      , sessionId
                                      , session->GetSocket(), session->GetAddr()->ToString().c_str());
    session->MaskClose();
    if(!session->CanDisconnect())
    {// 还有post event还没返回不可立即关闭应该先cancel,保证已经完成的io事件正常返回
        session->CancelRecvIoAndMaskClose(CancelIoReason::ForceClosed, true);
        session->CancelSendIoAndMaskClose(CancelIoReason::ForceClosed, true);
    }
    else
    {
        // close会把在缓冲完全清除,若有post还未返回则会出现丢数据情况
        _OnSessionDisconnected(session);
        _sessions.erase(sessionId);
    }
}

void FS_EpollMsgDispatcher::_UpdateSessionHeartbeat(IFS_Session *session)
{
    _sessionHeartbeatQueue.erase(session);
    session->UpdateHeartBeatExpiredTime();
    _sessionHeartbeatQueue.insert(session);

    const auto &newExpiredTime = session->GetHeartBeatExpiredTime();
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("new heartbeattime[%lld] %s"), newExpiredTime.GetMilliTimestamp(), newExpiredTime.ToString().c_str());
}

void FS_EpollMsgDispatcher::_PrintAlloctorOccupiedInfo()
{
    FS_String memInfo;
    memInfo.AppendFormat("\n【epoll dispatcher alloctor occupied info】\n");
    memInfo.AppendFormat("dispatcher compId[%u] threadId[%llu] alloctor occupied info:[", _compId, _transferThreadId);
    _sessionBufferAlloctor->MemInfoToString(memInfo);
    memInfo.AppendFormat(" ]");
    memInfo.AppendFormat("\n【+++++++++++++++++++++++++ End ++++++++++++++++++++++++++】\n");
    g_Log->mempool("%s", memInfo.c_str());
}

void FS_EpollMsgDispatcher::_OnMsgBlockRecvData(FS_EpollEvMessageBlock *msgBlock)
{
    _OnMsgRecv(msgBlock->CastTo<EpollRecvDataMessageBlock>());
}

void FS_EpollMsgDispatcher::_OnMsgBlockSendSuc(FS_EpollEvMessageBlock *msgBlock)
{
    _OnMsgSendSuc(msgBlock->CastTo<EpollSendSucMessageBlock>());
}

void FS_EpollMsgDispatcher::_OnMsgBlockNetSessionConnected(FS_EpollEvMessageBlock *msgBlock)
{
    auto connectedMsgBlock = msgBlock->CastTo<EpollEvSessionWillConnectMsg>();
    _OnSessionConnected(connectedMsgBlock);
}
// 
// void FS_EpollMsgDispatcher::_OnMsgBlockNetSessionDisconnected(FS_EpollEvMessageBlock *msgBlock)
// {
//     EpollEvDisconnectedMessageBlock *block = msgBlock->CastTo<EpollEvDisconnectedMessageBlock>();
//     auto session = _GetSession(block->_sessionId);
//     if(!session)
//     {
//         g_Log->w<FS_EpollMsgDispatcher>(_LOGFMT_("sessionid[%llu] session is not exist"), block->_sessionId);
//         return;
//     }
// 
//     session->MaskClose();
//     _toRemove.insert(session);
//     g_Log->net<FS_EpollMsgDispatcher>("_OnMsgBlockNetSessionDisconnected session id[%llu] disconnected callback will disconnected", block->_sessionId);
//     g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_OnMsgBlockNetSessionDisconnected session id[%llu] disconnected callback will disconnected"), block->_sessionId);
// }

void FS_EpollMsgDispatcher::_OnMsgBlockConnectorConnectOpFinish(FS_EpollEvMessageBlock *msgBlock)
{
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("connector connect finish"));
    if(_logic)
        _logic->OnConnectorConnectOpFinish();
}

void FS_EpollMsgDispatcher::_OnSessionDisconnectedNoFree(FS_EpollSession *session)
{
    // 从session返回正式移除session
    const auto sessionId = session->GetSessionId();
    g_Log->net<FS_EpollMsgDispatcher>("sessionId[%llu] sock[%llu] disconnected address<%s>"
                                      , sessionId
                                      , session->GetSocket(), session->GetAddr()->ToString().c_str());
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("sessionId[%llu] sock[%llu] disconnected address<%s>")
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
    --_curSessionQuatity;
}

void FS_EpollMsgDispatcher::_OnSessionDisconnected(FS_EpollSession *session)
{
    _OnSessionDisconnectedNoFree(session);
    Fs_SafeFree(session);
}

void FS_EpollMsgDispatcher::_OnSessionConnected(EpollEvSessionWillConnectMsg *connectedMsg)
{
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_(" new session is coming. FS_EpollMsgDispatcher _OnSessionConnected connectedMsg info:\n[%s]")
                  , connectedMsg->ToString().c_str());

    const UInt64 sessionId = connectedMsg->_sessionId;
    ++_curSessionQuatity;

    // 创建session对象并初始化
    auto newSession = FS_SessionFactory::Create(sessionId
                                                , connectedMsg->_compId
                                                , connectedMsg->_acceptorCompId
                                                , connectedMsg->_sock
                                                , &connectedMsg->_addrInfo
                                                , _sessionBufferAlloctor
                                                , _cfgs->_heartbeatDeadTimeMsInterval * Time::_microSecondPerMilliSecond)->CastTo<FS_EpollSession>();

    _sessions.insert(std::make_pair(sessionId, newSession));
    newSession->Bind(connectedMsg->_poller);
    newSession->OnConnect();
    auto addr = newSession->GetAddr();
    g_Log->net<FS_EpollMsgDispatcher>("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]"
                                      , sessionId
                                      , newSession->GetSocket()
                                      , addr->GetAddr().c_str()
                                      , addr->GetPort());
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("new session connected: id<%llu>,socket<%llu>,remote ip[%s:%hu]")
                                      , sessionId
                                      , newSession->GetSocket()
                                      , addr->GetAddr().c_str()
                                      , addr->GetPort());


    const Int32 st = _logic->OnSessionConnected(sessionId);
    if(st != StatusDefs::Success)
    {
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_logic->OnSessionConnected fail st[%d] sessionId[%llu]"), st, sessionId);

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
    else
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_DoPostRecv sessionId[%llu]"), sessionId);
}

void FS_EpollMsgDispatcher::_OnMsgRecv(EpollRecvDataMessageBlock *recvMsg)
{
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("a recv io msg from transfer generatorId[%u] compId[%u] is coming handle sessionId[%llu] transferBytes[%lld]")
                                    , recvMsg->_generatorId, recvMsg->_compId, recvMsg->_sessionId, recvMsg->_transferBytes);
    EpollIoData *ioData = reinterpret_cast<EpollIoData *>(recvMsg->_ioData);
    const UInt64 sessionId = recvMsg->_sessionId;
    const Int64 transferBytes = recvMsg->_transferBytes;
    const bool isRegisterInMonitor = recvMsg->_isSessionInRecverMonitor;

    auto session = _GetSession(sessionId);
    if(!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
        g_Log->net<FS_EpollMsgDispatcher>("_OnMsgRecv sessionId[%llu] is removed before.\n stack trace back:\n%s"
                                          , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());

        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_OnMsgRecv sessionId[%llu] is removed before.\n stack trace back:\n%s")
                                        , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }

    session->ResetPostRecvMask();
    if(session->IsDelayClose() || transferBytes <= 0)
    {// 客户端断开处理
        g_Log->net<FS_EpollMsgDispatcher>("_OnMsgRecv sessionId[%llu] sock[%llu] bytesTrans[%llu] will disconnect IsDelayClose[%d] sessionInfo:\n%s"
                                          , sessionId
                                          , session->GetSocket(),
                                          transferBytes,
                                          session->IsDelayClose()
                                          , session->ToString().c_str());
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_OnMsgRecv sessionId[%llu] sock[%llu] bytesTrans[%llu] will disconnect IsDelayClose[%d] sessionInfo:\n%s")
                                        , sessionId
                                        , session->GetSocket(),
                                        transferBytes,
                                        session->IsDelayClose()
                                        , session->ToString().c_str());

        if(!isRegisterInMonitor) // 已经不在recv注册队列中可以不用cancel
            session->SetRecvIoCanceled();
        _toRemove.insert(session);
        return;
    }

    session->OnRecvSuc(transferBytes, ioData);

    // 收到消息回调
    _engine->_HandleCompEv_RecvMsg(transferBytes);
    _OnSessionMsgHandle(session);
    _UpdateSessionHeartbeat(session);

    // 重新接收数据
    if(session->CanPost())
        _toPostRecv.insert(session);
}

void FS_EpollMsgDispatcher::_OnMsgSendSuc(EpollSendSucMessageBlock *sendSucMsg)
{
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("a send suc io msg from transfer generatorId[%u] compId[%u] is coming handle sessionId[%llu] transferBytes[%lld]")
                                    , sendSucMsg->_generatorId, sendSucMsg->_compId, sendSucMsg->_sessionId, sendSucMsg->_transferBytes);

    EpollIoData *ioData = reinterpret_cast<EpollIoData *>(sendSucMsg->_ioData);
    const UInt64 sessionId = sendSucMsg->_sessionId;
    const Int64 transferBytes = sendSucMsg->_transferBytes;
    const bool isRegisterInMonitor = sendSucMsg->_isSessionInSenderMonitor;

    auto session = _GetSession(sessionId);
    if(!session)
    {// 数据丢失,最大可能是已经断开链接了！！！！
        g_Log->net<FS_EpollMsgDispatcher>("sessionId[%llu] is removed before.\n stack trace back:\n%s"
                                          , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());

        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("sessionId[%llu] is removed before.\n stack trace back:\n%s")
                                          , sessionId, CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
        return;
    }

    session->ResetPostSendMask();
    if(session->IsDelayClose() || transferBytes <= 0)
    {// 客户端断开处理
        g_Log->net<FS_EpollMsgDispatcher>("_OnMsgSendSuc sessionId[%llu] sock[%llu] bytesTrans[%lld] disconnected IsDelayClose[%d], sessionInfo:\n%s"
                                          , sessionId
                                          , session->GetSocket()
                                          , transferBytes
                                          , session->IsDelayClose()
                                          , session->ToString().c_str());

        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("_OnMsgSendSuc sessionId[%llu] sock[%llu] bytesTrans[%lld] disconnected IsDelayClose[%d], sessionInfo:\n%s")
                                          , sessionId
                                          , session->GetSocket()
                                          , transferBytes
                                          , session->IsDelayClose()
                                          , session->ToString().c_str());
        if(!isRegisterInMonitor)
            session->SetSendIoCanceled();
        _toRemove.insert(session);
        return;
    }

    session->OnSendSuc(transferBytes, ioData);

    // 消息发送回调
    _engine->_HandleCompEv_SendMsg(transferBytes);
    // TODO:有被疯狂发包的风险，解决方案：心跳包协议+频繁发包计数+发包成功时更新时间戳
    _UpdateSessionHeartbeat(session);

    // 重新投递发送
    if(session->CanPost() &&
       !session->IsPostSend() &&
       session->HasMsgToSend())
        _toPostSend.insert(session);
}

void FS_EpollMsgDispatcher::_OnSessionMsgHandle(FS_EpollSession *session)
{
    auto recvBuffer = session->CastToRecvBuffer();
    const Byte8 *buffer = NULL;
    const UInt64 sessionId = session->GetSessionId();

    while(recvBuffer->HasMsg())
    {
        _msgDecoder->Decode(recvBuffer->GetData());
        g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("cmd[%d] msg iscoming len[%u]")
                                        , _msgDecoder->GetCmd(), _msgDecoder->GetMsgLen());

        // TODO:处理单一消息业务逻辑部分
        if(_logic)
            _logic->OnMsgDispatch(sessionId, _msgDecoder);

        _engine->_HandleCompEv_RecvMsgAmount();
        recvBuffer->PopFront(_msgDecoder->GetMsgLen());
    }
}

void FS_EpollMsgDispatcher::_MaskCloseSession(FS_EpollSession *session)
{
    g_Log->i<FS_EpollMsgDispatcher>(_LOGFMT_("will _MaskCloseSession sessionId[%llu]")
                                    , session->GetSessionId());
    session->EnableDisconnect();
    session->MaskClose();
    _toRemove.insert(session);
}

FS_NAMESPACE_END
#endif
