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
 * @file  : FS_EpollEvMessageBlockImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/06
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Defs_FS_EpollEvMessageBlock_H__
#pragma once

#ifndef _WIN32

FS_NAMESPACE_BEGIN
inline FS_EpollEvMessageBlock::FS_EpollEvMessageBlock()
    :_messageType(FS_EpollEvMessageType::None)
    , _compId(0)
    , _generatorId(0)
{
}

inline FS_EpollEvMessageBlock::~FS_EpollEvMessageBlock()
{
}

inline EpollEvArriveMessageBlock::EpollEvArriveMessageBlock()
    :_triggerAmount(0)
{
    _messageType = FS_EpollEvMessageType::EpollEvArriveTriggers;
}

inline EpollEvEpollInEvMessageBlock::EpollEvEpollInEvMessageBlock()
    : _sessionId(0)
{
    _messageType = FS_EpollEvMessageType::EpollInTrigger;
}

inline EpollEvEpollInEvMessageBlock::~EpollEvEpollInEvMessageBlock()
{
}

inline EpollEvEpollOutEvMessageBlock::EpollEvEpollOutEvMessageBlock()
    : _sessionId(0)
{
    _messageType = FS_EpollEvMessageType::EpollOutTrigger;
}

inline EpollEvSessionWillConnectMsg::EpollEvSessionWillConnectMsg()
    :_acceptorCompId(0)
    , _sessionId(0)
    , _sock(INVALID_SOCKET)
    , _addrInfo{0}
    , _poller(NULL)
    , _onUserDisconnectedRes(NULL)
{
    _messageType = FS_EpollEvMessageType::WillConnect;
}

inline EpollEvSessionWillConnectMsg::~EpollEvSessionWillConnectMsg()
{
    Fs_SafeFree(_onUserDisconnectedRes);
}

inline EpollRecvDataMessageBlock::EpollRecvDataMessageBlock()
    : _sessionId(0)
    , _transferBytes(0)
    , _ioData(NULL)
    ,_isSessionInRecverMonitor(true)
{
    _messageType = FS_EpollEvMessageType::RecvDataEv;
}

inline EpollSendSucMessageBlock::EpollSendSucMessageBlock()
    :_sessionId(0)
    , _transferBytes(0)
    , _ioData(NULL)
    ,_isSessionInSenderMonitor(true)
{
    _messageType = FS_EpollEvMessageType::SendSucEv;
}

inline PostRecvMessageBlock::PostRecvMessageBlock()
    :_sessionId(0)
    , _ioData(NULL)
{
    _messageType = FS_EpollEvMessageType::PostRecv;
}

inline PostSendMessageBlock::PostSendMessageBlock()
    :_sessionId(0)
    , _ioData(NULL)
{
    _messageType = FS_EpollEvMessageType::PostSend;
}
inline PostCancelIoToRecverMessageBlock::PostCancelIoToRecverMessageBlock(UInt64 sessionId, Int32 reason, bool giveRes)
    : _sessionId(sessionId)
    , _reason(reason)
    ,_giveRes(giveRes)
{
    _messageType = FS_EpollEvMessageType::PostCancelIoToRecver;
}

inline PostCancelIoToSenderMessageBlock::PostCancelIoToSenderMessageBlock(UInt64 sessionId, Int32 reason, bool giveRes)
    :_sessionId(sessionId)
    ,_reason(reason)
    ,_giveRes(giveRes)
{
    _messageType = FS_EpollEvMessageType::PostCancelIoToSender;
}
FS_NAMESPACE_END

#endif

#endif
