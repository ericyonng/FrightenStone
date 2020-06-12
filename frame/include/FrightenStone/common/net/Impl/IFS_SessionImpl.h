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
 * @file  : IFS_SessionImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Session_H__
#pragma once
FS_NAMESPACE_BEGIN

inline UInt64 IFS_Session::GetSessionId() const
{
    return _sessionId;
}

inline UInt32 IFS_Session::GetTransferCompId() const
{
    return _transferCompId;
}

inline UInt32 IFS_Session::GetSrcAcceptorCompId() const
{
    return _acceptorCompId;
}

inline UInt32 IFS_Session::GetSrcConnectorCompId() const
{
    return _connectorCompId;
}

inline UInt16 IFS_Session::GetProtocolPort() const
{
    return _protocolPort;
}

inline Int32 IFS_Session::GetSrcCompType() const
{
    return _srcCompType;
}

inline SOCKET IFS_Session::GetSocket() const
{
    return _sock;
}

inline const FS_Addr *IFS_Session::GetAddr() const
{
    return _addr;
}

inline bool IFS_Session::IsListen() const
{
    return _isListen;
}

inline void IFS_Session::SetListener(bool isListen)
{
    _isListen = isListen;
}

template<typename ObjType>
inline ObjType *IFS_Session::CastTo()
{
    return reinterpret_cast<ObjType *>(this);
}

template<typename ObjType>
inline const ObjType *IFS_Session::CastTo() const
{
    return reinterpret_cast<ObjType *>(this);
}

inline bool IFS_Session::IsClose() const
{
    return _sock == INVALID_SOCKET;
}

inline const Time &IFS_Session::GetHeartBeatExpiredTime() const
{
    return _heartBeatExpiredTime;
}

inline bool IFS_Session::CanDestroy() const
{
    return true;
}

inline bool IFS_Session::CanDisconnect() const
{
    return true;
}

inline void IFS_Session::MaskClose()
{
    g_Log->net<IFS_Session>("mask close stackbacktrace:\n %s", CrashHandleUtil::FS_CaptureStackBackTrace().c_str());
    _maskClose = true;
}

inline bool IFS_Session::IsDelayClose() const
{
    return _maskClose;
}

inline IFS_NetBuffer *IFS_Session::GetRecvBuffer()
{
    return _recvBuffer;
}

inline bool IFS_Session::IsValid() const
{
    return !_isDestroy && !_maskClose && _sessionId != 0 && _sock != INVALID_SOCKET;
}

inline bool IFS_Session::IsDestroy() const
{
    return _isDestroy;
}

inline bool IFS_Session::IsSendBufferListEmpty() const
{
    return _toSend.empty();
}

inline IFS_NetBuffer *IFS_Session::GetSendBuffer()
{
//     if (_toSend.empty())
//         return NULL;

    return _toSend.back();
}

FS_NAMESPACE_END

#endif
