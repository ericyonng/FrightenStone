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
 * @file  : FS_NetEngine.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/7
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_NetEngine_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_NetEngine_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/component/Impl/Time.h"
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include <FrightenStone/common/status/status.h>

FS_NAMESPACE_BEGIN

class  IFS_Session;
class  IFS_ServerConfigMgr;
class  IFS_Connector;
class  IFS_Acceptor;        // TODO
class  IFS_MsgTransfer;
class  IFS_MsgDispatcher;
class  FS_SessionMgr;
class  FS_CpuInfo;
class  FS_ThreadPool;
class  TimeSlice;
class  Time;
class  IFS_BusinessLogic;
class  ConcurrentMessageQueue;
struct NetMsg_DataHeader;
class  MessageQueue;
struct BriefSessionInfo;

class BASE_EXPORT FS_NetEngine
{
    friend class FS_IocpAcceptor;
public:
    FS_NetEngine();
    virtual ~FS_NetEngine();

    /* 对外接口 */
    #pragma region api
public:
    virtual Int32 Init();
    virtual Int32 Start();
    virtual void Wait();
    virtual void Close();
    #pragma endregion
    
    /* 网络事件 */
    #pragma region net event
    /*
    * brief: 
    *       1. 4 多个线程触发 不安全 如果只开启1个FS_Server就是安全的
    *       2. _OnNetMonitorTask 监听网络任务 OnRun(旧版) 建议多条线程去做monitor而不是单条线程，完成端口的get是线程安全的
    *       3. OnNetJoin 玩家加入 线程不安全
    *       4. OnNetLeave 玩家掉线 线程不安全
    *       5. OnNetMsg 玩家消息到来（消息是从FS_Server的_HandleNetMsg传入）线程不安全 NetMsg_DataHeader 转发到其他线程需要拷贝避免消息被覆盖
    *       6. OnNetRecv 接收到数据 线程不安全
            7. 接口只能由msgtransfer调度，避免线程不安全 msghandler不能有fs_session对象，只能创建user对象使用userId与sessionId关联，且不处理session的任意内容，只持有sessionId,避免导致线程不安全
    */
protected:
    void _OnConnected(BriefSessionInfo *sessionInfo);
    void _OnDisconnected(IFS_Session *session);
    void _OnHeartBeatTimeOut(IFS_Session *session);
    // 每接收一个完整包调用一次
    void _OnRecvMsg(IFS_Session *session, Int64 transferBytes);
    void _OnRecvMsgAmount(NetMsg_DataHeader *msgArrived);
    // 发送只能统计字节数，包数无法支持统计
    void _OnSendMsg(IFS_Session *session, Int64 transferBytes);

protected:
    // 服务器负载情况
    virtual void _OnSvrRuning(FS_ThreadPool *threadPool);
    void _PrintSvrLoadInfo(const TimeSlice &dis);
    #pragma endregion

    // 派生类重写
protected:
    // 读取配置位置
    virtual Int32 _OnReadCfgs() = 0;
    // 获取业务层,以便绑定到dispatcher上
    virtual void _GetLogics(std::vector<IFS_BusinessLogic *> &logics) {}
    // 初始化结束时
    virtual Int32 _InitFinish() { return StatusDefs::Success; }

    /* 内部方法 */
    #pragma region inner api
private:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    Int32 _AfterStart();

    void _WillClose();
    void _BeforeClose();
    void _AfterClose();

    // 将监听的接口注册到模块中
    void _RegisterToModule();

    std::vector<MessageQueue *> &_GetSenderMq();
    #pragma endregion

private:
    std::atomic_bool _isInit;
    FS_CpuInfo *_cpuInfo;                               // cpu信息
    //IFS_ServerConfigMgr *_serverConfigMgr;            // 服务器配置,配置依赖具体的派生类,不可被抽象

    Locker _locker;
    IFS_Connector * _connector;                         // 连接器
    UInt32 _acceptorQuantity;                            // 接受连接数量
    std::vector<IFS_Acceptor *> _acceptors;             // 支持监听多端口，具体看派生类对象的配置
    UInt32 _transferCnt;                                 // 消息收发器数量
    std::vector<IFS_MsgTransfer *> _msgTransfers;       // 多线程消息收发器
    ConcurrentMessageQueue *_messageQueue;              // 消息队列
    std::vector<MessageQueue *> _senderMessageQueue;    // 发送消息队列

    UInt32 _dispatcherCnt;
    Int64 _dispatcherResolutionInterval;                // 消息分配器的时间轮盘精度 microseconds 默认1ms
    std::vector<IFS_MsgDispatcher *> _msgDispatchers;   // 业务消息处理器 业务线程处理,支持多线程并发处理
    // std::vector<IFS_BusinessLogic *> _logics;        // 多业务逻辑并发 logic应该并到每个dispatcher中

    // TODO:sessionmgr可能需要移除避免锁冲突
    ConditionLocker _waitForClose;                      // 一般在主线程，用于阻塞等待程序结束
    FS_ThreadPool *_pool;

    // 统计数据
    Time _lastStatisticsTime;                       // 最后一次统计的时间
    TimeSlice _statisticsInterval;                  // 统计的时间间隔
    std::atomic<Int64> _curSessionConnecting;       // 正连接的会话数量
    std::atomic<Int64> _sessionConnectedBefore;     // 曾经连入的会话数量
    std::atomic<Int64> _sessionDisconnectedCnt;     // 断开链接的会话数量
    std::atomic<Int64> _recvMsgCountPerSecond;      // 每秒收到的包个数
    std::atomic<Int64> _recvMsgBytesPerSecond;      // 每秒收到的包的字节数
    std::atomic<Int64> _sendMsgBytesPerSecond;      // 每秒发送的包字节数
    std::atomic<Int64> _heartbeatTimeOutDisconnected;   // 心跳超时断开会话数

   // 连接的会话数据
    Locker _sessionlocker;
    Int32 _curSessionCnt;
    Int32 _maxSessionQuantityLimit;
    UInt64 _curMaxSessionId;
    const UInt64 _maxSessionIdLimit;
    Int32 _connectTimeOutMs;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_NetEngineImpl.h"

// extern BASE_EXPORT fs::FS_ServerCore *g_ServerCore;                         // 服务核心
// extern BASE_EXPORT fs::IFS_ServerConfigMgr *g_SvrCfg;                       // 服务器配置
// extern BASE_EXPORT fs::IFS_MsgDispatcher *g_Dispatcher;                     // 网络包分发器
// extern BASE_EXPORT fs::IFS_BusinessLogic *g_Logic;                          // 业务逻辑层
// extern BASE_EXPORT fs::ConcurrentMessageQueue *g_net2LogicMessageQueue;     // 网络层到业务层的消息队列


#endif
