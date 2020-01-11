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
#include "FrightenStone/common/net/Defs/FS_NetMessageBlock.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class IFS_Session;
struct NetMsg_DataHeader;
class IFS_MsgTransfer;
class IFS_BusinessLogic;
class ConcurrentMessageQueueNoThread;
struct FS_MessageBlock;
class IFS_NetEngine;
class FS_IocpSession;
struct FS_NetSessionWillConnectMsg;
struct FS_NetArrivedMsg;
struct FS_NetMsgBufferBlock;

class BASE_EXPORT FS_IocpMsgDispatcher : public IFS_MsgDispatcher
{
public:
    FS_IocpMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId);
    virtual ~FS_IocpMsgDispatcher();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &cfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();
    virtual void BindBusinessLogic(IFS_BusinessLogic *businessLogic);
    virtual Int32 GetSessionCnt() const;
    // 发送消息msg在内部会被拷贝到缓冲区
    virtual void SendData(UInt64 sessionId, NetMsg_DataHeader *msg);
    virtual void CloseSession(UInt64 sessionId);

private:
    // 业务线程
    void _OnBusinessProcessThread(FS_ThreadPool *pool);
    void _RunAFrame(bool hasMsg);
    // 心跳检查
    void _CheckHeartbeat();
    // 业务消息处理
    void _OnBusinessProcessing();
    // 统一推送投递消息
    void _PostEvents();
    // 延迟移除断开的会话
    void _WillRemoveSessions();
    // 清除所有会话
    void _ClearAllSessions();

    /* 操作会话 */
    bool _DoPostRecv(FS_IocpSession *session);
    bool _DoPostSend(FS_IocpSession *session);
    void _RemoveSessionGracefully(FS_IocpSession *session);
    void _RemoveFromPostRecvQueue(FS_IocpSession *session);
    void _RemoveFromPostSendQueue(FS_IocpSession *session);
    void _UpdateSessionHeartbeat(IFS_Session *session);

    /* 杂项 */
private:
    FS_IocpSession *_GetSession(UInt64 sessionId);
    void _PrintAlloctorOccupiedInfo();

    /* 消息队列处理 */
private:
    // 收到网络包
    void _OnMsgBlockNetMsgArrived(FS_NetMsgBufferBlock *msgBlock);
    // 会话连入
    void _OnMsgBlockNetSessionConnected(FS_NetMsgBufferBlock *msgBlock);
    
    /* 网络事件 */
private:
    // 会话断开
    void _OnSessionDisconnected(FS_IocpSession *session);
    // 会话连入
    void _OnSessionConnected(FS_NetSessionWillConnectMsg *connectedMsg);
    // 远端会话消息到达
    void _OnMsgArrived(FS_NetArrivedMsg *arrivedMsg);
    // 会话消息处理
    void _OnSessionMsgHandle(FS_IocpSession *session);

private:
    DispatcherCfgs *_cfgs;
    std::atomic<bool> _isClose;
    FS_ThreadPool *_pool;

    /* 业务层资源 */
    TimeWheel *_timeWheel;
    IFS_BusinessLogic *_logic;
    std::map<UInt64, std::list<IDelegate<void, UInt64> *>> _sessionIdRefUserDisconnected;
    std::vector<std::list<FS_MessageBlock *> *> *_recvMsgBlocks;       // 需要转换成 FS_NetMsgBufferBlock

    /* 会话 */ 
    IMemoryAlloctor *_sessionBufferAlloctor;
    IDelegate<void> *_printAlloctorOccupiedInfo;
    UInt64 _transferThreadId;

    std::atomic<Int32> _curSessionQuatity;
    std::map<UInt64, FS_IocpSession *> _sessions;  // key:sessionId
    Time _curTime;
    std::set<IFS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    std::set<FS_IocpSession *> _toPostRecv;
    std::set<FS_IocpSession *> _toPostSend;
    std::set<FS_IocpSession *> _toRemove;
    
    // 消息队列处理器
    typedef void (FS_IocpMsgDispatcher::*MessageQueueHandler)(FS_NetMsgBufferBlock *msgBlock);
    static MessageQueueHandler _msgBlockHandler[NetMessageBlockType::End];

    // TODO:订阅网络协议的facade开发，使用委托方式，各个系统关注各自的协议，注册到本系统

//     _CrtMemState s1;
//     _CrtMemState s2;
//     _CrtMemState s3;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_IocpMsgDispatcherImpl.h"

#endif

#endif
