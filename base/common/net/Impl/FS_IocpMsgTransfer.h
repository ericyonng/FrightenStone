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
 * @file  : FS_IocpMsgTransfer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_IocpMsgTransfer_H__
#define __Base_Common_Net_Impl_FS_IocpMsgTransfer_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/IFS_MsgTransfer.h"
#include "base/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT FS_Iocp;
class BASE_EXPORT IO_EVENT;

class BASE_EXPORT FS_IocpMsgTransfer : public IFS_MsgTransfer
{
public:
    FS_IocpMsgTransfer();
    virtual ~FS_IocpMsgTransfer();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();

    virtual void OnConnect(IFS_Session *session);
    virtual void OnDestroy();
    virtual void OnHeartBeatTimeOut();

    virtual void RegisterDisconnected(IDelegate<void, IFS_Session *> *callback);
    virtual void RegisterRecvSucCallback(IDelegate<void, IFS_Session *, Int64> *callback);
    virtual void RegisterSendSucCallback(IDelegate<void, IFS_Session *, Int64> *callback);
    virtual Int32 GetSessionCnt();

private:
    virtual void _OnMoniterMsg(const FS_ThreadPool *pool);
    Int32 _ListenNetEvents();
    void _MoveToSessions();
    void _CheckHeartbeat();
    void _DoSessionPost();

    // 网络事件 线程不安全
private:
    void _OnDisconnected(IFS_Session *session);

    // 辅助
private:
    IFS_Session *_GetSession(UInt64 sessionId);


private:
    Locker _locker;
    std::atomic<Int32> _sessionCnt;             // 会话个数
    std::map<UInt64, IFS_Session *> _sessions;  // key:sessionId

    FS_ThreadPool *_threadPool;
    FS_Iocp *_iocp;
    IO_EVENT *_ioEvent;

    IDelegate<void, IFS_Session *> *_serverCoreDisconnectedCallback;
    IDelegate<void, IFS_Session *, Int64> *_serverCoreRecvSucCallback;
    IDelegate<void, IFS_Session *, Int64> *_serverCoreSendSucCallback;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_IocpMsgTransferImpl.h"

#endif
