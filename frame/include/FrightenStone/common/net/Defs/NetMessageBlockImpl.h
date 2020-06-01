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
 * @file  : NetMessageBlockImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/04
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Defs_NetMessageBlock_H__

#pragma once

FS_NAMESPACE_BEGIN
inline FS_NetMsgBlock::FS_NetMsgBlock(UInt32 compId, UInt32 generatorId, Int32 messageType)
    :_compId(compId)
    , _generatorId(generatorId)
    , _messageType(messageType)
{

}

inline FS_NetMsgBlock::~FS_NetMsgBlock()
{

}

inline SessionConnectedNetMsgBlock::SessionConnectedNetMsgBlock(UInt32 compId, UInt32 generatorId)
    :FS_NetMsgBlock(compId, generatorId, NetMessageType::SessionConnected)
    ,_acceptorCompId(0)
    , _connectorCompId(0)
    , _protocolPort(0)
    , _srcCompType(0)
    , _sessionId(0)
    , _sock(INVALID_SOCKET)
    , _addrInfo{0}
#ifdef _WIN32
    , _iocp(NULL)
#else
    , _poller(NULL)
#endif
    , _onUserDisconnectedRes(NULL)
    , _stub(0)
{
}

inline SessionConnectedNetMsgBlock::~SessionConnectedNetMsgBlock()
{
    Fs_SafeFree(_onUserDisconnectedRes);
}

inline PostConnectionFailureToDispatcherMsgBlock::PostConnectionFailureToDispatcherMsgBlock(const FS_ConnectInfo &connectInfo, Int32 resultCode)
    :_connectInfo(connectInfo)
    , _resultCode(resultCode)
{
    _messageType = NetMessageType::PostConnectionFailureToDispatcher;
}

inline PostAsynConnectionToConnectorMsgBlock::PostAsynConnectionToConnectorMsgBlock(const FS_ConnectInfo &connectInfo)
    :_connectInfo(connectInfo)
    , _dispatcherMsgQueueConsumerId(0)
{
    _messageType = NetMessageType::PostAsynConnectionToConnector;
}

inline SessionDisconnectedMsgBlock::SessionDisconnectedMsgBlock(UInt64 sessionId)
    :_sessionId(sessionId)
{
    _messageType = NetMessageType::SessionDisconnected;
}

#ifdef _WIN32
inline IocpMsgArrivedMsgBlock::IocpMsgArrivedMsgBlock()
    :_ioEv(NULL)
    , _errorCode(StatusDefs::Success)
{
    _messageType = NetMessageType::IocpMsgArrived;
}

inline IocpMsgArrivedMsgBlock::~IocpMsgArrivedMsgBlock()
{
    Fs_SafeFree(_ioEv);
}

#else

inline EpollEvArriveMsgBlock::EpollEvArriveMsgBlock()
    :_triggerAmount(0)
{
    _messageType = NetMessageType::EpollEvArriveTriggers;
}

inline EpollEvEpollInEvMsgBlock::EpollEvEpollInEvMsgBlock()
    : _sessionId(0)
{
    _messageType = NetMessageType::EpollInTrigger;
}

inline EpollEvEpollOutEvMsgBlock::EpollEvEpollOutEvMsgBlock()
    : _sessionId(0)
{
    _messageType = NetMessageType::EpollOutTrigger;
}

inline PostRecvMsgBlock::PostRecvMsgBlock()
    : _sessionId(0)
    , _ioData(NULL)
{
    _messageType = NetMessageType::PostRecv;
}

inline PostSendMsgBlock::PostSendMsgBlock()
    : _sessionId(0)
    , _ioData(NULL)
{
    _messageType = NetMessageType::PostSend;
}

inline EpollRecvDataMsgBlock::EpollRecvDataMsgBlock(UInt32 compId, UInt32 generatorId)
    : FS_NetMsgBlock(compId, generatorId)
    , _sessionId(0)
    , _transferBytes(0)
    , _ioData(NULL)
    , _isSessionInRecverMonitor(true)
{
    _messageType = NetMessageType::RecvDataEv;
}

inline EpollSendSucMsgBlock::EpollSendSucMsgBlock(UInt32 compId, UInt32 generatorId)
    :FS_NetMsgBlock(compId, generatorId)
    ,_sessionId(0)
    , _transferBytes(0)
    ,_ioData(NULL)
    ,_isSessionInSenderMonitor(true)
{
    _messageType = NetMessageType::SendSucEv;
}

inline PostCancelIoToRecverMsgBlock::PostCancelIoToRecverMsgBlock(UInt64 sessionId, Int32 reason, bool giveRes)
    :_sessionId(sessionId)
    ,_reason(reason)
    , _giveRes(giveRes)
{
    _messageType = NetMessageType::PostCancelIoToRecver;
}

inline PostCancelIoToSenderMsgBlock::PostCancelIoToSenderMsgBlock(UInt64 sessionId, Int32 reason, bool giveRes)
    :_sessionId(sessionId)
    , _reason(reason)
    , _giveRes(giveRes)
{
    _messageType = NetMessageType::PostCancelIoToSender;
}

#endif

FS_NAMESPACE_END

#endif
