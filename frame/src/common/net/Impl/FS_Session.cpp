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
 * @file  : FS_Session.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/17
 * @brief : 网络会话
 */

#include "stdafx.h"
#include "FrightenStone/common/net/Impl/FS_Session.h"
#include "FrightenStone/common/net/Defs/BufferQueueNode.h"
#include "FrightenStone/common/net/Impl/FS_Addr.h"
#include "FrightenStone/common/net/Defs/IFS_NetBuffer.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/status/status.h"

#pragma region platform ralation
#ifdef _WIN32
#include "FrightenStone/common/net/Impl/FS_Iocp.h"
#include "FrightenStone/common/net/Defs/IocpDefs.h"
#else
#include "FrightenStone/common/net/Defs/EpollDefs.h"
#include "FrightenStone/common/net/Impl/FS_Epoll.h"
#include "FrightenStone/common/net/Impl/FS_EpollTransferPoller.h"
#endif

#pragma endregion


FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(FS_Session, __DEF_OBJ_POOL_OBJ_NUM__);

FS_Session:: ~FS_Session()
{
    if (!CanDestroy())
        g_Log->e<FS_Session>(_LOGFMT_("has io uncompleted please check"));

    Close();
}

FS_String FS_Session::ToString() const
{
    FS_String info;
    info.AppendFormat("%s\n", IFS_Session::ToString().c_str())
        .AppendFormat("_isPostRecv:%d\n", _isPostRecv)
        .AppendFormat("_isPostSend:%d\n", _isPostSend)
#ifndef _WIN32
        .AppendFormat("_isRecvIoCanceled:%d\n", _isRecvIoCanceled)
        .AppendFormat("_isSendIoCanceled:%d", _isSendIoCanceled);
#else
        ;
#endif

    return info;
}

void FS_Session::Close()
{
#ifdef _WIN32
    MaskClose();
    CancelPostedEventsAndMaskClose();
#else
    if (_sock != INVALID_SOCKET && !_epoll->IsDestroy())
        _epoll->DelEvent(_sock, _sessionId);
    CancelRecvIoAndMaskClose(CancelIoReason::ForceClosed, false);
    CancelSendIoAndMaskClose(CancelIoReason::ForceClosed, false);
#endif

    IFS_Session::Close();
}

Int32 FS_Session::PostRecv()
{
    auto recvBuffer = GetRecvBuffer();
    if (recvBuffer->IsFull())
    {
        // 很少情况会让接收缓冲区爆满 因为postrecv总是在消息消费完之后抛的,能爆满说明有一个网络包的大小已经超过了缓冲大小,无法立即消费
        g_Log->w<FS_Session>(_LOGFMT_("buffer is full have create new buffer to recv data"));
        return StatusDefs::Success;
    }

    _isPostRecv = true;
    auto ioData = recvBuffer->MakeRecvIoData();
//     if (!ioData)
//     {
//         ResetPostRecvMask();
//         g_Log->w<FS_Session>(_LOGFMT_("make recv io data fail sessionId[%llu] sock[%llu]"), _sessionId, _sock);
//         return StatusDefs::Success;
//     }

#ifdef _WIN32
    Int32 st = _iocp->PostRecv(_sock, ioData);
    if (st != StatusDefs::Success)
    {
        ResetPostRecvMask();
        if (st != StatusDefs::IOCP_ClientForciblyClosed)
        {
            g_Log->e<FS_Session>(_LOGFMT_("sessionId[%llu] socket[%llu] post recv fail st[%d]")
                , _sessionId, _sock, st);
        }

        // 这个时候也有可能post刚好完成，所以不宜用closesocket，而是cacelio，使得已完成的不会被取消，且到此session即将关闭
        MaskClose();
        CancelIoEx(HANDLE(_sock), NULL);
        return st;
    }

#else
    if (!_transferPoller->PushRecvMsg(ioData))
    {
        ResetPostRecvMask();
        g_Log->e<FS_Session>(_LOGFMT_("sessionId[%llu] socket[%d] post recv fail")
            , _sessionId, _sock);

        return StatusDefs::SockError;
    }
#endif

    return StatusDefs::Success;
}

