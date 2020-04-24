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
 * @file  : FS_EpollMsgDispatcher.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/26
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollMsgDispatcher_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_EpollMsgDispatcher_H__

#pragma once

#ifndef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_MsgDispatcher.h"
#include "FrightenStone/common/net/Defs/NetCfgDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Defs/HeartBeatComp.h"
#include "FrightenStone/common/net/Defs/FS_NetMessageBlock.h"
#include "FrightenStone/common/net/Defs/FS_EpollEvMessageBlock.h"

#include "FrightenStone/common/asyn/asyn.h"
#include <set>
#include "FrightenStone/common/component/Impl/TimeSlice.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class IFS_Session;
class FS_EpollSession;
class IFS_NetEngine;
class IFS_BusinessLogic;
struct NetEngineTotalCfgs;
struct NetMsg_DataHeader;
struct FS_MessageBlock;
class TimeWheel;
class FS_ThreadPool;
class NetMsgDecoder;
class IMemoryAlloctor;
struct FS_EpollEvMessageBlock;

class BASE_EXPORT FS_EpollMsgDispatcher : public IFS_MsgDispatcher
{
    OBJ_POOL_CREATE_DEF(FS_EpollMsgDispatcher);
public:
    FS_EpollMsgDispatcher(IFS_NetEngine *netEngine, UInt32 compId);
    virtual ~FS_EpollMsgDispatcher();

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
    virtual TimeWheel *GetTimeWheel();
    virtual const IFS_Session *GetSession(UInt64 sessionId) const;
    // 发送消息msg在内部会被拷贝到缓冲区
    virtual void SendData(UInt64 sessionId, NetMsg_DataHeader *msg);
    virtual void SendData(UInt64 sessionId, const Byte8 *msgBuffer, UInt64 msgLen);
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
    // 清理所有会话并等待transfer停止服务
    void _CancelAllSessionIosAndWaitForTransferIoFinish();
    // 清理所有会话并等待transfer停止服务
    void _ClearAllSessions();
    // 等待transfer停止
    // 通知所有系统关服 只能在业务线程做
    void _CloseLogicSvc();

    /* 操作会话 */
    bool _DoPostRecv(FS_EpollSession *session);
    bool _DoPostSend(FS_EpollSession *session);
    void _RemoveSessionGracefully(FS_EpollSession *session);
    void _RemoveFromPostRecvQueue(FS_EpollSession *session);
    void _RemoveFromPostSendQueue(FS_EpollSession *session);
    void _UpdateSessionHeartbeat(IFS_Session *session);

    /* 杂项 */
private:
    FS_EpollSession *_GetSession(UInt64 sessionId);
    const FS_EpollSession *_GetSession(UInt64 sessionId) const;
    void _PrintAlloctorOccupiedInfo();

    /* 消息队列处理 */
private:
    // 收到网络包
    void _OnMsgBlockRecvData(FS_EpollEvMessageBlock *msgBlock);
    // 发送成功事件
    void _OnMsgBlockSendSuc(FS_EpollEvMessageBlock *msgBlock);
    // 会话连入
    void _OnMsgBlockNetSessionConnected(FS_EpollEvMessageBlock *msgBlock);
    // 会话断开
    // void _OnMsgBlockNetSessionDisconnected(FS_EpollEvMessageBlock *msgBlock);
    // 连接器连接操作完成,但并不代表所有连接都已成功
    void _OnMsgBlockConnectorConnectOpFinish(FS_EpollEvMessageBlock *msgBlock);

    /* 网络事件 */
private:
    // 会话断开
    void _OnSessionDisconnectedNoFree(FS_EpollSession *session);
    // 会话断开
    void _OnSessionDisconnected(FS_EpollSession *session);
    // 会话连入
    void _OnSessionConnected(EpollEvSessionWillConnectMsg *connectedMsg);
    // 远端会话消息到达
    void _OnMsgRecv(EpollRecvDataMessageBlock *recvMsg);
    // 消息发送成功
    void _OnMsgSendSuc(EpollSendSucMessageBlock *sendSucMsg);
    // 会话消息处理
    void _OnSessionMsgHandle(FS_EpollSession *session);

private:
    void _MaskCloseSession(FS_EpollSession *session);

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
    std::map<UInt64, FS_EpollSession *> _sessions;  // key:sessionId
    Time _curTime;
    std::set<IFS_Session *, HeartBeatComp> _sessionHeartbeatQueue;  // 心跳队列
    std::set<FS_EpollSession *> _toPostRecv;
    std::set<FS_EpollSession *> _toPostSend;
    std::set<FS_EpollSession *> _toRemove;

    /* 协议相关 */
    NetMsgDecoder *_msgDecoder;

    // 消息队列处理器
    typedef void (FS_EpollMsgDispatcher::*MessageQueueHandler)(FS_EpollEvMessageBlock *msgBlock);
    static MessageQueueHandler _msgBlockHandler[FS_EpollEvMessageType::End];
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/FS_EpollMsgDispatcherImpl.h"

#endif

#endif
