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
 * @file  : IFS_MsgDispatcher.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgDispatcher_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgDispatcher_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"
#include "FrightenStone/common/net/Defs/HeartBeatComp.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/crashhandle/CrashHandle.h"

FS_NAMESPACE_BEGIN

struct NetMsg_DataHeader;
class  IFS_MsgTransfer;
class  IFS_BusinessLogic;
class  ConcurrentMessageQueueNoThread;
struct DispatcherCfgs;
class IFS_Session;
struct FS_ConnectInfo;
class NetMsgDecoder;

class BASE_EXPORT IFS_MsgDispatcher : public IFS_EngineComp
{
public:
    IFS_MsgDispatcher(IFS_NetEngine *engine, UInt32 compId);
    virtual ~IFS_MsgDispatcher();

    /* 引擎对象启动与关闭 */
public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &cfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

    /* 参数获取与设定 */
public:
    // 获取生产者id
    virtual UInt32 GetGeneratorId() const;
    // 获取消费者id
    virtual UInt32 GetConsumerId() const;
    virtual Int32 GetSessionCnt() const;
    virtual TimeWheel *GetTimeWheel();
    bool IsClose() const;
    // 并发消息队列参数绑定
    virtual void BindConcurrentParams(UInt32 generatorId, UInt32 consumerId, ConcurrentMessageQueueNoThread *concurrentMq);
    virtual void BindBusinessLogic(IFS_BusinessLogic *businessLogic);
    IFS_BusinessLogic *GetLogic();
    const IFS_BusinessLogic *GetLogic() const;

    const FS_Session *GetSession(UInt64 sessionId) const;
    FS_Session *GetSession(UInt64 sessionId);
    // 业务层只能由user调用
    void CloseSession(UInt64 sessionId);
    // 底层不应该与自定义协议耦合TODO:
    void SendData(UInt64 sessionId, const Byte8 *msgBuffer, UInt64 msgLen);
    virtual Int32 AsynConnect(const FS_ConnectInfo &connectInfo);
    void MaskSend(FS_Session *session);

protected:
    // 业务线程
    void _OnBusinessProcessThread(FS_ThreadPool *pool);
    // 执行一帧
    void _RunAFrame(bool hasMsg);
    // 心跳检查
    void _CheckHeartbeat();
    // 业务消息处理
    void _OnBusinessProcessing();
    // 统一推送投递消息
    void _PostEvents();
    // 延迟移除断开的会话
    void _WillRemoveSessions();
    // 等待transfer停止
    // 通知所有系统关服 只能在业务线程做
    void _CloseLogicSvc();
    // 清理所有会话并等待transfer停止服务
    void _ClearAllSessions();
    // 本地服务准备就绪
    void _OnLocalServerReady();
    // 即将启动
    void _OnWillStartup();

#pragma region platform relation
#ifndef _WIN32
    // 清理所有会话并等待transfer停止服务
    void _CancelAllSessionIosAndWaitForTransferIoFinish();
#endif
#pragma endregion

    /* 操作会话 */
private:
    // 外部必须判断惨post
    bool _DoPostRecv(FS_Session *session);
    bool _DoPostSend(FS_Session *session);
    void _RemoveSessionGracefully(FS_Session *session);
    void _RemoveFromPostRecvQueue(FS_Session *session);
    void _RemoveFromPostSendQueue(FS_Session *session);
    void _UpdateSessionHeartbeat(FS_Session *session);

#pragma region net msg handle
    /* 消息队列处理 */
private:
    // 会话连入
    void _OnMsgBlockNetSessionConnected(FS_NetMsgBlock *msgBlock);
    // 连接失败
    void _OnMsgBlockConnectionFailure(FS_NetMsgBlock *msgBlock);

    /* 网络事件 */
private:
    // 会话断开
    void _OnSessionDisconnectedNoFree(FS_Session *session);
    // 会话断开
    void _OnSessionDisconnected(FS_Session *session);
    // 会话连入
    void _OnSessionConnected(SessionConnectedNetMsgBlock *connectedMsg);
    // 会话消息处理
    void _OnSessionMsgHandle(FS_Session *session);

#ifdef _WIN32

    /* 消息队列处理 */
private:
    // 收到网络包
    void _OnMsgBlockNetMsgArrived(FS_NetMsgBlock *msgBlock);

    /* 网络事件 */
private:
    // 远端会话消息到达
    void _OnMsgArrived(IocpMsgArrivedMsgBlock *arrivedMsg);

#else

    /* 消息队列处理 */
private:
    // 收到网络包
    void _OnMsgBlockRecvData(FS_NetMsgBlock *msgBlock);
    // 发送成功事件
    void _OnMsgBlockSendSuc(FS_NetMsgBlock *msgBlock);

    /* 网络事件 */
private:
    // 远端会话消息到达
    void _OnMsgRecv(EpollRecvDataMsgBlock *recvMsg);
    // 消息发送成功
    void _OnMsgSendSuc(EpollSendSucMsgBlock *sendSucMsg);

#endif
#pragma endregion

    /* 杂项 */
protected:
    void _PrintAlloctorOccupiedInfo();
    void _DelayCloseSession(FS_Session *session);
    void _InitMsgHandlers();

protected:
    UInt32 _generatorId;            // engine中concurrentmq指定的生产者id
    UInt32 _consumerId;             // 指定的，并发消息队列中对应的消费者id
    ConcurrentMessageQueueNoThread *_concurrentMq;  // 单向的消息队列,当组件是生产者时只能push不能pop,只能generatiorId

    DispatcherCfgs *_cfgs;
    std::atomic<bool> _isClose;
    FS_ThreadPool *_pool;

    /* 业务层资源 */
    TimeWheel *_timeWheel;
    IFS_BusinessLogic *_logic;
    std::map<UInt64, std::list<IDelegate<void, UInt64> *>> _sessionIdRefUserDisconnected;
    std::vector<std::list<FS_MessageBlock *> *> *_recvMsgBlocks;       // 需要转换成 FS_NetMsgBufferBlock

    /* 会话缓冲分配器,会话心跳 */
    IMemoryAlloctor *_sessionBufferAlloctor;
    IDelegate<void> *_printAlloctorOccupiedInfo;
    UInt64 _transferThreadId;
    Time _curTime;
    std::set<FS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    std::atomic<Int32> _curSessionQuatity;

    /* 会话管理 */ 
    std::map<UInt64, FS_Session *> _sessions;  // key:sessionId
    std::set<FS_Session *> _toPostRecv;
    std::set<FS_Session *> _toPostSend;
    std::set<FS_Session *> _toRemove;

    /* 消息队列处理器 */
    typedef void (IFS_MsgDispatcher::*MessageQueueHandler)(FS_NetMsgBlock *msgBlock);
    static MessageQueueHandler _msgBlockHandler[NetMessageType::End];
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_MsgDispatcherImpl.h"

#endif
