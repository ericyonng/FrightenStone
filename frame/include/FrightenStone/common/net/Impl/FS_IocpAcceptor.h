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
struct AcceptorCfgs;
class FS_IocpPoller;
class IFS_NetEngine;

class BASE_EXPORT FS_IocpAcceptor : public IFS_Acceptor
{
public:
    FS_IocpAcceptor(UInt32 compId
                    , Locker &sessionLocker
                    ,Int32 &curSessionCnt
                    ,Int32 &maxSessionQuantityLimit
                    ,UInt64 &curMaxSessionId
                    , IFS_NetEngine *netEngine);
    virtual ~FS_IocpAcceptor();

public:    
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &totalCfgs);
    virtual Int32 Start();
    virtual void AfterStart();
    virtual void WillClose();
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose();


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
private:
    virtual void OnDisconnected(UInt64 sessionId);

    /* 数据成员 */
    #pragma region data member
private:
    // 配置
    AcceptorCfgs *_cfgs;
    FS_IocpPoller *_poller;
    // 监听端口套接字
    SOCKET _sock;
    bool _isInit;

    // 客户端连接上限
    Locker &_locker;
    Int32 &_curSessionCnt;
    Int32 &_maxSessionQuantityLimit;
    UInt64 &_curMaxSessionId;
#pragma endregion
};

FS_NAMESPACE_END

#endif

#endif
