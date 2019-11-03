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
 * @file  : MessageQueue.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__
#define __Base_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/asyn/asyn.h"
#include "base/common/objpool/objpool.h"
#include "base/common/status/status.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT FS_MessageBlock;
class BASE_EXPORT FS_ThreadPool;

// 消息队列遵循FIFO原则，接收端取一条处理一条
class BASE_EXPORT MessageQueue
{
    OBJ_POOL_CREATE_DEF(MessageQueue);

public:
    MessageQueue();
    virtual ~MessageQueue();

public:
    Int32 BeforeStart();
    Int32 Start();
    void BeforeClose();
    void Close();

public:
    // 压入末节点
    void PushLock();
    bool Push(std::list<FS_MessageBlock *> &msgs);
    void PushUnlock();

    // 其他线程等待消息到来并从前节点弹出
    void PopLock();
    // 成功返回超时WaitEventTimeOut或者成功Success
    Int32 WaitForPoping(std::list<FS_MessageBlock *> &exportMsgsOut, ULong timeoutMilisec = INFINITE);
    void PopUnlock();

private:
    void _MsgQueueWaiterThread(FS_ThreadPool *pool);

private:
    ConditionLocker _msgGeneratorGuard;
    std::atomic_bool _msgGeneratorChange;
    std::list<FS_MessageBlock *> _msgGeneratorQueue;
    std::list<FS_MessageBlock *> _msgSwitchQueue;

    ConditionLocker _msgConsumerGuard;
    std::atomic_bool _msgComsumerQueueChange;
    std::list<FS_MessageBlock *> _msgComsumerQueue;

    std::atomic_bool _isWorking;
    FS_ThreadPool *_pool;
};

FS_NAMESPACE_END

#include "base/common/component/Impl/MessageQueue/Impl/MessageQueueImpl.h"

#endif
