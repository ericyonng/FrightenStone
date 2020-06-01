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
 * @file  : FS_SessionImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/17
 * @brief : 会话对象
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_FS_Session_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_Session::FS_Session(BuildSessionInfo &buildData)
    :IFS_Session(buildData)
    , _isPostRecv(false)
    , _isPostSend(false)
    , _msgHandler(NULL)
    , _protocolPort(0)
#ifdef _WIN32
    , _iocp(NULL)
#else
    , _transferPoller(NULL)
    , _epoll(NULL)
    , _isRecvIoCanceled(false)
    , _isSendIoCanceled(false)
#endif
{
}

inline void FS_Session::ResetPostRecvMask()
{
    _isPostRecv = false;
}

inline void FS_Session::ResetPostSendMask()
{
    _isPostSend = false;
}

inline void FS_Session::EnableDisconnect()
{
    _isPostSend = false;
    _isPostRecv = false;
}

inline bool FS_Session::IsPostIoChange() const
{
    return _isPostRecv || _isPostSend;
}

inline bool FS_Session::IsPostSend() const
{
    return _isPostSend;
}

inline bool FS_Session::IsPostRecv() const
{
    return _isPostRecv;
}

inline bool FS_Session::CanPost() const
{
    return !(_isDestroy || _maskClose || IsClose());
}

inline bool FS_Session::CanDestroy() const
{
    return !IsPostIoChange();
}

inline bool FS_Session::CanDisconnect() const
{
    return !IsPostIoChange();
}

inline void FS_Session::BindMsgHandler(fs::IDelegate<void, fs::FS_Session *> *msgHandler)
{
    _msgHandler = msgHandler;
}

inline void FS_Session::InvokeMsgHandler()
{
    _msgHandler->Invoke(this);
}

#pragma region platform ralation
#ifdef _WIN32
inline void FS_Session::Bind(FS_Iocp *iocp)
{
    _iocp = iocp;
}

#else

inline bool FS_Session::IsRecvIoCanceled() const
{
    return _isRecvIoCanceled;
}

inline bool FS_Session::IsSendIoCaceled() const
{
    return _isSendIoCanceled;
}

inline void FS_Session::SetRecvIoCanceled()
{
    _isRecvIoCanceled = true;
}

inline void FS_Session::SetSendIoCanceled()
{
    _isSendIoCanceled = true;
}
#endif
#pragma endregion
FS_NAMESPACE_END

#endif
