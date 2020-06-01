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
 * @file  : NetMessageBlock.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/04
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_NetMessageBlock_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_NetMessageBlock_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/net/Defs/FS_ConnectInfo.h"
#include "FrightenStone/common/net/Defs/FS_NetDefs.h"
#include "FrightenStone/common/net/Defs/IoEvDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Defs/EpollDefs.h"

#ifdef _WIN32
class IFS_NetModule;

#else
#pragma region linux
#include <unistd.h>
#include <sys/epoll.h>
#pragma endregion

class IFS_BasePoller;
#endif

FS_NAMESPACE_BEGIN

// 枚举可能会动态变化不可持久化
class BASE_EXPORT NetMessageType
{
public:
    enum 
    {
        None = 0,                       // 无效

        SessionConnected = 1,                   // 连接 transfer -> dispatcher
        PostConnectionFailureToDispatcher = 2,  // 向dispatcher 抛连接失败的消息（带回执stub的）
        PostAsynConnectionToConnector = 3,      // 向connector抛连接消息

        #pragma region iocp
        IocpMsgArrived,                 // 收到网络包 Transfer to Dispatcher
        #pragma endregion

        #pragma region Epoll
        EpollEvArriveTriggers,      // 初步事件触发

        EpollInTrigger,             // epoll可读事件
        EpollOutTrigger,            // 可写事件

        PostRecv,                   // 投递接收数据 dispatcher -> transfer
        PostSend,                   // 投递发送数据 dispatcher -> transfer

        RecvDataEv,                 // 收到数据 transfer -> dispatcher
        SendSucEv,                  // 发送成功事件 transfer -> dispatcher

        PostCancelIoToRecver,      // 向transfer的 recver抛取消会话io消息
        PostCancelIoToSender,      // 向transfer的 sender抛取消会话io的消息
        #pragma endregion

        SessionDisconnected,        // 连接断开

        End,
    };
};


struct BASE_EXPORT FS_NetMsgBlock : public FS_MessageBlock
{
    OBJ_POOL_CREATE_DEF(FS_NetMsgBlock);

    FS_NetMsgBlock(UInt32 compId = 0, UInt32 generatorId = 0, Int32 messageType = NetMessageType::None);
    ~FS_NetMsgBlock();

    UInt32 _compId;                     // 组件id 一般是transfercompId(消息产生者)
    UInt32 _generatorId;                // 生产者id
    Int32 _messageType;                 // NetMessageType
};

// 会话消息连入
struct BASE_EXPORT SessionConnectedNetMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(SessionConnectedNetMsgBlock);

    SessionConnectedNetMsgBlock(UInt32 compId, UInt32 generatorId);
    ~SessionConnectedNetMsgBlock();

    FS_String ToString();

    UInt32 _acceptorCompId;             // 监听compId
    UInt32 _connectorCompId;
    UInt16 _protocolPort;
    Int32 _srcCompType;
    UInt64 _sessionId;
    SOCKET _sock;
    sockaddr_in _addrInfo;

    // TODO:
    #ifdef _WIN32
    IFS_NetModule *_iocp;
    #else
    IFS_BasePoller *_poller;                // transfer poller
    #endif

    /* 连接器的额外参数 */
    IDelegate<void, UInt64> *_onUserDisconnectedRes;    // UInt64 :sessionId
    UInt64 _stub;   // 存根
};

// 连接失败
struct BASE_EXPORT PostConnectionFailureToDispatcherMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostConnectionFailureToDispatcherMsgBlock);

    PostConnectionFailureToDispatcherMsgBlock(const FS_ConnectInfo &connectInfo, Int32 resultCode);

    FS_ConnectInfo _connectInfo;
    Int32 _resultCode;
};

// 异步连接返回
struct BASE_EXPORT PostAsynConnectionToConnectorMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostAsynConnectionToConnectorMsgBlock);

    PostAsynConnectionToConnectorMsgBlock(const FS_ConnectInfo &connectInfo);

    FS_ConnectInfo _connectInfo;
    UInt32 _dispatcherMsgQueueConsumerId;   // dispatcher作为消息队列的消费者
};

// 连接断开
struct BASE_EXPORT SessionDisconnectedMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(SessionDisconnectedMsgBlock);

    SessionDisconnectedMsgBlock(UInt64 sessionId);
    UInt64 _sessionId;
};

#ifdef _WIN32

// 会话消息到达
struct BASE_EXPORT IocpMsgArrivedMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(IocpMsgArrivedMsgBlock);
    IocpMsgArrivedMsgBlock();
    ~IocpMsgArrivedMsgBlock();

    IoEvent *_ioEv;                     // 对象池创建
    Int32 _errorCode;                   // 网络错误码
};

#else

struct BASE_EXPORT EpollEvArriveMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvArriveMsgBlock);

    EpollEvArriveMsgBlock();
    ~EpollEvArriveMsgBlock() {}

    epoll_event _ev[__FS_MAX_EVENT_NUM_TO_HANDLE_ONCE_DEF__];   // 触发的事件
    Int32 _triggerAmount;                                       // 事件触发的最大数量
};

struct BASE_EXPORT EpollEvEpollInEvMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvEpollInEvMsgBlock);
    EpollEvEpollInEvMsgBlock();
    ~EpollEvEpollInEvMsgBlock() {}

    UInt64 _sessionId;
};

struct BASE_EXPORT EpollEvEpollOutEvMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvEpollOutEvMsgBlock);
    EpollEvEpollOutEvMsgBlock();
    ~EpollEvEpollOutEvMsgBlock() {}

    UInt64 _sessionId;
};

struct BASE_EXPORT PostRecvMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostRecvMsgBlock);

    PostRecvMsgBlock();

    UInt64 _sessionId;
    EpollIoData *_ioData;
};

struct BASE_EXPORT PostSendMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostSendMsgBlock);

    PostSendMsgBlock();

    UInt64 _sessionId;
    EpollIoData *_ioData;
};

struct BASE_EXPORT EpollRecvDataMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(EpollRecvDataMsgBlock);

    EpollRecvDataMsgBlock(UInt32 compId, UInt32 generatorId);

    UInt64 _sessionId;
    Int64 _transferBytes;
    EpollIoData *_ioData;
    bool _isSessionInRecverMonitor;
};

struct BASE_EXPORT EpollSendSucMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(EpollSendSucMsgBlock);

    EpollSendSucMsgBlock(UInt32 compId, UInt32 generatorId);

    UInt64 _sessionId;
    Int64 _transferBytes;
    EpollIoData *_ioData;
    bool _isSessionInSenderMonitor;
};


struct BASE_EXPORT PostCancelIoToRecverMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostCancelIoToRecverMsgBlock);

    PostCancelIoToRecverMsgBlock(UInt64 sessionId, Int32 reason, bool giveRes);

    UInt64 _sessionId;
    Int32 _reason;                  // 断开原因
    bool _giveRes;                  // 反馈消息
};

struct BASE_EXPORT PostCancelIoToSenderMsgBlock : public FS_NetMsgBlock
{
    OBJ_POOL_CREATE_DEF(PostCancelIoToSenderMsgBlock);

    PostCancelIoToSenderMsgBlock(UInt64 sessionId, Int32 reason, bool giveRes);

    UInt64 _sessionId;
    Int32 _reason;                  // 断开原因
    bool _giveRes;                  // 反馈消息
};
#endif

FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/NetMessageBlockImpl.h"

#endif
