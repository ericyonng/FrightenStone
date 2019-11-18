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
 * @file  : FS_IocpMsgTransfer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_IocpMsgTransfer_H__
#define __Base_Common_Net_Impl_FS_IocpMsgTransfer_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/IFS_MsgTransfer.h"
#include "base/common/asyn/asyn.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/net/Impl/IFS_Session.h"
#include "base/common/net/Impl/FS_IocpSession.h"
#include "base/common/net/Defs/HeartBeatComp.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT FS_Iocp;
struct BASE_EXPORT IO_EVENT;
class BASE_EXPORT FS_IocpSession;
struct BASE_EXPORT NetMsg_DataHeader;
struct BASE_EXPORT FS_MessageBlock;
struct BASE_EXPORT BriefSessionInfo;

class BASE_EXPORT FS_IocpMsgTransfer : public IFS_MsgTransfer
{

public:
    FS_IocpMsgTransfer(Int32 id);
    virtual ~FS_IocpMsgTransfer();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

    // 网络
    virtual void OnConnect(const BriefSessionInfo  &sessionInfo);
    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut(IFS_Session *session);

    virtual Int32 GetSessionCnt();

    // 消息队列
    virtual void AttachMsgQueue(ConcurrentMessageQueue *messageQueue, Int32 generatorId);
    virtual void AttachSenderMsgQueue(MessageQueue *messageQueue);

private:
    void _OnMoniterMsg(FS_ThreadPool *pool);
    Int32 _DoEvents();
    Int32 _HandleNetEvent();
    void _OnMsgArrived();
    void _OnMsgArrived(FS_IocpSession *session);
    void _RemoveSessions(bool forceDisconnect = false);

    // 网络事件 线程不安全
private:
    void _OnGracefullyDisconnect(FS_IocpSession *session);
    void _OnDelayDisconnected(FS_IocpSession *session);
    // 需要判断是否可断开
    void _OnDisconnected(FS_IocpSession *session);
    void _DestroySession(FS_IocpSession *session);
    bool _DoPostSend(FS_IocpSession *session);
    bool _DoPostRecv(FS_IocpSession *session);

private:
    void _NtySessionConnectedMsg(UInt64 sessionId);
    void _NtySessionDisConnectMsg(UInt64 sessionId);

    // 辅助
private:
    FS_IocpSession *_GetSession(UInt64 sessionId);
    void _ClearSessionsWhenClose();

    void _UpdateSessionHeartbeat(IFS_Session *session); // 线程不安全
    void _CheckSessionHeartbeat();  // 线程不安全
    void _PostEventsToIocp();
    void _AsynSendFromDispatcher();
    void _ClearSenderMessageQueue();
    void _FreeSendList(std::list<NetMsg_DataHeader *> *sendQueue);
    void _LinkCacheToSessions();

    void _UpdateCanCreateNewNodeForAlloctor(size_t addOccupiedBytes);
    void _PrintAlloctorOccupiedInfo();

private:
    Int32 _id;
    ULong _transferThreadId;
    IMemoryAlloctor *_sessionBufferAlloctor;
    UInt64 _maxAlloctorBytes;
    std::atomic<size_t> _curAlloctorOccupiedBytes;
    std::atomic_bool _canCreateNewNodeForAlloctor;
    IDelegate<void, size_t> *_updateAlloctorOccupied;
    IDelegate<void> *_printAlloctorOccupiedInfo;

    std::atomic<Int32> _sessionCnt;             // 会话个数
    std::map<UInt64, FS_IocpSession *> _sessions;  // key:sessionId
    Time _curTime;
    std::set<IFS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    FS_ThreadPool *_threadPool;
    FS_Iocp *_iocp;
    IO_EVENT *_ioEvent;
    ConcurrentMessageQueue *_messageQueue;
    std::list<FS_MessageBlock *> *_senderMsgs;
    std::list<FS_MessageBlock *> *_recvMsgList;
    MessageQueue *_senderMessageQueue;
    Int32 _generatorId;

    // 缓冲区
    Locker _connectorGuard;
    std::atomic<bool> _hasNewSessionLinkin;         // 
    std::list<BriefSessionInfo> _pendingNewSessionInfos;     // 连入的会话缓冲区

    // 待发送的会话缓冲区
//     Locker _asynSendGuard;
//     std::atomic<bool> _isSendCacheDirtied;
//     std::map<UInt64, std::list<NetMsg_DataHeader *> *> _asynSendMsgQueueCache; // key:sessionId
//     std::map<UInt64, std::list<NetMsg_DataHeader *> *> _asynSendMsgQueue;  // key:sessionId

    std::list<FS_IocpSession *> _msgArriviedSessions;  // 消息到达的会话
    std::set<FS_IocpSession *> _toPostRecv;
    std::set<FS_IocpSession *> _toPostSend;
    std::set<FS_IocpSession *> _toRemove;

//     _CrtMemState s1;
//     _CrtMemState s2;
//     _CrtMemState s3;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpMsgTransferImpl.h"

#endif
