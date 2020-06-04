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
 * @file  : MessageQueue.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/memorypool/memorypool.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

struct FS_MessageBlock;
class FS_ThreadPool;
class ITask;

// 消息队列遵循FIFO原则，单一生产者对单一消费者
class BASE_EXPORT MessageQueue
{
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
    // msgs堆创建
    bool Push(std::list<FS_MessageBlock *> *&msgs);
    bool Push(FS_MessageBlock *msg);    // 需要额外调用notify
    void Notify();
    void PushUnlock();

    // 其他线程等待消息到来并从前节点弹出
    void PopLock();
    // 成功返回超时WaitEventTimeOut或者成功Success exportMsgsOut 必须是堆创建
    Int32 WaitForPoping(std::list<FS_MessageBlock *> *&exportMsgsOut, UInt64 timeoutMilisec = INFINITE);
    void PopImmediately(std::list<FS_MessageBlock *> *&exportMsgsOut);
    bool IsQueueInHandling();
    bool HasMsgToConsume();
    bool IsWorking() const;
    void PopUnlock();

private:
    void _MsgQueueWaiterThread(FS_ThreadPool *pool);

private:
    ConditionLocker _msgGeneratorGuard;
    std::atomic_bool _msgGeneratorChange;
    std::list<FS_MessageBlock *> *_msgGeneratorQueue;
    std::list<FS_MessageBlock *> *_msgSwitchQueue;

    ConditionLocker _msgConsumerGuard;
    std::atomic_bool _msgConsumerQueueChange;
    std::list<FS_MessageBlock *> *_msgConsumerQueue;

    std::atomic_bool _isWorking;
    FS_ThreadPool *_pool;
    std::atomic_bool _isStart;
};

// 建议使用多生产者单一消费者模型
// 多生产者对多消费者并发型消息队列 生产者与消费者数量必须呈倍数关系，以便均衡的分配消息 生产者需大于消费者
class BASE_EXPORT ConcurrentMessageQueue
{
public:
    ConcurrentMessageQueue(UInt32 generatorQuantity, UInt32 consumerQuantity = 1);
    ~ConcurrentMessageQueue();

public:
    Int32 BeforeStart();
    Int32 Start();
    void BeforeClose();
    void Close();
    bool IsWorking() const;
    bool IsQueueInHandling(UInt32 consumerQueueId) const;

public:
    // 压入末节点
    void PushLock(UInt32 generatorQueueId);
    bool Push(UInt32 generatorQueueId, std::list<FS_MessageBlock *> *&msgs);
    bool Push(UInt32 generatorQueueId, FS_MessageBlock *messageBlock);
    void Notify(UInt32 generatorQueueId);
    void PushUnlock(UInt32 generatorQueueId);

    // 其他线程等待消息到来并从前节点弹出
    void PopLock(UInt32 consumerQueueId);
    // 成功返回超时WaitEventTimeOut或者成功Success  exportMsgsOut 必须是堆创建
    Int32 WaitForPoping(UInt32 consumerQueueId, std::list<FS_MessageBlock *> *&exportMsgsOut, ULong timeoutMilisec = INFINITE);
    void NotifyPop(UInt32 consumerQueueId);
    void PopImmediately(UInt32 consumerQueueId, std::list<FS_MessageBlock *> *&exportMsgsOut);
    void PopUnlock(UInt32 consumerQueueId);
    bool HasMsgToConsume(UInt32 consumerQueueId) const;

private:
    void _Generator2ConsumerQueueTask(ITask *task, FS_ThreadPool *pool);

private:
    /* 生产者 */
    std::vector<ConditionLocker *> _genoratorGuards;
    std::vector<std::atomic_bool *> _generatorChange;
    std::vector<std::list<FS_MessageBlock *> *> _generatorMsgQueues;
    std::vector<std::list<FS_MessageBlock *> *> _msgSwitchQueues;

    /* 消费者参数 */
    std::vector<ConditionLocker *>  _consumerGuards;
    std::vector<std::atomic_bool *> _msgConsumerQueueChanges;
    std::vector<std::list<FS_MessageBlock *> *> _consumerMsgQueues;

    /* 系统参数 */
    std::atomic<UInt32> _generatorQuantity;
    std::atomic<UInt32> _consumerQuantity;
    std::atomic_bool _isWorking;
    std::atomic_bool _isStart;
    FS_ThreadPool *_pool;
};

// 任意组合多对多
// 每个通过订阅主题方式产生generatorId与consumerid映射
// 通过消息类型推算出所在的generatorId与consumerId这样

