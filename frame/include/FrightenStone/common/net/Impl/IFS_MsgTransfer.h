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
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgTransfer_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgTransfer_H__

#pragma once
#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/net/Impl/IFS_EngineComp.h"

FS_NAMESPACE_BEGIN

class  IFS_Session;
struct NetMsg_DataHeader;
class ConcurrentMessageQueueNoThread;
class MessageQueueNoThread;
struct BriefSessionInfo;
struct TransferCfgs;

class BASE_EXPORT IFS_MsgTransfer : public IFS_EngineComp
{
public:
    IFS_MsgTransfer(UInt32 compId, IFS_NetEngine *engine);
    virtual ~IFS_MsgTransfer();

    /* 网络事件 */
public:
    virtual bool OnWillConnect(BriefSessionInfo *newSessionInfo) = 0;
    virtual void OnPostConnectOpFinish() = 0;

    /* 属性方法 */
public:
    virtual Int32 GetSessionCnt() = 0;
    // 获取生产者id
    virtual UInt32 GetGeneratorId() const;
    // 获取消费者id
    virtual UInt32 GetConsumerId() const;
    // 并发消息队列参数绑定
    virtual void BindConcurrentParams(UInt32 generatorId, UInt32 consumerId, ConcurrentMessageQueueNoThread *concurrentMq);
    virtual bool IsIoFinish() const { return true; }
protected:
    UInt32 _generatorId;            // engine中concurrentmq指定的生产者id
    UInt32 _consumerId;             // 指定的，并发消息队列中对应的消费者id
    ConcurrentMessageQueueNoThread *_concurrentMq;  // 单向的消息队列,当组件是生产者时只能push不能pop,只能generatiorId
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_MsgTransferImpl.h"

#endif

