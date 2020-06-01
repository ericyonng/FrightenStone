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
 * @file  : FS_EpollTransferPoller.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/05/05
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollTransferPoller_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollTransferPoller_H__

#pragma once

#ifndef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_BasePoller.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"
#include "FrightenStone/common/net/Defs/PollerDefs.h"

FS_NAMESPACE_BEGIN

class FS_Epoll;
class FS_ThreadPool;
class MessageQueueNoThread;
struct EpollIoData;
struct FS_NetMsgBlock;
class FS_String;

class BASE_EXPORT FS_EpollTransferPoller : public IFS_BasePoller
{
public:
    FS_EpollTransferPoller(IFS_EngineComp *engineComp);
    ~FS_EpollTransferPoller();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();

    // 网络模型iocp,epoll
    virtual IFS_NetModule *GetNetModuleObj();
    FS_Epoll *GetEpollObj();

public:
    bool PushRecvMsg(EpollIoData *ioData);
    bool PushSendMsg(EpollIoData *ioData);
    void CancelRecvIo(UInt64 sessionId, Int32 reason, bool giveRes);
    void CancelSendIo(UInt64 sessionId, Int32 reason, bool giveRes);
    virtual bool OnWillConnect(SOCKET sock, UInt64 sessionId);
    bool IsIoFinish() const;

private:
    // 处理数据传输
    void _OnTransferEvMonitorPoller(FS_ThreadPool *pool);
    void _OnPreDistributeEvPoller(FS_ThreadPool *pool);
    void _OnReadPoller(FS_ThreadPool *pool);
    void _OnSendPoller(FS_ThreadPool *pool);

    // 数据传输的消息处理
    void _OnEpollReadableEv(FS_NetMsgBlock *messageBlock);
    void _OnEpollWritableEv(FS_NetMsgBlock *messageBlock);
    void _OnBePostedRecv(FS_NetMsgBlock *messageBlock);
    void _OnBePostedSend(FS_NetMsgBlock *messageBlock);
    void _OnBePostedCancleIoToRecver(FS_NetMsgBlock *messageBlock);
    void _OnBePostedCancleIoToSender(FS_NetMsgBlock *messageBlock);

    // 收发
    Int32 _OnRecv(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode);
    Int32 _OnSend(SOCKET sock, Byte8 *buff, Int64 buffLen, Int32 &sockErrorCode);

    // 网络事件处理
    void _OnRecvRemoveSession(SOCKET sock, UInt64 sessionId);
    void _OnSendRemoveSession(SOCKET sock, UInt64 sessionId);
    void _PostCancelToRecver(UInt64 sessionId, Int32 reason, bool giveRes);
    void _PostCancelToSender(UInt64 sessionId, Int32 reason, bool giveRes);
    void _PostRecvMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData);
    void _PostSendSucMsgToDispatcher(UInt64 sessionId, Int64 transferBytes, EpollIoData *ioData);

    void _AddCount(const FS_String &name);
    void _InitMsgBlockHandlers();

private:
    FS_ThreadPool *_pool;
    FS_Epoll *_epoll;
    MessageQueueNoThread *_innerEvThread2MsgPreHandleMq;      // 监听事件线程消息到消息预处理线程
    ConditionLocker _preHandlerGuard;
    std::atomic_bool _isInit;
    std::atomic_bool _isWillClose;
    UInt32 _generatorId;

    // 消息处理 注意recvermailbox，与sendermailbox处理的事件必须要互斥避免出现资源竞争！！！
    typedef void (FS_EpollTransferPoller::*MessageBlockHandler)(FS_NetMsgBlock *messageBlock);
    static MessageBlockHandler _msgBlockHandler[NetMessageType::End];

    // 接受数据
    mutable Locker _recvLock;
    std::map<UInt64, bool> _sessionIdRefRecverEvStatus;             // 是否是触发状态
    std::map<UInt64, EpollIoData *> _sessionIdRefRecverEvIoData;    // 关联的数据
    MessageQueueNoThread *_recverMailBox;                           // 接收消息的邮箱

    // 发送数据
    mutable Locker _sendLock;
    std::map<UInt64, bool> _sessionIdRefSenderEvStatus;             // 是否是触发状态
    std::map<UInt64, EpollIoData *> _sessionIdRefSenderIoData;      // 关联的数据
    MessageQueueNoThread *_senderMailBox;                           // 发送消息的邮箱
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_EpollTransferPollerImpl.h"

#endif

#endif
