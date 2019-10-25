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
#include "base/common/net/Defs/HeartBeatComp.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT FS_Iocp;
struct BASE_EXPORT IO_EVENT;
class BASE_EXPORT FS_IocpSession;

class BASE_EXPORT FS_IocpMsgTransfer : public IFS_MsgTransfer
{
public:
    FS_IocpMsgTransfer();
    virtual ~FS_IocpMsgTransfer();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

    // 网络
    virtual void OnConnect(IFS_Session *session);
    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut(IFS_Session *session);
    // msg内存池创建 其他线程调用本接口，send需要加锁
    virtual void AsynSend(UInt64 sessionId, NetMsg_DataHeader *msg);

    virtual void RegisterDisconnected(IDelegate<void, IFS_Session *> *callback);
    virtual void RegisterRecvSucCallback(IDelegate<void, IFS_Session *, Int64> *callback);
    virtual void RegisterSendSucCallback(IDelegate<void, IFS_Session *, Int64> *callback);
    virtual void RegisterHeatBeatTimeOutCallback(IDelegate<void, IFS_Session *> *callback);
    virtual Int32 GetSessionCnt();

private:
    void _OnMoniterMsg(const FS_ThreadPool *pool);
    void _HandleNetEvent(std::set<UInt64> &sessionIdsToRemove, std::set<UInt64> &toPostRecv, std::set<UInt64> &toPostSend);
    void _RemoveSessions(const std::set<UInt64> &sessionIds);
    void _OnHeartbeatTimeOut(const std::set<UInt64> &timeoutSessionIds, std::set<UInt64> &leftSessionIdsToRemove);

    // 网络事件 线程不安全
private:
    void _OnGracefullyDisconnect(IFS_Session *session);
    void _OnDelayDisconnected(IFS_Session *session);
    // 需要判断是否可断开
    void _OnDisconnected(IFS_Session *session);
    bool _DoPostSend(FS_IocpSession *session, bool removeIfFail = true);
    bool _DoPostRecv(FS_IocpSession *session, bool removeIfFail = true);

    // 辅助
private:
    IFS_Session *_GetSession(UInt64 sessionId);
    void _ClearSessionsWhenClose();

    void _UpdateSessionHeartbeat(IFS_Session *session); // 线程不安全
    void _CheckSessionHeartbeat(std::set<IFS_Session *> &sessionsToRemove);  // 线程不安全

    void _PostSessions();
    void _FreeSendList(std::list<NetMsg_DataHeader *> *sendQueue);
    void _LinkCacheToSessions();

private:
    std::atomic<Int32> _sessionCnt;             // 会话个数
    std::map<UInt64, IFS_Session *> _sessions;  // key:sessionId
    Time _curTime;
    std::set<IFS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    FS_ThreadPool *_threadPool;
    FS_Iocp *_iocp;
    IO_EVENT *_ioEvent;

    // 缓冲区
    Locker _connectorGuard;
    std::atomic<bool> _hasNewSessionLinkin;     // 
    std::list<IFS_Session *> _linkSessionCache; // 连入的会话缓冲区
    // 待发送的会话缓冲区
    Locker _asynSendGuard;
    std::atomic<bool> _isSendCacheDirtied;
    std::map<UInt64, std::list<NetMsg_DataHeader *> *> _asynSendQueueCache; // key:sessionId
    std::map<UInt64, std::list<NetMsg_DataHeader *> *> _asynSendQueue;  // key:sessionId
    

    IDelegate<void, IFS_Session *> *_serverCoreDisconnectedCallback;
    IDelegate<void, IFS_Session *, Int64> *_serverCoreRecvSucCallback;
    IDelegate<void, IFS_Session *, Int64> *_serverCoreSendSucCallback;
    IDelegate<void, IFS_Session *> *_serverCoreHeartBeatTimeOutCallback;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpMsgTransferImpl.h"

#endif
