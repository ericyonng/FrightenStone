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
 * @file  : FS_ServerCore.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_ServerCore_H__
#define __Base_Common_Net_Impl_FS_ServerCore_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_ServerConfigMgr;
class BASE_EXPORT IFS_Connector;
class BASE_EXPORT IFS_MsgTransfer;
class BASE_EXPORT IFS_MsgHandler;
class BASE_EXPORT FS_SessionMgr;

class BASE_EXPORT FS_ServerCore
{
public:
    FS_ServerCore();
    virtual ~FS_ServerCore();

    /* 对外接口 */
    #pragma region api
public:
    virtual Int32 Start();
    virtual void Close();
    #pragma endregion
    
//     /* 网络事件 */
//     #pragma region net event
//     /*
//     * brief: 
//     *       1. FS_Server 4 多个线程触发 不安全 如果只开启1个FS_Server就是安全的
//     *       2. _OnNetMonitorTask 监听网络任务 OnRun(旧版) 建议多条线程去做monitor而不是单条线程，完成端口的get是线程安全的
//     *       3. OnNetJoin 玩家加入 线程不安全
//     *       4. OnNetLeave 玩家掉线 线程不安全
//     *       5. OnNetMsg 玩家消息到来（消息是从FS_Server的_HandleNetMsg传入）线程不安全 NetMsg_DataHeader 转发到其他线程需要拷贝避免消息被覆盖
//     *       6. OnNetRecv 接收到数据 线程不安全
//     */
// protected:
//     void _OnConnected(FS_Session *session);
//     void _OnDisconnected(FS_Session *session);
//     void _OnMsgArrived(FS_Session *session);
// 
// protected:
//     // 服务器负载情况
//     virtual void _OnSvrRuningDataRecord(const FS_ThreadPool *threadPool) = 0;
// 
//     // 网络操作
// protected:
//     void _AddSessionToMsgTrasfer(FS_Session *client);
//     void _StatisticsMsgPerSecond();
//     #pragma endregion

    /* 内部方法 */
    #pragma region inner api
private:
    Int32 _ReadConfig();
    Int32 _CreateNetModules();
    Int32 _StartModules();

    Int32 _BeforeStart();
    Int32 _OnStart();
    Int32 _AfterStart();
    #pragma endregion

private:
    IFS_ServerConfigMgr *_serverConfigMgr;          // 服务器配置
    IFS_Connector *_connector;                      // 连接器
    IFS_MsgTransfer *_msgTransfer;                  // 消息传输器
    IFS_MsgHandler *_msgHandler;                    // 消息处理器
    FS_SessionMgr *_sessiomMgr;                     // 会话管理
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_ServerCoreImpl.h"

#endif
