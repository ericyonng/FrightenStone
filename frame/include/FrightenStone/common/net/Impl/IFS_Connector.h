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
 * @file  : IFS_Connector.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Connector_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_Connector_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/InterfaceComps.h"
#include "FrightenStone/common/net/Defs/ServerCompsDef.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVar.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"
#include "FrightenStone/common/net/Defs/NetMessageBlock.h"

FS_NAMESPACE_BEGIN

class IFS_Session;
struct BriefSessionInfo;
struct FS_ConnectInfo;
struct ConnectorCfgs;
class FS_ThreadPool;
struct FS_NetMsgBlock;

class BASE_EXPORT IFS_Connector : public IFS_EngineComp
{
public:
    IFS_Connector(IFS_NetEngine *netEngine
        , UInt32 compId
        , Locker &locker
        , Int32 &curSessionCnt
        , Int32 &maxSessionQuantityLimit
        , UInt64 &curMaxSessionId);
    virtual ~IFS_Connector();

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &cfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

public:
    // 连接
    virtual Int32 Connect(UInt32 dispatcherConsumerId, const FS_ConnectInfo &connectInfo);
    // 异步连接请求
    virtual Int32 AsynConnect(UInt32 dispatcherMsgQueueConsumerId, const FS_ConnectInfo &connectInfo);

private:
    // 断开 从dispatcher调用委托回调
    void _OnUserDisconnected(UInt64 sessionId);

    Int32 _CheckConnect(const FS_ConnectInfo &connectInfo, FS_String &addrInfoOut) const;
    void _MakeAddrInfo(const FS_String &ip, UInt64 port, FS_String &addrInfo) const;
    Int32 _Connect(SOCKET sock, const sockaddr_in &sin, const FS_ConnectInfo &connectInfo);
    void _OnConnectFailureRes(UInt32 dispatcherConsumerId, const FS_ConnectInfo &connectInfo, Int32 resultCode);

    // 监控消息到来
    virtual void _OnMonitor(FS_ThreadPool *pool);
    // 异步连接请求
    void _OnAsynConnection(FS_NetMsgBlock *msgBlock);
    // 连接断开
    void _OnSessionDisconnected(FS_NetMsgBlock *msgBlock);

protected:
    std::atomic_bool _isInit;
    // 连接器配置
    ConnectorCfgs * _cfgs;
    // 客户端连接上限
    Locker &_locker;
    Int32 &_curSessionCnt;
    Int32 &_maxSessionQuantityLimit;
    UInt64 &_curMaxSessionId;
    std::set<UInt64> _sucConnectedSessionIds;

    // 消息队列处理器
    FS_ThreadPool *_pool;
    typedef void (IFS_Connector::*MessageQueueHandler)(FS_NetMsgBlock *msgBlock);
    static MessageQueueHandler _msgBlockHandler[NetMessageType::End];
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_ConnectorImpl.h"

#endif
