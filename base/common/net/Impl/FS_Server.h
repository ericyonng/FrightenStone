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
 * @file  : FS_Server.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/3
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_Server_H__
#define __Base_Common_Net_Impl_FS_Server_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/asyn/asyn.h"
#include "base/common/net/Interface/INetEvent.h"
#include "base/common/component/Impl/Time.h"
#include "base/common/component/Impl/FS_ThreadPool.h"

FS_NAMESPACE_BEGIN

class FS_Client;

class BASE_EXPORT FS_Server
{
public:
    FS_Server();
    virtual ~FS_Server();

#pragma region 
public:
    size_t GetClientCount() const;
    virtual void SetClientNum(Int32 socketNum);
    void SetEventHandleObj(INetEvent *handleObj);
    void SetId(Int32 id);

    // 接收数据 处理粘包 拆分包
    Int32 RecvData(FS_Client *client);
    void AddClient(FS_Client *client);
    void Start();
    void Close();

private:
    // 收发客户端消息中转
    void _ClientMsgTransfer(const FS_ThreadPool *pool);
    // 客户端状态变化（连入/有待发送数据/有待接收数据）
    virtual bool _OnClientStatusDirtied() = 0;
    void _DetectClientHeartTime();
    void _OnClientLeave(FS_Client *client);
    virtual void _OnClientJoin(FS_Client *client);
    void _OnNetRecv(FS_Client *client);
    void _OnClientMsgTransfer();
    virtual void _HandleNetMsg(FS_Client *client, NetMsg_DataHeader *header);

    void _ClearClients();

protected:
    // 正式客户队列 隐患：不严格按照包到达时序处理，若两个包有先后依赖会出问题
    std::map<SOCKET, FS_Client *> _socketRefClients;
    std::set<FS_Client *> _clients;      // 严格时序 使用set兼顾时间排序与移除是O(Log n),消息变化时候先移除后插入以便重新排序

private:
    // 缓冲客户队列
    std::vector<FS_Client *> _clientsCache; // 待发送，接收,待连入缓冲
    // 缓冲队列的锁
    Locker _locker;
    // 网络事件处理对象
    INetEvent *_eventHandleObj = nullptr;
    // 旧的时间戳
    Time _oldTime = Time::Now();
    //
    FS_ThreadPool *_threadPool;
protected:
    // 服务id
    Int32 _id = -1;
    // 客户列表是否有变化
    bool _clientsChange = true;
};

FS_NAMESPACE_END

#endif
