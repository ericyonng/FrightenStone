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
 * @file  : FS_IocpAcceptor.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/08
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpAcceptor_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_FS_IocpAcceptor_H__

#pragma once


#ifdef _WIN32

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/net/Impl/IFS_Acceptor.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class IFS_Session;
class FS_IniFile;
class FS_Iocp;
struct IoDataBase;
struct BriefListenAddrInfo;

class BASE_EXPORT FS_IocpAcceptor : public IFS_Acceptor
{
public:
    FS_IocpAcceptor(Locker &sessionLocker
                    ,Int32 &curSessionCnt
                    ,Int32 &maxSessionQuantityLimit
                    ,UInt64 &curMaxSessionId
                    ,const UInt64 &maxSessionIdLimit, FS_NetEngine *netEngine);
    virtual ~FS_IocpAcceptor();

public:
    virtual Int32 BeforeStart();
    virtual Int32 Start();
    virtual void BeforeClose();
    virtual void Close();
    virtual void OnDisconnected(IFS_Session *session);
    // 监听地址
    virtual void SetListenAddrInfo(const BriefListenAddrInfo &listenAddrInfo);
    /* TCP 常规操作 */
    #pragma region tcp normal operate
    /*
    * brief:
    *       1. - InitSocket 初始化Socket等
    *       2. - Bind 绑定IP和端口号
    *       3. - Listen 监听端口号
    */
private:
    SOCKET _InitSocket();
    Int32 _Bind(const Byte8 *ip, UInt16 port);
    Int32 _Listen(Int32 unconnectQueueLen = SOMAXCONN);
    #pragma endregion

    /* 网络事件 */
    #pragma region net event
    /*
    * brief: 
    *       1. FS_Server 4 多个线程触发 不安全 如果只开启1个FS_Server就是安全的
    *       2. _OnNetMonitorTask 监听网络任务 OnRun(旧版) 建议多条线程去做monitor而不是单条线程，完成端口的get是线程安全的
    *       3. OnNetJoin 玩家加入 线程不安全
    *       4. OnNetLeave 玩家掉线 线程不安全
    *       5. OnNetMsg 玩家消息到来（消息是从FS_Server的_HandleNetMsg传入）线程不安全 NetMsg_DataHeader 转发到其他线程需要拷贝避免消息被覆盖
    *       6. OnNetRecv 接收到数据 线程不安全
    */
private:
    void _OnConnected(SOCKET sock, const sockaddr_in *addrInfo, FS_Iocp *iocpListener);
    void _OnIocpMonitorTask(FS_ThreadPool *threadPool);
    void _PreparePostAccept(FS_Iocp *listenIocp, char **&bufArray, IoDataBase **&ioDataArray);
    void _FreePrepareAcceptBuffers(char **&bufArray, IoDataBase **&ioDataArray);

    #pragma endregion

    /* 数据成员 */
    #pragma region data member
private:
    // 线程 
    FS_ThreadPool *_threadPool;
    SOCKET _sock;

    // 网络事件回调
    IDelegate<void> *_closeIocpDelegate;

    // 客户端连接上限
    Locker &_locker;
    Int32 &_curSessionCnt;
    Int32 &_maxSessionQuantityLimit;
    UInt64 &_curMaxSessionId;
    const UInt64 &_maxSessionIdLimit;
    std::set<UInt64> _sucConnectedSessionIds;

    BriefListenAddrInfo *_listenAddrInfo;

#pragma endregion
};

FS_NAMESPACE_END

#endif

#endif
