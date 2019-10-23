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
#ifndef __Base_Common_Net_Impl_FS_IocpMsgDispatcher_H__
#define __Base_Common_Net_Impl_FS_IocpMsgDispatcher_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/IFS_MsgDispatcher.h"
#include "base/common/asyn/asyn.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include <set>
#include "base/common/component/Impl/TimeSlice.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT IFS_Session;
struct BASE_EXPORT NetMsg_DataHeader;
class BASE_EXPORT IFS_MsgTransfer;
class BASE_EXPORT IFS_BusinessLogic;

class BASE_EXPORT FS_IocpMsgDispatcher : public IFS_MsgDispatcher
{
public:
    FS_IocpMsgDispatcher();
    virtual ~FS_IocpMsgDispatcher();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();

    virtual void OnRecv(IFS_Session *session, Int64 &incPacketsCnt);
    virtual void OnDisconnected(IFS_Session *session);
    virtual void OnConnect(UInt64 sessionId, IFS_MsgTransfer *transfer);
    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut();

    virtual void SendData(UInt64 sessionId, NetMsg_DataHeader *msg);
    virtual void BindBusinessLogic(IFS_BusinessLogic *businessLogic);

private:
    // msgData会拷贝到内存池创建的缓冲区中 线程不安全，外部需要加锁
    void _OnBusinessProcessThread(const FS_ThreadPool *pool);
    void _MoveToBusinessLayer(IFS_Session *session, NetMsg_DataHeader *msgData);
    void _OnBusinessProcessing();

    void _DoBusinessProcess(UInt64 sessionId, NetMsg_DataHeader *msgData);
    void _OnDelaySessionDisconnect(UInt64 sessionId);

private:
    std::atomic<bool> _isClose;
    ConditionLocker _locker;
    ConditionLocker _connectLocker;
    FS_ThreadPool *_pool;

    // 数据在线程结束时清理
    std::atomic<bool> _isDataDirtied;
    std::set<UInt64> _newMsgSessionIds;     // TODO:最新的sessionId用于索引最新的数据避免全session遍历
    std::map<UInt64, std::list<NetMsg_DataHeader *> *> _sessionIdRefMsgs;
    std::map<UInt64, std::list<NetMsg_DataHeader *> *> _sessionIdRefMsgCache;      // 缓存：转移消息链表指针即可消耗小
    std::list<std::list<NetMsg_DataHeader *> *> _toFree;
    std::map<UInt64, IFS_MsgTransfer *> _sessionIdRefTransfer;
    std::set<UInt64> _delayDisconnectedSessions;
    std::set<UInt64> _delayDisconnectedSessionsCache;

    // 业务层资源
    TimeSlice _resolutionInterval;      // 时钟轮盘时间间隔
    TimeWheel *_timeWheel;
    IFS_BusinessLogic *_logic;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpMsgDispatcherImpl.h"

#endif
