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
 * @file  : FS_EpollSession.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/01
 * @brief :
 */
#include "stdafx.h"

#ifndef _WIN32

#include "FrightenStone/common/net/Impl/FS_EpollSession.h"
#include "FrightenStone/common/net/Impl/FS_EpollPoller.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Defs/FS_EpollBuffer.h"
#include "FrightenStone/common/net/Defs/CancelIoReason.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/socket/socket.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_EpollSession, __DEF_OBJ_POOL_OBJ_NUM__)
FS_EpollSession::~FS_EpollSession()
{
    if(!CanDestroy())
        g_Log->e<FS_EpollSession>(_LOGFMT_("has io uncompleted please check"));
    Close();
}

void FS_EpollSession::Close()
{
    if(_sock != INVALID_SOCKET && !_epoll->IsDestroy())
        _epoll->DelEvent(_sock, _sessionId);
    CancelRecvIoAndMaskClose(CancelIoReason::ForceClosed, false);
    CancelSendIoAndMaskClose(CancelIoReason::ForceClosed, false);
    IFS_Session::Close();
}

void FS_EpollSession::Bind(FS_EpollPoller *transferPoller)
{
    _transferPoller = transferPoller;
    _epoll = _transferPoller->GetEpollObj();
}

FS_String FS_EpollSession::ToString() const
{
    FS_String info;
    info.AppendFormat("%s\n", IFS_Session::ToString().c_str())
        .AppendFormat("_isPostRecv:%d\n", _isPostRecv)
        .AppendFormat("_isPostSend:%d\n", _isPostSend)
        .AppendFormat("_isRecvIoCanceled:%d\n", _isRecvIoCanceled)
        .AppendFormat("_isSendIoCanceled:%d", _isSendIoCanceled);

    return info;
}

// post
Int32 FS_EpollSession::PostRecv()
{
    _isPostRecv = true;
    auto ioData = _recvBuffer->CastToBuffer<FS_EpollBuffer>()->MakeRecvIoData();
    if(!ioData)
    {
        ResetPostRecvMask();
        g_Log->w<FS_EpollSession>(_LOGFMT_("make recv io data fail sessionId[%llu] socket[%d] post recv fail")
                                  , _sessionId, _sock);
        return StatusDefs::Success;
    }

    if(!_transferPoller->PushRecvMsg(ioData))
    {
        ResetPostRecvMask();
        g_Log->e<FS_EpollSession>(_LOGFMT_("sessionId[%llu] socket[%d] post recv fail")
                                  , _sessionId, _sock);

        return StatusDefs::SockError;
    }

    return StatusDefs::Success;
}

Int32 FS_EpollSession::PostSend()
{
    _isPostSend = true;
    auto buffer = _toSend.front()->CastToBuffer<FS_EpollBuffer>();
    auto ioData = buffer->MakeSendIoData();
    if(!ioData)
    {
        ResetPostSendMask();
        g_Log->w<FS_EpollSession>(_LOGFMT_("make send io data fail sessionId[%llu] socket[%d] post send fail")
                                  , _sessionId, _sock);
        return StatusDefs::Success;
    }

    if(!ioData->_node)
        ioData->_node = new BufferQueueNode;

    auto node = ioData->_node;
    node->_iterNode = _toSend.begin();
    if(!_transferPoller->PushSendMsg(ioData))
    {
        ResetPostSendMask();
        g_Log->e<FS_EpollSession>(_LOGFMT_("sessionId[%llu] socket[%d] post send fail")
                                  , _sessionId, _sock);

        return StatusDefs::SockError;
    }

    return StatusDefs::Success;
}

void FS_EpollSession::CancelRecvIoAndMaskClose(Int32 reason, bool giveRes)
{
    g_Log->i<FS_EpollSession>(_LOGFMT_("will cancel recv io sessionId[%llu] giveRes[%d]"), GetSessionId(), giveRes);
    MaskClose();
    if(!_isRecvIoCanceled)
    {
        _isRecvIoCanceled = true;
        _transferPoller->CancelRecvIo(_sessionId, reason, giveRes);
    }
}

void FS_EpollSession::CancelSendIoAndMaskClose(Int32 reason, bool giveRes /*= false*/)
{
    g_Log->i<FS_EpollSession>(_LOGFMT_("will cancel send io sessionId[%llu] giveRes[%d]"), GetSessionId(), giveRes);
    MaskClose();
    if(!_isSendIoCanceled)
    {
        _isSendIoCanceled = true;
        _transferPoller->CancelSendIo(_sessionId, reason, giveRes);
    }
}

void FS_EpollSession::OnConnect()
{
    IFS_Session::OnConnect();

    // ¹ØÁªepoll
    auto st = _epoll->AddEvent(_sock, _sessionId, __ADD_FS_EPOLL_EVENT_FLAGS_DEF__);
    if(st != StatusDefs::Success)
    {
        g_Log->e<FS_EpollSession>(_LOGFMT_("AddEvent sock[%d] sessionId[%llu] fail st[%d]"), _sock, _sessionId, st);
    }
}

void FS_EpollSession::OnSendSuc(Int64 transferBytes, EpollIoData *ioData)
{
    _isPostSend = false;
    ioData->_callback->Invoke(transferBytes);

    auto bufferNode = ioData->_node;
    auto buffer = *bufferNode->_iterNode;
    if(buffer->IsEmpty())
    {
        _toSend.erase(bufferNode->_iterNode);
        Fs_SafeFree(buffer);
    }

    g_Log->i<FS_EpollSession>(_LOGFMT_("send suc transferBytes[%lld] sessionId[%llu]"), transferBytes, GetSessionId());
}

void FS_EpollSession::OnRecvSuc(Int64 transferBytes, EpollIoData *ioData)
{
    _isPostRecv = false;
    ioData->_callback->Invoke(transferBytes);

    g_Log->i<FS_EpollSession>(_LOGFMT_("recv suc transferBytes[%lld] sessionId[%llu]"), transferBytes, GetSessionId());
}

FS_NAMESPACE_END

#endif
