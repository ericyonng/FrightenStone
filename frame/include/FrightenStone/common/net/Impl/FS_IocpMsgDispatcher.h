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

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT IFS_Session;
struct BASE_EXPORT NetMsg_DataHeader;
class BASE_EXPORT IFS_MsgTransfer;
class BASE_EXPORT IFS_BusinessLogic;
class BASE_EXPORT ConcurrentMessageQueue;
struct BASE_EXPORT FS_MessageBlock;

class BASE_EXPORT FS_IocpMsgDispatcher : public IFS_MsgDispatcher
{
public:
    FS_IocpMsgDispatcher(UInt32 id);
    virtual ~FS_IocpMsgDispatcher();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();

    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut();

    virtual void SendData(UInt64 sessionId,  UInt64 consumerId, NetMsg_DataHeader *msg);
    virtual void BindBusinessLogic(IFS_BusinessLogic *businessLogic);
    virtual void AttachRecvMessageQueue(ConcurrentMessageQueue *messageQueue);
    virtual Int32 GetId() const;

private:
    // msgData会拷贝到内存池创建的缓冲区中 线程不安全，外部需要加锁
    void _OnBusinessProcessThread(FS_ThreadPool *pool);
    void _OnBusinessProcessing();

    void _DoBusinessProcess(UInt64 sessionId, UInt64 generatorId, NetMsg_DataHeader *msgData);
    void _OnDelaySessionDisconnect(UInt64 sessionId);

private:
    UInt32 _id;
    std::atomic<bool> _isClose;
    FS_ThreadPool *_pool;

    // 数据在线程结束时清理
    std::set<UInt64> _delayDisconnectedSessions;

    // 业务层资源
    TimeSlice _resolutionInterval;      // 时钟轮盘时间间隔
    TimeWheel *_timeWheel;
    IFS_BusinessLogic *_logic;

    std::list<FS_MessageBlock *> *_recvMsgBlocks;       // 需要转换成 FS_NetMsgBufferBlock
    ConcurrentMessageQueue *_messgeQueue;

    // TODO:订阅网络协议的facade开发，使用委托方式，各个系统关注各自的协议，注册到本系统

//     _CrtMemState s1;
//     _CrtMemState s2;
//     _CrtMemState s3;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_IocpMsgDispatcherImpl.h"

#endif

#endif
