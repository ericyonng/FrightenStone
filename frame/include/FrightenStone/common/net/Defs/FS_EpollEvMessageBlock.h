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
 * @file  : FS_EpollEvMessageBlock.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/02/05
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_FS_EpollEvMessageBlock_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_FS_EpollEvMessageBlock_H__

#pragma once

#ifndef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/MessageQueue/MessageQueue.h"
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/net/Defs/EpollDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/component/Impl/FS_String.h"

#pragma region linux
#include <unistd.h>
#include <sys/epoll.h>
#pragma endregion

FS_NAMESPACE_BEGIN

class IFS_NetModule;
class FS_EpollPoller;

class BASE_EXPORT FS_EpollEvMessageType
{
public:
    enum
    {
        None = 0,                       // 无效
        EpollEvArriveTriggers = 1,      // 初步事件触发

        EpollInTrigger = 2,             // epoll可读事件
        EpollOutTrigger = 3,            // 可写事件

        PostRecv = 4,                   // 投递接收数据 dispatcher -> transfer
        PostSend = 5,                   // 投递发送数据 dispatcher -> transfer

        RecvDataEv = 6,                 // 收到数据 transfer -> dispatcher
        SendSucEv = 7,                  // 发送成功事件 transfer -> dispatcher

        WillConnect = 8,                // 连接 transfer -> dispatcher
        // Disconnect = 9,                 // 断开连接 transfer -> dispatcher
        ConnectorConnectOpFinish = 9,  // 连接器连接成功 transfer -> dispatcher

        // CancelIo = 11,                  // 取消io transfer->dispatcher 在transfer recver与sender数据都移除后方可发送

        PostCancelIoToRecver = 10,      // 向transfer的 recver抛取消会话io消息
        PostCancelIoToSender = 11,      // 向transfer的 sender抛取消会话io的消息
        End,
    };
};

struct BASE_EXPORT FS_EpollEvMessageBlock : public FS_MessageBlock
{
    OBJ_POOL_CREATE_DEF(FS_EpollEvMessageBlock);

    FS_EpollEvMessageBlock();
    virtual ~FS_EpollEvMessageBlock();

    Int32 _messageType;

    UInt32 _compId;                     // 组件id 一般是transfercompId(消息产生者)
    UInt32 _generatorId;                // 生产者id
};

struct BASE_EXPORT EpollEvArriveMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvArriveMessageBlock);

    EpollEvArriveMessageBlock();
    ~EpollEvArriveMessageBlock() {}

    epoll_event _ev[__FS_MAX_EVENT_NUM_TO_HANDLE_ONCE_DEF__];   // 触发的事件
    Int32 _triggerAmount;                                       // 事件触发的最大数量
};

// 会话消息连入
struct BASE_EXPORT EpollEvSessionWillConnectMsg : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvSessionWillConnectMsg);

    EpollEvSessionWillConnectMsg();
    ~EpollEvSessionWillConnectMsg();

    FS_String ToString();

    UInt32 _acceptorCompId;             // 监听compId
    UInt64 _sessionId;
    SOCKET _sock;
    sockaddr_in _addrInfo;

    FS_EpollPoller *_poller;

    /* 连接器的额外参数 */
    IDelegate<void, UInt64> *_onUserDisconnectedRes;    // UInt64 :sessionId
};

struct BASE_EXPORT EpollEvEpollInEvMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvEpollInEvMessageBlock);
    EpollEvEpollInEvMessageBlock();
    ~EpollEvEpollInEvMessageBlock();

    UInt64 _sessionId;
};

struct BASE_EXPORT EpollEvEpollOutEvMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollEvEpollOutEvMessageBlock);
    EpollEvEpollOutEvMessageBlock();
    ~EpollEvEpollOutEvMessageBlock() {}

    UInt64 _sessionId;
};

struct BASE_EXPORT EpollRecvDataMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollRecvDataMessageBlock);

    EpollRecvDataMessageBlock();

    UInt64 _sessionId;
    Int64 _transferBytes;
    EpollIoData *_ioData;
    bool _isSessionInRecverMonitor;
};

struct BASE_EXPORT EpollSendSucMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(EpollSendSucMessageBlock);

    EpollSendSucMessageBlock();

    UInt64 _sessionId;
    Int64 _transferBytes;
    EpollIoData *_ioData;
    bool _isSessionInSenderMonitor;
};

struct BASE_EXPORT PostRecvMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(PostRecvMessageBlock);

    PostRecvMessageBlock();

    UInt64 _sessionId;
    EpollIoData *_ioData;
};

struct BASE_EXPORT PostSendMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(PostSendMessageBlock);

    PostSendMessageBlock();

    UInt64 _sessionId;
    EpollIoData *_ioData;
};

struct BASE_EXPORT PostCancelIoToRecverMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(PostCancelIoToRecverMessageBlock);

    PostCancelIoToRecverMessageBlock(UInt64 sessionId, Int32 reason, bool giveRes);

    UInt64 _sessionId;
    Int32 _reason;                  // 断开原因
    bool _giveRes;                  // 反馈消息
};

struct BASE_EXPORT PostCancelIoToSenderMessageBlock : public FS_EpollEvMessageBlock
{
    OBJ_POOL_CREATE_DEF(PostCancelIoToSenderMessageBlock);

    PostCancelIoToSenderMessageBlock(UInt64 sessionId, Int32 reason, bool giveRes);

    UInt64 _sessionId;
    Int32 _reason;                  // 断开原因
    bool _giveRes;                  // 反馈消息
};
FS_NAMESPACE_END

#include "FrightenStone/common/net/Defs/FS_EpollEvMessageBlockImpl.h"

#endif

#endif
