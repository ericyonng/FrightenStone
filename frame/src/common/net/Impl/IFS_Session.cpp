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
 * @file  : IFS_Session.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/9/30
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_Session.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Defs/IFS_NetBuffer.h"
#include "FrightenStone/common/net/Defs/FS_BufferFactory.h"
#include "FrightenStone/common/net/Impl/IFS_ConfigMgr.h"

#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/socket/socket.h"


FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_ANCESTOR_IMPL(IFS_Session, __DEF_OBJ_POOL_OBJ_NUM__);
OBJ_POOL_CREATE_ANCESTOR_IMPL(BuildSessionInfo, __DEF_OBJ_POOL_OBJ_NUM__);

IFS_Session::IFS_Session(BuildSessionInfo &buildInfo)
    :_isDestroy(false)
    ,_sessionId(buildInfo._sessionId)
    ,_transferCompId(buildInfo._transferCompId)
    ,_acceptorCompId(buildInfo._acceptorCompId)
    , _connectorCompId(buildInfo._connectorCompId)
    , _protocolPort(buildInfo._protocolPort)
    , _srcCompType(buildInfo._srcCompType)
    ,_sock(buildInfo._sock)
    ,_isListen(false)
    ,_recvBuffer(NULL)
    ,_lastErrorReason{StatusDefs::Success}
    ,_maskClose(false)
    ,_heartbeatIntervalMicroSeconds(buildInfo._heartbeatIntervalMicroSeconds)
    ,_alloctor(buildInfo._memAlloctor)
{
    _addr = new FS_Addr(this, buildInfo._addrInfo);
    _recvBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
    _recvBuffer->BindTo(_sessionId, _sock);

    auto newSendBuff = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
    newSendBuff->BindTo(_sessionId, _sock);
    _toSend.push_back(newSendBuff);

    UpdateHeartBeatExpiredTime();
}

IFS_Session::~IFS_Session()
{
    _Destroy();
}

bool IFS_Session::HasMsgToRead() const
{
    return _recvBuffer->IsEmpty();
}

bool IFS_Session::HasMsgToSend() const
{
    return _toSend.empty() ? false : !(*_toSend.begin())->IsEmpty();
}

FS_String IFS_Session::ToString() const
{
    FS_String info;
    info.AppendFormat("_sessionId:%llu\n", _sessionId)
        .AppendFormat("_sock:%d\n", _sock)
        .AppendFormat("addr:%s\n", _addr->IsSucInit() ? _addr->ToString().c_str() : "Not Init")
        .AppendFormat("isDestroy:%d\n", _isDestroy)
        .AppendFormat("maskClose:%d\n", _maskClose)
        .AppendFormat("transferCompId:%u\n", _transferCompId)
        .AppendFormat("acceptorCompId:%u\n", _acceptorCompId)
        .AppendFormat("_connectorCompId:%u\n", _connectorCompId)
        .AppendFormat("_protocolPort:%u\n", _protocolPort)
        .AppendFormat("_srcCompType:%u\n", _srcCompType)
        .AppendFormat("heartBeatExpiredTime:%lld, %s\n", _heartBeatExpiredTime.GetMilliTimestamp(), _heartBeatExpiredTime.ToString().c_str())
        .AppendFormat("heartbeatIntervalMicroSeconds:%lld\n", _heartbeatIntervalMicroSeconds)
//        .AppendFormat("alloctorInfo:");
//    _alloctor->MemInfoToString(info);
//    info.AppendFormat("\n")
       .AppendFormat("recvBuffer:%s\n", _recvBuffer->ToString().c_str());

    Int32 id = 0;
    info.AppendFormat("sendBuffers:\n");
    for(auto sendBuffer : _toSend)
    {
        ++id;
        info.AppendFormat("[%d]:%s\n", id, sendBuffer->ToString().c_str());
    }

//     id = 0;
//     info.AppendFormat("recvBuffers:\n");
//     for (auto recvBuffer : _recvBuffer)
//     {
//         ++id;
//         info.AppendFormat("[%d]:%s\n", id, recvBuffer->ToString().c_str());
//     }

    return info;
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

bool IFS_Session::PushMsgToSend(const Byte8 *data, UInt64 len)
{
    if(_isDestroy)
        return false;

//     if(_toSend.empty())
//     {
//         auto newBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
//         _toSend.push_back(newBuffer);
//         newBuffer->BindTo(_sessionId, _sock);
//     }

    // 缓冲空间不足 当单包比一个buffer还要大呢？需要创建多个buffer容纳一个单包 TODO:需要发包测试
    IFS_NetBuffer *buffer = _toSend.back();
    size_t suitableBytes = 0;
    while (len)
    {
        // 获取可以容纳的合适的大小
        suitableBytes = buffer->GetSuitableBytesToPush(len);

        // 没有空间了则新创建一个buffer
        if (!suitableBytes)
        {
            buffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
            _toSend.push_back(buffer);
            buffer->BindTo(_sessionId, _sock);
            suitableBytes = buffer->GetSuitableBytesToPush(len);
        }

        buffer->PushBack(data, suitableBytes);
        data += suitableBytes;
        len -= suitableBytes;
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
    Close();
    _sock = INVALID_SOCKET;

    Fs_SafeFree(_recvBuffer);
    STLUtil::DelListContainer(_toSend);
}

IFS_NetBuffer *IFS_Session::NewSendBuffer()
{
    auto newBuffer = FS_BufferFactory::Create(FS_BUFF_SIZE_DEF, _alloctor);
    _toSend.push_back(newBuffer);
    newBuffer->BindTo(_sessionId, _sock);
    return newBuffer;
}

FS_NAMESPACE_END


