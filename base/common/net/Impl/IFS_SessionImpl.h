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
 * @file  : IFS_SessionImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Net_Impl_IFS_Session_H__
#pragma once
FS_NAMESPACE_BEGIN

inline UInt64 IFS_Session::GetSessionId() const
{
    return _sessionId;
}

inline SOCKET IFS_Session::GetSocket() const
{
    return _sock;
}

inline const FS_Addr *IFS_Session::GetAddr() const
{
    return _addr;
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

inline void IFS_Session::Lock()
{
    _lock.Lock();
}

inline void IFS_Session::Unlock()
{
    _lock.Unlock();
}

inline void IFS_Session::UpdateHeartBeatExpiredTime()
{
    _heartBeatExpiredTime.FlushAppendTime(CLIENT_HREAT_DEAD_TIME*Time::_microSecondPerMilliSecond);
}

inline bool IFS_Session::IsClose() const
{
    return _sock == INVALID_SOCKET;
}

const Time &IFS_Session::GetHeartBeatExpiredTime() const
{
    return _heartBeatExpiredTime;
}

FS_NAMESPACE_END

#endif
