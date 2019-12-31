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
 * @file  : IFS_NetEngine.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/28
 * @brief :
 * 
 *
 * 
 */
#pragma once
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_NetEngine_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_NetEngine_H__

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/component/Impl/TimeSlice.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class TimeSlice;
struct NetEngineTotalCfgs;
class IFS_Connector;
class IFS_Acceptor;
class IFS_MsgTransfer;
class ConcurrentMessageQueueNoThread;
class MessageQueueNoThread;

class BASE_EXPORT IFS_NetEngine
{
public:
    IFS_NetEngine();
    virtual ~IFS_NetEngine();

public:
    virtual Int32 Init();
    virtual Int32 Start();
    virtual void Wait();
    virtual void Close();

    // 组件事件
public:
    virtual void HandleCompEv_WillConnect(UInt64 sessionId, SOCKET sock, const sockaddr_in *addrInfo);

protected:
    virtual void _Monitor(FS_ThreadPool *threadPool);
    void _PrintInfo(const TimeSlice &dis);

    /* 启服关服时序调用 */
protected:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    Int32 _AfterStart();

    void _WillClose();
    void _BeforeClose();
    void _AfterClose();

    /* 配置 */
protected:
    // 读取配置位置
    virtual Int32 _OnReadCfgs() = 0;
    // 初始化结束时
    virtual Int32 _OnInitFinish() { return StatusDefs::Success; }

    // 生成组件id
    Int32 _GenerateCompId();

protected:
    NetEngineTotalCfgs *_totalCfgs;
    std::atomic_bool _isInit;

    Locker _locker;
    IFS_Connector * _connector;                         // 连接器
    std::vector<IFS_Acceptor *> _acceptors;             // 支持监听多端口，具体看派生类对象的配置
    std::vector<IFS_MsgTransfer *> _msgTransfers;       // 多线程消息收发器
    Int32 _curMaxCompId;                                // 组件id最大值

    ConcurrentMessageQueueNoThread *_mq;                // 单向,只能从生产者到消费者 generatorid 与consumerId需要特殊生成
    Int32 _curMaxGeneratorId;                           // 当前生产者最大id
    Int32 _curMaxConsumerId;                            // 当前最大消费者id
    std::map<Int32, MessageQueueNoThread *> _compIdRefConsumerMq;       // 组件对应消费者消息队列
    
    /* 统计数据 */ 
    Time _lastStatisticsTime;                       // 最后一次统计的时间
    TimeSlice _statisticsInterval;                  // 统计的时间间隔
    std::atomic<Int64> _curSessionConnecting;       // 正连接的会话数量
    std::atomic<Int64> _sessionConnectedBefore;     // 曾经连入的会话数量
    std::atomic<Int64> _sessionDisconnectedCnt;     // 断开链接的会话数量
    std::atomic<Int64> _recvMsgCountPerSecond;      // 每秒收到的包个数
    std::atomic<Int64> _recvMsgBytesPerSecond;      // 每秒收到的包的字节数
    std::atomic<Int64> _sendMsgBytesPerSecond;      // 每秒发送的包字节数
    std::atomic<Int64> _heartbeatTimeOutDisconnected;   // 心跳超时断开会话数
};


FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_NetEngineImpl.h"

#endif