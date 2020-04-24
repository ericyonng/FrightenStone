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
 * @file  : FS_EpollSessionImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/01
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollSession_H__
#pragma once

#ifndef _WIN32

FS_NAMESPACE_BEGIN
inline FS_EpollSession::FS_EpollSession(UInt64 sessionId
                                      , UInt32 transferCompId
                                      , UInt32 acceptorCompId
                                      , SOCKET sock
                                      , const sockaddr_in *addrInfo
                                      , IMemoryAlloctor *memAlloctor
                                      , Int64 heartbeatIntervalMicroSeconds)
    :IFS_Session(sessionId, transferCompId, acceptorCompId, sock, addrInfo, memAlloctor, heartbeatIntervalMicroSeconds)
    , _isPostRecv(false)
    , _isPostSend(false)
    , _transferPoller(NULL)
    ,_epoll(NULL)
    , _isRecvIoCanceled(false)
    , _isSendIoCanceled(false)
{
}

inline void FS_EpollSession::ResetPostRecvMask()
{
    _isPostRecv = false;
}

inline void FS_EpollSession::ResetPostSendMask() 
{
    _isPostSend = false;
}

inline void FS_EpollSession::SetRecvIoCanceled()
{
    _isRecvIoCanceled = true;
}

inline void FS_EpollSession::SetSendIoCanceled()
{
    _isSendIoCanceled = true;
}

inline void FS_EpollSession::EnableDisconnect()
{
    _isPostSend = false;
    _isPostRecv = false;
}

inline FS_EpollBuffer *FS_EpollSession::CastToRecvBuffer()
{
    return reinterpret_cast<FS_EpollBuffer *>(_recvBuffer);
}

inline bool FS_EpollSession::IsPostIoChange() const
{
    return _isPostRecv || _isPostSend;
}

inline bool FS_EpollSession::IsPostSend() const
{
    return _isPostSend;
}

inline bool FS_EpollSession::IsPostRecv() const
{
    return _isPostRecv;
}

inline bool FS_EpollSession::IsRecvIoCanceled() const
{
    return _isRecvIoCanceled;
}

inline bool FS_EpollSession::IsSendIoCaceled() const
{
    return _isSendIoCanceled;
}

inline bool FS_EpollSession::CanPost() const
{
    return !(_isDestroy || _maskClose || IsClose());
}

inline bool FS_EpollSession::CanDestroy() const
{
    return !IsPostIoChange();
}

inline bool FS_EpollSession::CanDisconnect() const
{
    return !IsPostIoChange();
}

FS_NAMESPACE_END

#endif

#endif
