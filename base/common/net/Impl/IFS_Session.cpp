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
 * @file  : IFS_Session.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/net/Impl/IFS_Session.h"
#include "base/common/net/Impl/FS_Addr.h"
#include "base/common/net/Impl/FS_SessionMgr.h"
#include "base/common/net/Defs/FS_Packet2.h"
#include "base/common/net/Defs/FS_IocpBuffer.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/net/Defs/FS_BufferFactory.h"

#include "base/common/assist/utils/Impl/STLUtil.h"

OBJ_POOL_CREATE_DEF_IMPL(fs::IFS_Session, __DEF_OBJ_POOL_OBJ_NUM__)

FS_NAMESPACE_BEGIN

IFS_Session::IFS_Session(UInt64 sessionId, SOCKET sock, FS_SessionMgr *sessionMgr)
    :_isDestroy(false)
    ,_sessionId(sessionId)
    ,_addr(new FS_Addr)
    ,_sock(sock)
    ,_recvBuffer(NULL)
    ,_sessionMgr(sessionMgr)
    ,_recvMsgId(1)
    ,_sendMsgId(1)
{
    _recvBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF);
}

IFS_Session::~IFS_Session()
{
    _Destroy();
}

bool IFS_Session::HasMsgToRead() const
{
#ifdef _WIN32
    return _recvBuffer->CastToBuffer<FS_IocpBuffer>()->HasMsg();
#else
#endif
}

void IFS_Session::Close()
{
    if(_sock != INVALID_SOCKET)
        SocketUtil::DestroySocket(_sock);
}

bool IFS_Session::Send(NetMsg_DataHeader *header)
{
    if(_isDestroy)
        return false;

    auto newBuffer = FS_BufferFactory::Create(static_cast<size_t>(header->_packetLength));
#ifdef _WIN32
    if(!newBuffer->CastToBuffer<FS_IocpBuffer>()->BindTo(_sessionId, _sock))
    {
        g_Log->e<IFS_Session>(_LOGFMT_("has bind to sessionId[%llu] sock[%llu]"), _sessionId, _sock);
    }

#else
#endif

    if(!newBuffer->PushBack(reinterpret_cast<const char *>(header), header->_packetLength))
    {
        g_Log->e<IFS_Session>(_LOGFMT_("newBuffer push back data fail"));
        return false;
    }

    _toSend.push_back(newBuffer);
    return _OnSend();
}

void IFS_Session::OnDestroy()
{

}

void IFS_Session::OnConnect()
{

}

void IFS_Session::OnHeartBeatTimeOut()
{

}

void IFS_Session::OnMsgArrived()
{
}

void IFS_Session::_Destroy()
{
    _lock.Lock();
    if(_isDestroy)
    {
        _lock.Unlock();
        return;
    }
    _isDestroy = true;

    Fs_SafeFree(_addr);
    if(_sock != INVALID_SOCKET)
    {
        SocketUtil::DestroySocket(_sock);
        _sock = INVALID_SOCKET;
    }

    Fs_SafeFree(_recvBuffer);
    STLUtil::DelListContainer(_toSend);
    _lock.Unlock();
}

FS_NAMESPACE_END