// 单向无线程并发消息队列
class BASE_EXPORT ConcurrentMessageQueueNoThread
{
    OBJ_POOL_CREATE_ANCESTOR(ConcurrentMessageQueueNoThread);
public:
    ConcurrentMessageQueueNoThread(UInt32 generatorQuantity, UInt32 consumerQuantity = 1);
    ~ConcurrentMessageQueueNoThread();

public:
    Int32 BeforeStart();
    Int32 Start();
    void BeforeClose();
    void Close();
    bool IsWorking() const;
    UInt32 GetGeneratorQuality() const;
    UInt32 GenerateGeneratorId();       // generatorid不可超过或等于 generatorQuantity
    UInt32 GenerateConsumerId();        // ConsumerId 不可超过或等于 consumerQuantity
    bool CanGenerateGeneratiroId() const;
    bool CanGenerateConsumerId() const;

public:
    bool Push(UInt32 generatorQueueId, std::list<FS_MessageBlock *> *&msgs);
    bool PushByConsumerId(UInt32 generatorQueueId, UInt32 consumerId, std::list<FS_MessageBlock *> *&msgs);
    bool Push(UInt32 generatorQueueId, FS_MessageBlock *messageBlock);
    bool PushByConsumerId(UInt32 generatorQueueId, UInt32 consumerId, FS_MessageBlock *messageBlock);
    void NotifyConsumerByGenerator(UInt32 generatorQueueId);

    // 其他线程等待消息到来并从前节点弹出
    // 成功返回超时WaitEventTimeOut或者成功Success  generatorMsgs 的索引是generatorid 且vector中的list需要预先堆创建,以便采用swap方式快速拷贝
    // generatorMsgs 的索引是generatorid 且vector中的list需要预先堆创建,以便采用swap方式快速拷贝
    Int32 WaitForPoping(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs, ULong timeoutMilisec = INFINITE);
    void PopImmediately(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs);
    void NotifyConsumer(UInt32 consumerQueueId);
    bool HasMsgToConsume(UInt32 consumerQueueId) const;

private:
    UInt32 _GetConsumerIdByGeneratorId(UInt32 generatorId) const;
    void _PopImmediately(std::vector<std::list<FS_MessageBlock *> *> *consumerQueue, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool isConsumerQueueChange);

private:
    /* 生产者消费者参数 */
    std::vector<ConditionLocker *>  _generatorGuards;   // 操作数据
    std::vector<std::atomic_bool *> _msgGeneratorMsgQueueChanges;   // 生产者有消息未被消费者消费
    std::vector<ConditionLocker *> _consumerGuards;     // 用于唤醒消费者
    std::vector<std::atomic_bool *> _msgConsumerQueueChanges;
    // 第一次容器使用consumerid索引,第2层vector通过generatorid索引,这个时候由于消费者生产者线程处于不同线程需要加锁,再一层的list是真正的generator的消息队列
    // 消息加入之后需要通过consumerguards唤醒消费者线程
    std::vector<std::vector<std::list<FS_MessageBlock *> *> *> _consumerMsgQueues;     // 第一层vector通过consumerid索引到对应的消费者队列,std::list<FS_MessageBlock *> *只是某个消费者塞进去的消息,避免了遍历list

    /* 系统参数 */
    ConditionLocker _sysLocker;
    std::atomic<UInt32> _consumerQuantity;
    std::atomic<UInt32> _generatorQuantity;
    std::atomic_bool _isWorking;
    std::atomic_bool _isStart;
    std::atomic<UInt32> _generatorMaxId;
    std::atomic<UInt32> _consumerMaxId;
};

// 只能单向,只能从生产者到消费者 无线程同步消息队列
class BASE_EXPORT MessageQueueNoThread
{
    OBJ_POOL_CREATE_ANCESTOR(MessageQueueNoThread);
public:
    MessageQueueNoThread();
    ~MessageQueueNoThread();

public:
    Int32 BeforeStart();
    Int32 Start();
    void BeforeClose();
    void Close();
    bool IsWorking() const;

public:
    bool Push(std::list<FS_MessageBlock *> *&msgs);
    bool Push(FS_MessageBlock *messageBlock);

    Int32 WaitForPoping(std::list<FS_MessageBlock *> *&generatorMsgs, ULong timeoutMilisec = INFINITE);
    void PopImmediately(std::list<FS_MessageBlock *> *&generatorMsgs);
    void Notify();
    bool HasMsgToConsume() const;

private:
    void _PopImmediately(std::list<FS_MessageBlock *> *&generatorMsgs);

private:
    /* 生产者消费者参数 */
    ConditionLocker _consumerGuard;     // 用于唤醒消费者
    std::atomic_bool _msgConsumerQueueChange;
    std::list<FS_MessageBlock *> *_consumerMsgQueue;

    /* 系统参数 */
    std::atomic_bool _isWorking;
    std::atomic_bool _isStart;
};


FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/MessageQueue/Impl/MessageQueueImpl.h"

#endif
