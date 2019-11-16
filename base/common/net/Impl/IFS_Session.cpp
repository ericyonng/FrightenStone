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
#include "base/common/net/Defs/FS_IocpBuffer.h"
#include "base/common/net/protocol/protocol.h"
#include "base/common/net/Defs/FS_BufferFactory.h"
#include "base/common/net/Impl/IFS_ServerConfigMgr.h"
#include "base/common/net/Impl/FS_ServerCore.h"

#include "base/common/assist/utils/Impl/STLUtil.h"
#include "base/common/socket/socket.h"

OBJ_POOL_CREATE_DEF_IMPL(fs::IFS_Session, __DEF_OBJ_POOL_OBJ_NUM__)

FS_NAMESPACE_BEGIN

IFS_Session::IFS_Session(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo)
    :_isDestroy(false)
    ,_sessionId(sessionId)
    ,_addr(new FS_Addr(this, addrInfo))
    ,_sock(sock)
    ,_isListen(false)
    ,_recvBuffer(NULL)
    ,_recvMsgId(1)
    ,_sendMsgId(1)
    ,_lastErrorReason{StatusDefs::Success}
    ,_maskClose(false)
    ,_heartbeatInterval(0)
{
    _heartbeatInterval = static_cast<Int64>(g_SvrCfg->GetHeartbeatDeadTimeInterval());
    _recvBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF);
    auto newSendBuff = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF);
#ifdef _WIN32
    _recvBuffer->CastToBuffer<FS_IocpBuffer>()->BindTo(_sessionId, sock);
    newSendBuff->CastToBuffer<FS_IocpBuffer>()->BindTo(_sessionId, sock);
#else
#endif
    _toSend.push_back(newSendBuff);

    UpdateHeartBeatExpiredTime();
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

void IFS_Session::UpdateHeartBeatExpiredTime()
{
    _heartBeatExpiredTime.FlushAppendTime(_heartbeatInterval*Time::_microSecondPerMilliSecond);
}

bool IFS_Session::PushMsgToSend(NetMsg_DataHeader *header)
{
    if(_isDestroy)
        return false;

    if(_toSend.empty())
    {
        auto newBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF);
        _toSend.push_back(newBuffer);
        newBuffer->BindTo(_sessionId, _sock);
    }

    // »º³å¿Õ¼ä²»×ã
    IFS_Buffer *buffer = _toSend.front();
    if(!buffer->CanPush(header->_packetLength))
    {
        buffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF);
        buffer->BindTo(_sessionId, _sock);
        _toSend.push_back(buffer);
    }

    if(!buffer->PushBack(reinterpret_cast<const char *>(header), header->_packetLength))
    {
        g_Log->e<IFS_Session>(_LOGFMT_("push back data fail cmd[%s:%hu] len[%hu]")
                              , ProtocolCmd::GetStr(header->_cmd)
                              , header->_cmd
                              , header->_packetLength);
        return false;
    }

    return true;
}

void IFS_Session::OnDestroy()
{

}

void IFS_Session::OnConnect()
{
    //UpdateHeartBeatExpiredTime();
}

void IFS_Session::OnDisconnect()
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


