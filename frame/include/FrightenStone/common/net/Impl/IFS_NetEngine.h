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
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class TimeSlice;
struct NetEngineTotalCfgs;
class IFS_Connector;
class IFS_Acceptor;
class IFS_MsgTransfer;
class ConcurrentMessageQueueNoThread;
class MessageQueueNoThread;
class FS_CpuInfo;
struct BriefSessionInfo;
class IFS_MsgDispatcher;
class IFS_EngineComp;
class IFS_BusinessLogic;
class IFS_ConfigMgr;

// 注意:各个模块启线程应该放到after start做，所有的数据准备不应该在afeterstart做
class BASE_EXPORT IFS_NetEngine
{
    OBJ_POOL_CREATE_DEF(IFS_NetEngine);
public:
    IFS_NetEngine();
    virtual ~IFS_NetEngine();

public:
    virtual Int32 Init();
    virtual Int32 Start();
    virtual void Wait();
    virtual void Sinal();
    virtual void Close();
    bool IsCompAllReady() const;        // 线程安全
    bool IsCompAllFinish() const;       // 线程安全

    /* 组件事件 */
protected:
    virtual void _HandleCompEv_WillConnect(BriefSessionInfo *newSessionInfo);
    virtual void _HandleCompEv_Disconnected(UInt64 sessionId, UInt32 acceptorCompId, UInt32 transferCompId);
    virtual void _HandleCompEv_HeartBeatTimeOut();
    virtual void _HandleCompEv_RecvMsg(Int64 transferBytes);
    virtual void _HandleCompEv_RecvMsgAmount();
    virtual void _HandleCompEv_SendMsg(Int64 transferBytes);
    virtual void _HandleCompEv_ConnectorPostConnectOpFinish();

protected:
    virtual void _Monitor(FS_ThreadPool *threadPool);
    void _PrintInfo(const TimeSlice &dis);
    virtual void _ModifyCustomPrintInfo(FS_String &customStr) {}

    /* 启服关服时序调用 */
protected:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    void _AfterStart();

    void _WillClose();
    void _BeforeClose();
    void _AfterClose();

    /* 配置 */
protected:
    // 准备配置文件
    virtual IFS_ConfigMgr *_PreparConfigFile() const = 0;
    virtual Int32 _ReadBaseCfgs(IFS_ConfigMgr *cfgMgr);
    virtual Int32 _ReadCustomCfgs() { return StatusDefs::Success; } // 自定义的配置
    virtual Int32 _OnReadCfgs();
    // 初始化结束时
    virtual Int32 _OnInitFinish() { return StatusDefs::Success; }
    // 获取业务层,以便绑定到dispatcher上
    virtual void _GetLogics(std::vector<IFS_BusinessLogic *> &logics) {}

    /* 辅助 */
protected:
    // 生成组件id
    UInt32 _GenerateCompId();
    // 获取transfer->dispatcher并发消息队列
    ConcurrentMessageQueueNoThread *_GetConcurrentMQ();

    void _AddNewComp(UInt32 compId, IFS_EngineComp *newComp);
    IFS_EngineComp *_GetComp(UInt32 compId);

protected:
    /* 友元 */
    friend class FS_IocpPoller;
    friend class FS_IocpConnector;
    friend class FS_IocpAcceptor;
    friend class FS_IocpMsgDispatcher;
    
    NetEngineTotalCfgs *_totalCfgs;
    FS_CpuInfo *_cpuInfo;
    std::atomic_bool _isInit;

    Locker _locker;
    IFS_Connector * _connector;                         // 连接器
    std::vector<IFS_Acceptor *> _acceptors;             // 支持监听多端口，具体看派生类对象的配置
    std::vector<IFS_MsgTransfer *> _msgTransfers;       // 多线程消息收发器
    std::vector<IFS_MsgDispatcher *> _msgDispatchers;   // 业务消息处理器 业务线程处理,支持多线程并发处理
    UInt32 _curMaxCompId;                                // 组件id最大值 最小值从1开始
    std::map<UInt32, IFS_EngineComp *> _compIdRefComps; // 组件

    ConcurrentMessageQueueNoThread *_concurrentMq;                // 单向,只能从生产者到消费者 generatorid 与consumerId需要特殊生成
    std::vector<MessageQueueNoThread *> _compConsumerMq;            // 用compId做下表索引
    
    /* 统计数据 */ 
    ConditionLocker _waitForClose;                  // 一般在主线程，用于阻塞等待程序结束
    FS_ThreadPool *_pool;
    Time _lastStatisticsTime;                       // 最后一次统计的时间
    TimeSlice _statisticsInterval;                  // 统计的时间间隔
    std::atomic<Int64> _curSessionConnecting;       // 正连接的会话数量
    std::atomic<Int64> _sessionConnectedBefore;     // 曾经连入的会话数量
    std::atomic<Int64> _sessionDisconnectedCnt;     // 断开链接的会话数量
    std::atomic<Int64> _recvMsgCountPerSecond;      // 每秒收到的包个数
    std::atomic<Int64> _recvMsgBytesPerSecond;      // 每秒收到的包的字节数
    std::atomic<Int64> _sendMsgBytesPerSecond;      // 每秒发送的包字节数
    std::atomic<Int64> _heartbeatTimeOutDisconnected;   // 心跳超时断开会话数
    
    /* 连接的会话数据 */ 
    Locker _sessionlocker;
    Int32 _curSessionCnt;
    UInt64 _curMaxSessionId;
};


FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_NetEngineImpl.h"

#endif