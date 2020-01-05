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
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Impl/FS_SessionMgr.h"
#include "FrightenStone/common/net/Defs/FS_IocpBuffer.h"
#include "FrightenStone/common/net/ProtocolInterface/protocol.h"
#include "FrightenStone/common/net/Defs/FS_BufferFactory.h"
#include "FrightenStone/common/net/Impl/IFS_ServerConfigMgr.h"

#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/socket/socket.h"


FS_NAMESPACE_BEGIN

IFS_Session::IFS_Session(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo, IMemoryAlloctor *memAlloctor, Int64 heartbeatIntervalMicroSeconds)
    :_isDestroy(false)
    ,_sessionId(sessionId)
    ,_sock(sock)
    ,_isListen(false)
    ,_recvBuffer(NULL)
    ,_recvMsgId(1)
    ,_sendMsgId(1)
    ,_lastErrorReason{StatusDefs::Success}
    ,_maskClose(false)
    ,_heartbeatIntervalMicroSeconds(heartbeatIntervalMicroSeconds)
    ,_alloctor(memAlloctor)
{
    _addr = new FS_Addr(this, addrInfo);
    _recvBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
    _recvBuffer->BindTo(_sessionId, sock);

    auto newSendBuff = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
    newSendBuff->BindTo(_sessionId, sock);
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
    // TODO:Linux
    return false;
#endif
}

void IFS_Session::Close()
{
    if(_sock != INVALID_SOCKET)
        SocketUtil::DestroySocket(_sock);
}

void IFS_Session::UpdateHeartBeatExpiredTime()
{
    _heartBeatExpiredTime.FlushAppendTime(_heartbeatIntervalMicroSeconds);
}

void IFS_Session::ResetAddr()
{
    _addr->Reset();
}

bool IFS_Session::PushMsgToSend(NetMsg_DataHeader *header)
{
    if(_isDestroy)
        return false;

    if(_toSend.empty())
    {
        auto newBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
        _toSend.push_back(newBuffer);
        newBuffer->BindTo(_sessionId, _sock);
    }

    // 缓冲空间不足
    IFS_Buffer *buffer = _toSend.back();
    if(!buffer->CanPush(header->_packetLength))
    {
        buffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
        buffer->BindTo(_sessionId, _sock);
        _toSend.push_back(buffer);
    }

    if(!buffer->PushBack(reinterpret_cast<const char *>(header), header->_packetLength))
    {
        g_Log->e<IFS_Session>(_LOGFMT_("push back data fail cmd[%hu] len[%hu]")
                              , header->_cmd
                              , header->_packetLength);
        return false;
    }

    return true;
}

bool IFS_Session::PushMsgToSend(const Byte8 *data, UInt64 len)
{
    if(_isDestroy)
        return false;

    if(_toSend.empty())
    {
        auto newBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
        _toSend.push_back(newBuffer);
        newBuffer->BindTo(_sessionId, _sock);
    }

    // 缓冲空间不足
    IFS_Buffer *buffer = _toSend.back();
    if(!buffer->CanPush(len))
    {
        buffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
        buffer->BindTo(_sessionId, _sock);
        _toSend.push_back(buffer);
    }

    if(!buffer->PushBack(data, len))
    {
        g_Log->e<IFS_Session>(_LOGFMT_("push back data fail len[%llu]")
                              , len);
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
    if(!_addr->IsSucInit())
        _addr->Reset();
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
    if(_isDestroy)
    {
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
}

FS_NAMESPACE_END


