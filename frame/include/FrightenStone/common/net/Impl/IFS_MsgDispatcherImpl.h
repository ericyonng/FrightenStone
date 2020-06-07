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
 * @file  : IFS_MsgDispatcherImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/01/05
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgDispatcher_H__
#pragma once

FS_NAMESPACE_BEGIN

inline IFS_MsgDispatcher::IFS_MsgDispatcher(IFS_NetEngine *engine, UInt32 compId)
    :IFS_EngineComp(engine, compId, EngineCompType::Dispatcher)
    , _generatorId(0)
    , _consumerId(0)
    , _concurrentMq(NULL)
    , _pool(NULL)
    , _isClose{ false }
    , _timeWheel(NULL)
    , _logic(NULL)
    , _recvMsgBlocks(NULL)
    , _cfgs(NULL)
    , _transferThreadId(0)
    , _sessionBufferAlloctor(NULL)
    , _printAlloctorOccupiedInfo(NULL)
    , _curSessionQuatity{ 0 }
{
}

inline UInt32 IFS_MsgDispatcher::GetGeneratorId() const
{
    return _generatorId;
}

inline UInt32 IFS_MsgDispatcher::GetConsumerId() const
{
    return _consumerId;
}

inline Int32 IFS_MsgDispatcher::GetSessionCnt() const
{
    return _curSessionQuatity;
}

inline TimeWheel *IFS_MsgDispatcher::GetTimeWheel()
{
    return _timeWheel;
}

inline bool IFS_MsgDispatcher::IsClose() const
{
    return _isClose;
}

inline void IFS_MsgDispatcher::BindConcurrentParams(UInt32 generatorId, UInt32 consumerId, ConcurrentMessageQueueNoThread *concurrentMq)
{
    _generatorId = generatorId;
    _consumerId = consumerId;
    _concurrentMq = concurrentMq;
}

inline void IFS_MsgDispatcher::BindBusinessLogic(IFS_BusinessLogic *businessLogic)
{
    _logic = businessLogic;
}

inline IFS_BusinessLogic *IFS_MsgDispatcher::GetLogic()
{
    return _logic;
}

inline const IFS_BusinessLogic *IFS_MsgDispatcher::GetLogic() const
{
    return _logic;
}

inline const FS_Session *IFS_MsgDispatcher::GetSession(UInt64 sessionId) const
{
    auto iterSession = _sessions.find(sessionId);
    if (iterSession == _sessions.end())
        return NULL;

    return iterSession->second;
}

inline FS_Session *IFS_MsgDispatcher::GetSession(UInt64 sessionId)
{
    auto iterSession = _sessions.find(sessionId);
    if (iterSession == _sessions.end())
        return NULL;

    return iterSession->second;
}

inline void IFS_MsgDispatcher::CloseSession(UInt64 sessionId)
{
    // 延迟关闭，有可能在关闭时候，session还有send或者recv还没处理,所以需要延迟关闭
    auto session = GetSession(sessionId);
    if (!session)
        return;

    _DelayCloseSession(session);
}

inline void IFS_MsgDispatcher::SendData(UInt64 sessionId, const Byte8 *msgBuffer, UInt64 msgLen)
{
    if (_isClose)
        return;

    if (msgLen > FS_BUFF_SIZE_DEF)
    {
        g_Log->e<IFS_MsgDispatcher>(_LOGFMT_("cant send a package larger than buffer, buffersize[%d] package len[%u]")
            , FS_BUFF_SIZE_DEF, msgLen);
        return;
    }

    auto session = GetSession(sessionId);
    if (!session || !session->IsValid()) 
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("sessionid[%llu] session not found or is invalid cant send data")
            , sessionId);
        return;
    }

    if (!session->PushMsgToSend(msgBuffer, msgLen))
    {
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("sessionid[%llu] send msg len[%llu] fail")
            , sessionId
            , msgLen);
    }

    if (session->IsPostSend())
    {
        auto iterSendQueue = _toPostSend.find(session);
        g_Log->w<IFS_MsgDispatcher>(_LOGFMT_("session is already post send sessionId[%llu] %s")
            , sessionId, iterSendQueue == _toPostSend.end() ? "not in post send queue" : "in post send queue");
        return;
    }

//     if (session->IsPostSend()
//         || !session->HasMsgToSend())
//     {
//         g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("sessionId[%llu] already posted or has no msg to send"), session->GetSessionId());
//         return true;
//     }

    // 加快响应,因为如果队列很长的话响应会很慢
    // send延后发送,进行优化
    // _DoPostSend(session);   
    _toPostSend.insert(session);
}

inline void IFS_MsgDispatcher::MaskSend(FS_Session *session)
{
    _toPostSend.insert(session);
}

inline void IFS_MsgDispatcher::_RemoveFromPostRecvQueue(FS_Session *session)
{
    _toPostRecv.erase(session);
}

inline void IFS_MsgDispatcher::_RemoveFromPostSendQueue(FS_Session *session)
{
    _toPostSend.erase(session);
}

inline void IFS_MsgDispatcher::_UpdateSessionHeartbeat(FS_Session *session)
{
    _sessionHeartbeatQueue.erase(session);
    session->UpdateHeartBeatExpiredTime();
    _sessionHeartbeatQueue.insert(session);

#ifndef _WIN32
//     const auto &newExpiredTime = session->GetHeartBeatExpiredTime();
//     g_Log->i<IFS_MsgDispatcher>(_LOGFMT_("new heartbeattime[%lld] %s")
//         , newExpiredTime.GetMilliTimestamp(), newExpiredTime.ToString().c_str());
#endif
}

inline void IFS_MsgDispatcher::_OnMsgBlockNetSessionConnected(FS_NetMsgBlock *msgBlock)
{
    auto connectedMsgBlock = msgBlock->CastTo<SessionConnectedNetMsgBlock>();
    _OnSessionConnected(connectedMsgBlock);
}

inline void IFS_MsgDispatcher::_OnSessionDisconnected(FS_Session *session)
{
    // 从session返回正式移除session
    _OnSessionDisconnectedNoFree(session);

    // 释放资源
    Fs_SafeFree(session);
}

#pragma region net msg handle

#ifdef _WIN32
inline void IFS_MsgDispatcher::_OnMsgBlockNetMsgArrived(FS_NetMsgBlock *msgBlock)
{
    auto arrivedMsg = msgBlock->CastTo<IocpMsgArrivedMsgBlock>();
    _OnMsgArrived(arrivedMsg);
}
#else

inline void IFS_MsgDispatcher::_OnMsgBlockRecvData(FS_NetMsgBlock *msgBlock)
{
    _OnMsgRecv(msgBlock->CastTo<EpollRecvDataMsgBlock>());
}

inline void IFS_MsgDispatcher::_OnMsgBlockSendSuc(FS_NetMsgBlock *msgBlock)
{
    _OnMsgSendSuc(msgBlock->CastTo<EpollSendSucMsgBlock>());
}

#endif
#pragma endregion

inline void IFS_MsgDispatcher::_DelayCloseSession(FS_Session *session)
{
    session->MaskClose();
    _toRemove.insert(session);
}
FS_NAMESPACE_END

#endif
