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
inline void FS_IocpMsgTransfer::_OnGracefullyDisconnect(IFS_Session *session)
{
    if(!session->CanDisconnect())
    {
        _OnDelayDisconnected(session);
        return;
    }

    _OnDisconnected(session);
}

inline void FS_IocpMsgTransfer::_OnMsgArrived()
{
    // ������Ϣ����
    for(auto iterSession = _msgArriviedSessions.begin(); iterSession != _msgArriviedSessions.end();)
    {
        auto session = *iterSession;
        _serverCoreRecvAmountCallback->Invoke(session);
        iterSession = _msgArriviedSessions.erase(iterSession);
    }
}

inline void FS_IocpMsgTransfer::_RemoveSessions(std::set<IFS_Session *> &sessions)
{
    for(auto &session : sessions)
    {
        _OnDisconnected(session);
        // _OnGracefullyDisconnect(session);
    }
}

inline void FS_IocpMsgTransfer::_OnHeartbeatTimeOut(const std::set<UInt64> &timeoutSessionIds, std::set<UInt64> &leftSessionIdsToRemove)
{
    for(auto &sessionId : timeoutSessionIds)
    {
        leftSessionIdsToRemove.erase(sessionId);
        auto session = _GetSession(sessionId);
        if(!session)
            continue;

        OnHeartBeatTimeOut(session);
    }
}

FS_NAMESPACE_END
#endif