Int32 FS_Session::PostSend()
{
    auto buffer = _toSend.front();
    if (buffer->IsEmpty())
    {
        g_Log->w<FS_Session>(_LOGFMT_("make send io data fail sessionId[%llu] sock[%llu] len[%lld]")
            , _sessionId, _sock, buffer->GetLength());
        return StatusDefs::Success;
    }

    _isPostSend = true;
    auto ioData = buffer->MakeSendIoData();
//     if (!ioData)
//     {// 只有在没有数据可发送情况才为空
//         ResetPostSendMask();
//         g_Log->w<FS_Session>(_LOGFMT_("make send io data fail sessionId[%llu] sock[%llu] len[%lld]")
//             , _sessionId, _sock, buffer->GetLength());
//         return StatusDefs::Success;
//     }

    // MakeSendIoData 时候创建
//     if (!ioData->_node)
//         ioData->_node = new BufferQueueNode;

#ifdef _WIN32
    auto node = ioData->_node;
    node->_iterNode = _toSend.begin();
    Int32 st = _iocp->PostSend(_sock, ioData);
    if (st != StatusDefs::Success)
    {
        ResetPostSendMask();
        if (st != StatusDefs::IOCP_ClientForciblyClosed)
        {
            g_Log->e<FS_Session>(_LOGFMT_("sessionId[%llu] socket[%llu] post send fail st[%d]")
                , _sessionId, _sock, st);
        }

        // 这个时候也有可能post刚好完成，所以不宜用closesocket，而是cacelio，使得已完成的不会被取消，且到此session即将关闭
        MaskClose();
        CancelIoEx(HANDLE(_sock), NULL);
        return st;
    }

#else
    auto node = ioData->_node;
    node->_iterNode = _toSend.begin();
    if (!_transferPoller->PushSendMsg(ioData))
    {
        ResetPostSendMask();
        g_Log->e<FS_Session>(_LOGFMT_("sessionId[%llu] socket[%d] post send fail")
            , _sessionId, _sock);

        return StatusDefs::SockError;
    }
#endif

    return StatusDefs::Success;
}

void FS_Session::OnConnect()
{
    IFS_Session::OnConnect();

#ifdef _WIN32
    // 绑定iocp
    Int32 st = _iocp->Reg(_sock, _sessionId);
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_Session>(_LOGFMT_("iocp reg sock[%llu] sessionId[%llu] addr<%s> fail st[%d]")
            , _sock, _sessionId, _addr->ToString().c_str(), st);
    }
#else
    // 关联epoll
    auto st = _epoll->AddEvent(_sock, _sessionId, __ADD_FS_EPOLL_EVENT_FLAGS_DEF__);
    if (st != StatusDefs::Success)
    {
        g_Log->e<FS_Session>(_LOGFMT_("AddEvent sock[%d] sessionId[%llu] fail st[%d]"), _sock, _sessionId, st);
    }
#endif
}

#pragma region platform relation

#ifdef _WIN32
void FS_Session::OnSendSuc(Int64 transferBytes, IoDataBase *ioData)
#else
void FS_Session::OnSendSuc(Int64 transferBytes, EpollIoData *ioData)
#endif
{// 注意 _node 属于ioData，而ioData是buffer的数据成员，故若先释放buffer就会直接释放ioData， node就会被直接释放，所以应该先移除list中的节点再释放buffer
    _isPostSend = false;
    ioData->_callback->Invoke(transferBytes);
    auto bufferNode = ioData->_node;
    auto buffer = *bufferNode->_iterNode;
    if (buffer->IsEmpty())
    {
        if (_toSend.size() > 1)
        {// 保留至少一个缓冲区
            _toSend.erase(bufferNode->_iterNode);
            Fs_SafeFree(buffer);
        }
    }

#ifndef _WIN32
    g_Log->i<FS_Session>(_LOGFMT_("send suc transferBytes[%lld] sessionId[%llu]"), transferBytes, _sessionId);
#endif
}

#ifdef _WIN32
void FS_Session::OnRecvSuc(Int64 transferBytes, IoDataBase *ioData)
#else
void FS_Session::OnRecvSuc(Int64 transferBytes, EpollIoData *ioData)
#endif
{
    _isPostRecv = false;
    ioData->_callback->Invoke(transferBytes);

#ifndef _WIN32
    g_Log->i<FS_Session>(_LOGFMT_("recv suc transferBytes[%lld] sessionId[%llu]"), transferBytes, _sessionId);
#endif
}

#ifdef _WIN32
void FS_Session::CancelPostedEventsAndMaskClose()
{
    // 不用closesocket，而是cacelio，使得已完成的不会被取消，且到此session即将关闭
    MaskClose();
    CancelIoEx(HANDLE(_sock), NULL);
}

#else
void FS_Session::Bind(FS_EpollPoller *transferPoller)
{
    _transferPoller = transferPoller;
    _epoll = _transferPoller->GetEpollObj();
}

void FS_Session::CancelRecvIoAndMaskClose(Int32 reason, bool giveRes /* = false */)
{
    g_Log->i<FS_Session>(_LOGFMT_("will cancel recv io sessionId[%llu] giveRes[%d]"), GetSessionId(), giveRes);
    MaskClose();
    if (!_isRecvIoCanceled)
    {
        _isRecvIoCanceled = true;
        _transferPoller->CancelRecvIo(_sessionId, reason, giveRes);
    }
}

void FS_Session::CancelSendIoAndMaskClose(Int32 reason, bool giveRes /*= false*/)
{
    g_Log->i<FS_Session>(_LOGFMT_("will cancel send io sessionId[%llu] giveRes[%d]"), GetSessionId(), giveRes);
    MaskClose();
    if (!_isSendIoCanceled)
    {
        _isSendIoCanceled = true;
        _transferPoller->CancelSendIo(_sessionId, reason, giveRes);
    }
}
#endif
#pragma endregion
FS_NAMESPACE_END
