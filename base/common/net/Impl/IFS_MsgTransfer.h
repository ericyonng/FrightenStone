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
 * @file  : IFS_MsgTransfer.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/07
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Net_Impl_IFS_MsgTransfer_H__
#define __Base_Common_Net_Impl_IFS_MsgTransfer_H__

#pragma once
#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT IFS_Session;
struct BASE_EXPORT NetMsg_DataHeader;

class BASE_EXPORT IFS_MsgTransfer
{
public:
    IFS_MsgTransfer();
    virtual ~IFS_MsgTransfer();

public:
    virtual Int32 BeforeStart() { return StatusDefs::Success; }
    virtual Int32 Start() = 0;
    virtual Int32 AfterStart() { return StatusDefs::Success; }
    virtual void WillClose() {} // 断开与模块之间的依赖
    virtual void BeforeClose() {}   // 处理未决数据，初步的清理
    virtual void Close() = 0;
    virtual void AfterClose() {}

    virtual void OnConnect(IFS_Session *session) = 0;
    virtual void OnDestroy() = 0;
    virtual void OnHeartBeatTimeOut(IFS_Session *session) = 0;
    // msg内存池创建
    virtual void AsynSend(UInt64 sessionId, NetMsg_DataHeader *msg) = 0;


    // 会话个数
    virtual void RegisterDisconnected(IDelegate<void, IFS_Session *> *callback) = 0;
    virtual void RegisterRecvSucCallback(IDelegate<void, IFS_Session *, Int64> *callback) = 0;
    virtual void RegisterRecvAmountCallback(IDelegate<void, IFS_Session *> *callback) = 0;
    virtual void RegisterSendSucCallback(IDelegate<void, IFS_Session *, Int64> *callback) = 0;
    virtual void RegisterHeatBeatTimeOutCallback(IDelegate<void, IFS_Session *> *callback) = 0;
    virtual Int32 GetSessionCnt() = 0;
};

FS_NAMESPACE_END

#include "base/common/net/Impl/IFS_MsgTransferImpl.h"

#endif

