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
 * @file  : FS_IocpMsgTransferImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */

#ifdef __Base_Common_Net_Impl_FS_IocpMsgTransfer_H__
#pragma once

FS_NAMESPACE_BEGIN
inline Int32 FS_IocpMsgTransfer::GetSessionCnt()
{
    return _sessionCnt;
}

inline void FS_IocpMsgTransfer::AttachMsgQueue(ConcurrentMessageQueue *messageQueue, Int32 generatorId)
{
    _messageQueue = messageQueue;
    _generatorId = generatorId;
}

inline void FS_IocpMsgTransfer::AttachSenderMsgQueue(MessageQueue *messageQueue)
{
    _senderMessageQueue = messageQueue;
}

inline Int32 FS_IocpMsgTransfer::_DoEvents()
{
    Int32 ret = StatusDefs::Success;
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

inline void FS_IocpMsgTransfer::_RemoveSessions(bool forceDisconnect)
{
    FS_IocpSession *session = NULL;
    for(auto iterSsession = _toRemove.begin(); iterSsession != _toRemove.end();)
    {
        session = *iterSsession;
        _toPostRecv.erase(session);
        _toPostSend.erase(session);
        if(!forceDisconnect)
        {
            _OnGracefullyDisconnect(session);
        }
        else
        {
            session->MaskClose();
            session->ResetAllIoMask();
            _OnDisconnected(session);
        }

        iterSsession = _toRemove.erase(iterSsession);
        // _OnGracefullyDisconnect(session);
    }
}

inline void FS_IocpMsgTransfer::_OnGracefullyDisconnect(FS_IocpSession *session)
{
    if(!session->CanDisconnect())
    {
        _OnDelayDisconnected(session);
        return;
    }

    _OnDisconnected(session);
}

inline FS_IocpSession *FS_IocpMsgTransfer::_GetSession(UInt64 sessionId)
{
    auto iterSession = _sessions.find(sessionId);
    if(iterSession == _sessions.end())
        return NULL;

    return iterSession->second;
}

inline void FS_IocpMsgTransfer::_UpdateSessionHeartbeat(IFS_Session *session)
{
    _sessionHeartbeatQueue.erase(session);
    session->UpdateHeartBeatExpiredTime();
    _sessionHeartbeatQueue.insert(session);
}

inline void FS_IocpMsgTransfer::_CancelSessionWhenTransferZero(FS_IocpSession *session)
{
    _toRemove.insert(session);
    _toPostRecv.erase(session);
    _toPostSend.erase(session);
}

inline void FS_IocpMsgTransfer::_UpdateCanCreateNewNodeForAlloctor(size_t addOccupiedBytes)
{
    if(!_canCreateNewNodeForAlloctor)
        return;

    _curAlloctorOccupiedBytes += addOccupiedBytes;
    if(_curAlloctorOccupiedBytes >= _maxAlloctorBytes)
        _canCreateNewNodeForAlloctor = false;
}

inline void FS_IocpMsgTransfer::_PrintAlloctorOccupiedInfo()
{
    FS_String memInfo;
    memInfo.AppendFormat("\n¡¾iocp transfer alloctor occupied info¡¿\n");
    memInfo.AppendFormat("transfer id[%d] threadId[%lu] alloctor occupied info:[", _id, _transferThreadId);
    _sessionBufferAlloctor->MemInfoToString(memInfo);
    memInfo.AppendFormat("total occupied bytes[%llu], in used bytes[%llu]."
                         , _sessionBufferAlloctor->GetOccupiedBytes()
                         , _sessionBufferAlloctor->GetInUsedBytes());
    memInfo.AppendFormat(" ]");
    memInfo.AppendFormat("\n¡¾+++++++++++++++++++++++++ End ++++++++++++++++++++++++++¡¿\n");
    g_Log->mempool("%s", memInfo.c_str());
}
FS_NAMESPACE_END
#endif

