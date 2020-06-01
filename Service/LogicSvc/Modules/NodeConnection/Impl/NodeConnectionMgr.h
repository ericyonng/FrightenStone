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
 * @file  : NodeConnectionMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/04/06
 * @brief :
 */
#ifndef __Service_LogicSvc_Modules_NodeConnection_Impl_NodeConnectionMgr_H__
#define __Service_LogicSvc_Modules_NodeConnection_Impl_NodeConnectionMgr_H__

#pragma once

#include "Service/LogicSvc/Modules/NodeConnection/Interface/INodeConnectionMgr.h"

class User;
class PendingStubInfo;

class NodeConnectionMgr : public INodeConnectionMgr
{
public:
    NodeConnectionMgr();
    ~NodeConnectionMgr();

public:
    virtual Int32 OnInitialize();

public:
    virtual Int32 AsynConnect(const fs::FS_ConnectInfo &connectInfo);
    /* 异步连接
    * @param:connectInfo 连接信息
    * @param:connectInfo callBack:
                                1. rtn:void 无返回值
                                2. @param:sessionId 会话id,连接失败情况会为0
                                3. @param:PendingStubInfo * 存根
                                4. @param:resultCode 返回的状态码
    */
    virtual Int32 AsynConnect(const fs::FS_ConnectInfo &connectInfo, fs::IDelegate<void, UInt64, PendingStubInfo *, Int32> *callBack);

private:
    void _RegisterEvents();
    void _UnRegisterEvents();
    void _OnLocalServerReady(fs::FS_Event *ev);
    // 连接成功
    void _OnAfterSessionConnected(fs::FS_Event *ev);
    // 连接断开
    void _OnSessionDisconnect(fs::FS_Event *ev);
    // 连入失败
    void _OnSessionConnectFail(fs::FS_Event *ev);
    // 连接失败
    void _OnConnectFailure(fs::FS_Event *ev);
    // 连接完成处理
    void _OnConnectFinish(UInt64 sessionId, const fs::FS_String &ip, UInt16 port, PendingStubInfo *stubInfo, Int32 resultCode);

    Int32 _AsynConnect(const fs::FS_ConnectInfo &connectInfo, fs::IDelegate<void, UInt64, PendingStubInfo *, Int32> *callBack = NULL);

    void _FireAsynConnectResult(UInt64 sessionId,  const fs::FS_String &ip, UInt16 port, UInt64 stub, Int32 resultCode);

    void _AddNewPendingStubInfo(UInt64 stub, PendingStubInfo *newStubInfo);
    void _RemovePendingStubInfo(PendingStubInfo *stubInfo);

private:
    std::map<UInt64, PendingStubInfo *> _pendingStubRefInfos;
    std::map<UInt64, User *> _sessionIdRefConnectedUsers;

    fs::FS_ListenerStub _onLocalServerReadyStub;
    fs::FS_ListenerStub _onAfterSessionConnectedStub;
    fs::FS_ListenerStub _onSessionDisconnectStub;
    fs::FS_ListenerStub _onSessionConnectFailStub;
    fs::FS_ListenerStub _onConnectFailureStub;
};

#endif
