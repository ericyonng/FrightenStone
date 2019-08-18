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
#include "base/common/net/Defs/HeartBeatComp.h"

FS_NAMESPACE_BEGIN

// 服务类
class BASE_EXPORT FS_Server
{
    OBJ_POOL_CREATE(FS_Server, _objPoolHelper);
public:
    FS_Server();
    virtual ~FS_Server();

    /* 杂项 */
    #pragma region misc
    /*
    *   brief:
    *       1. - GetClientCount 客户端数量
    *       2. - SetClientNum 设置最大服务的客户端数量
    *       3. - SetEventHandleObj 设置网络消息处理对象
    *       4. - SetId 设置服务id
    *       5. - _ClearClients 清理客户端
    */
public:
    size_t GetClientCount() const;
    virtual void SetClientNum(Int32 socketNum);
    void SetEventHandleObj(INetEvent *handleObj);
    void SetId(Int32 id);
private:
    void _ClearClients();
    #pragma endregion

    /* 接受数据/添加客户端/启动服务/停止服务 */
    #pragma region recv/addclient/start/close
public:
    // 接收数据 处理粘包 拆分包
    Int32 RecvData(FS_Client *client);
    void AddClient(FS_Client *client);
    void Start();
    virtual void BeforeClose();
    void Close();
    #pragma endregion

    /* 网络消息处理 */
    #pragma region net message handle
    /*
    *   brief:
    *       1. - _ClientMsgTransfer 收发客户端消息中转
    *       2. - _OnClientStatusDirtied 客户端状态变化（连入/有待发送数据/有待接收数据）
    *       3. - _DetectClientHeartTime 客户端心跳检测，TODO:需要优化
    *       4. - _OnClientLeave 客户端断开
    *       5. - _OnClientJoin 客户端连入 iocp与epoll有不同的重写方法
    *       6. - _OnNetRecv 接收网络消息
    *       7. - _OnClientMsgTransfer 网络消息再次中转
    *       8. - _HandleNetMsg 网络消息处理 <真正的消息入口> 
    *               NetMsg_DataHeader 是缓冲区中的数据，
    *               若要转发到其他地方需要进行拷贝否则会被覆盖数据
    */
protected:
    void _ClientMsgTransfer(const FS_ThreadPool *pool);
    virtual Int32 _BeforeClientMsgTransfer(std::set<SOCKET> &delayDestroyClients) = 0;
    
    // TODO:心跳优化
    void _DetectClientHeartTime();
    void _AddToHeartBeatQueue(FS_Client *client);
    void _OnClientHeartBeatUpdate(FS_Client *client);

    void _RmClient(FS_Client *client);
    void _OnClientLeave(FS_Client *client);
    virtual void _OnClientJoin(FS_Client *client);
    void _OnPrepareNetRecv(FS_Client *client);

    void _OnClientMsgArrived();
    virtual Int32 _HandleNetMsg(FS_Client *client, NetMsg_DataHeader *header);
    #pragma endregion

    /* 数据成员 */
    #pragma region data member
protected:
    // 正式客户队列 隐患：不严格按照包到达时序处理，若两个包有先后依赖会出问题
    std::map<SOCKET, FS_Client *> _socketRefClients;
    std::set<FS_Client *, HeartBeatComp> _clientHeartBeatQueue;

private:
    // 缓冲客户队列
    std::vector<FS_Client *> _clientsCache; // 待发送，接收,待连入缓冲
    // 缓冲队列的锁
    Locker _locker;
    // 网络事件处理对象
    INetEvent *_eventHandleObj = NULL;    // 内部不释放
    // 旧的时间戳
    Time _lastHeartDetectTime = Time::Now();
    // 线程
    FS_ThreadPool *_threadPool;
    std::set<SOCKET> _delayRemoveClients;
protected:
    // 服务id
    Int32 _id = -1;
    // 客户列表是否有变化
    bool _clientsChange = true;
    Int32 _clientJoin = 0;
    #pragma endregion
};

FS_NAMESPACE_END
#include "base/common/net/Impl/FS_ServerImpl.h"

#endif
