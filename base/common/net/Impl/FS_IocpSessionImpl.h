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
 * @file  : FS_IocpSessionImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/14
 * @brief :
 * 
 *
 * 
 */

#ifdef __Base_Common_Net_Impl_FS_IocpSession_H__
#pragma once

FS_NAMESPACE_BEGIN

inline FS_IocpSession::FS_IocpSession(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo)
    :IFS_Session(sessionId, sock, addrInfo)
    ,_sender(NULL)
    ,_isPostRecv(false)
    ,_isPostSend(false)
{
}

inline void FS_IocpSession::BindSender(IDelegate<bool, FS_IocpSession *> *sender)
{
    Fs_SafeFree(_sender);
    _sender = sender;
}

inline void FS_IocpSession::ResetPostRecvMask()
{
    _isPostRecv = false;
}

inline void FS_IocpSession::ResetPostSendMask()
{
    _isPostSend = false;
}

inline bool FS_IocpSession::IsPostIoChange() const
{
    return _isPostRecv || _isPostSend;
}

inline bool FS_IocpSession::IsPostSend() const
{
    return _isPostSend;
}

inline bool FS_IocpSession::IsPostRecv() const
{
    return _isPostRecv;
}

inline bool FS_IocpSession::CanPost() const
{
    return !(_isDestroy || _maskClose || IsClose());
}

inline bool FS_IocpSession::CanDestroy() const
{
    return !IsPostIoChange();
}

inline bool FS_IocpSession::CanDisconnect() const
{
    return !IsPostIoChange();
}

inline void FS_IocpSession::OnSendSuc(size_t transferBytes, IoDataBase *ioData)
{
    _isPostSend = false;
    ioData->_callback->Invoke(transferBytes);
    auto bufferNode = ioData->_node;
    auto buffer = *bufferNode->_iterNode;
    if(buffer->IsEmpty())
    {
        Fs_SafeFree(buffer);
        _toSend.erase(bufferNode->_iterNode);
    }
}

inline void FS_IocpSession::OnRecvSuc(size_t transferBytes, IoDataBase *ioData)
{
    _isPostRecv = false;
    ioData->_callback->Invoke(transferBytes);
}

inline void FS_IocpSession::_OnSend()
{
    _sender->Invoke(this);
}

FS_NAMESPACE_END
#endif
