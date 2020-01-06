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
 * @file  : FS_IocpMsgDispatcher.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpMsgDispatcher_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpMsgDispatcher_H__
#pragma once

#ifdef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_MsgDispatcher.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include <set>
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include "FrightenStone/common/net/Defs/HeartBeatComp.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class IFS_Session;
struct NetMsg_DataHeader;
class IFS_MsgTransfer;
class IFS_BusinessLogic;
class ConcurrentMessageQueueNoThread;
struct FS_MessageBlock;
class IUser;
class IFS_NetEngine;
class FS_IocpSession;

class BASE_EXPORT FS_IocpMsgDispatcher : public IFS_MsgDispatcher
{
public:
    FS_IocpMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId);
    virtual ~FS_IocpMsgDispatcher();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &cfgs);
    virtual Int32 Start();
    virtual void AfterStart() {}
    virtual void WillClose() {}
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose() {}


private:
    void _OnBusinessProcessThread(FS_ThreadPool *pool);

    void _CheckHeartbeat();
    void _OnBusinessProcessing();
    void _PostEvents();
    void _DelayRemoveSessions();

    bool _DoPostRecv(FS_IocpSession *session);
    bool _DoPostSend(FS_IocpSession *session);

    /////////////////////////////////////////////////////////////////////////旧的接口
    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut();

    virtual void SendData(UInt64 sessionId,  UInt64 consumerId, NetMsg_DataHeader *msg);// 线程安全
    virtual void BindBusinessLogic(IFS_BusinessLogic *businessLogic);
    virtual void AttachRecvMessageQueue(ConcurrentMessageQueueNoThread *messageQueue);
    virtual void CloseSession(UInt64 sessionId, UInt64 consumerId);

private:
    // msgData会拷贝到内存池创建的缓冲区中 线程不安全，外部需要加锁
    void _DoBusinessProcess(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData);
    void _OnDelaySessionDisconnect(UInt64 sessionId);

private:
    DispatcherCfgs * _cfgs;
    std::atomic<bool> _isClose;
    FS_ThreadPool *_pool;

    // 数据在线程结束时清理
    std::set<UInt64> _delayDisconnectedSessions;

    // 业务层资源
    TimeWheel *_timeWheel;
    IFS_BusinessLogic *_logic;
    std::map<UInt64, std::list<IDelegate<void, IUser *> *>> _sessionIdRefUserDisconnected;

    std::vector<std::list<FS_MessageBlock *> *> *_recvMsgBlocks;       // 需要转换成 FS_NetMsgBufferBlock

    /* 会话 */ 
    IMemoryAlloctor *_sessionBufferAlloctor;
    std::atomic<size_t> _curAlloctorOccupiedBytes;
    std::atomic_bool _canCreateNewNodeForAlloctor;
    IDelegate<void> *_printAlloctorOccupiedInfo;

    std::map<UInt64, FS_IocpSession *> _sessions;  // key:sessionId
    Time _curTime;
    std::set<IFS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    std::set<FS_IocpSession *> _toPostRecv;
    std::set<FS_IocpSession *> _toPostSend;
    std::set<FS_IocpSession *> _toRemove;

    // TODO:订阅网络协议的facade开发，使用委托方式，各个系统关注各自的协议，注册到本系统

//     _CrtMemState s1;
//     _CrtMemState s2;
//     _CrtMemState s3;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_IocpMsgDispatcherImpl.h"

#endif

#endif
