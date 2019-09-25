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
 * @file  : FS_MainIocpServer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/8
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_FS_MainIocpServer_H__
#define __Base_Common_Net_Impl_FS_MainIocpServer_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/net/Impl/FS_TcpServer.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/asyn/asyn.h"
#include "base/common/objpool/objpool.h"

// 主服务器单线程监听，多线程收发
FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;

class BASE_EXPORT FS_MainIocpServer : public FS_TcpServer
{
    OBJ_POOL_CREATE(FS_MainIocpServer, _objPoolHelper);
public:
    FS_MainIocpServer();
    virtual ~FS_MainIocpServer();

    /* 启动与关闭服务器 */
    #pragma region start close
    /*
    *   起服关服步骤：InitSocket->Bind->Listen->Start->Close
    */
public:
    void Start(Int32 msgTransferServerCnt);
    virtual void BeforeClose();
    #pragma endregion

    /* 监听客户端连入 */
    #pragma region listen net handle
    /*
    *   brief:
    *       1. - _OnNetMonitorTask 监听客户端连入线程处理
    *       2. - _IocpAccept - 接受客户端连入
    */
protected:
    virtual void _OnNetMonitorTask(const FS_ThreadPool *pool);
    SOCKET _OnIocpAccept(SOCKET sock);
    #pragma endregion

private:
    Locker _mainLocker;
    IDelegate<void> *_closeIocpDelegate;    // 委托执行关服时退出iocp
};

FS_NAMESPACE_END

#include "base/common/net/Impl/FS_MainIocpServerImpl.h"

#endif
