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
 * @file  : MessageQueue.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/MessageQueue/Impl/MessageQueue.h"
#include "base/common/component/Impl/MessageQueue/Defs/MessageQueueTask.h"
#include "base/common/component/Impl/MessageQueue/Defs/FS_MessageBlock.h"

#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/log/Log.h"
#include "base/common/component/Impl/Task/Task.h"
#include "base/common/assist/utils/Impl/STLUtil.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(MessageQueue, __DEF_OBJ_POOL_OBJ_NUM__, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);

MessageQueue::MessageQueue()
    :_msgGeneratorChange{false}
    ,_msgConsumerQueueChange(false)
    ,_pool(NULL)
    ,_isWorking{false}
{
}

MessageQueue::~MessageQueue()
{
    if(_isWorking)
    {
        BeforeClose();
        Close();
    }

    Fs_SafeFree(_pool);
}

Int32 MessageQueue::BeforeStart()
{
    _pool = new FS_ThreadPool(0, 1);
    return StatusDefs::Success;
}

Int32 MessageQueue::Start()
{
    auto task = DelegatePlusFactory::Create(this, &MessageQueue::_MsgQueueWaiterThread);
    if(!_pool->AddTask(task, true))
    {
        Fs_SafeFree(task);
        g_Log->e<MessageQueue>(_LOGFMT_("message queue add task fail"));
        return StatusDefs::Error;
    }

    return StatusDefs::Success;
}

void MessageQueue::BeforeClose()
{
    // 唤醒消费者线程
    _msgGeneratorGuard.Lock();
    _isWorking = false;
    _msgGeneratorGuard.Sinal();
    _msgGeneratorGuard.Unlock();

    if(_pool)
        _pool->Close();

    _msgConsumerGuard.Broadcast();
}

void MessageQueue::Close()
{

}

void MessageQueue::_MsgQueueWaiterThread(FS_ThreadPool *pool)
{
    _isWorking = true;
    while(pool->IsPoolWorking()|| _msgGeneratorChange)
    {
        if(_isWorking)
        {
            _msgGeneratorGuard.Lock();
            _msgGeneratorGuard.Wait();
            _msgGeneratorGuard.Unlock();
        }

        // 生产者消息转移到缓存
        _msgGeneratorGuard.Lock();
        for(auto iterMsgBlock = _msgGeneratorQueue.begin(); iterMsgBlock != _msgGeneratorQueue.end();)
        {
            _msgSwitchQueue.push_back(*iterMsgBlock);
            iterMsgBlock = _msgGeneratorQueue.erase(iterMsgBlock);
        }
        _msgGeneratorChange = false;
        _msgGeneratorGuard.Unlock();

        _msgConsumerGuard.Lock();
        for(auto iterMsgBlock = _msgSwitchQueue.begin(); iterMsgBlock != _msgSwitchQueue.end();)
        {
            _msgConsumerQueue.push_back(*iterMsgBlock);
            iterMsgBlock = _msgSwitchQueue.erase(iterMsgBlock);
        }
        _msgConsumerQueueChange = true;
        _msgConsumerGuard.Sinal();
        _msgConsumerGuard.Unlock();
    }

    _msgConsumerGuard.Lock();
    _msgConsumerGuard.ResetSinal();
    _msgConsumerGuard.Unlock();

    _isWorking = false;
}

/////// 多生产者多消费者

OBJ_POOL_CREATE_DEF_IMPL(ConcurrentMessageQueue, __DEF_OBJ_POOL_OBJ_NUM__, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);

ConcurrentMessageQueue::ConcurrentMessageQueue(UInt32 generatorQuantity, UInt32 consumerQuantity)
    :_generatorQuantity{generatorQuantity}
    ,_consumerQuantity{consumerQuantity}
    ,_isWorking{false}
    ,_pool(NULL)
    ,_isStart{false}
{
}

ConcurrentMessageQueue::~ConcurrentMessageQueue()
{
    BeforeClose();
    Close();
}

Int32 ConcurrentMessageQueue::BeforeStart()
{
    if(_isStart)
    {
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("message queue is already started"));
        return StatusDefs::Success;
    }

    _genoratorGuards.resize(_generatorQuantity);
    _generatorChange.resize(_generatorQuantity);
    _generatorMsgQueues.resize(_generatorQuantity);
    _msgSwitchQueues.resize(_generatorQuantity);

    for(UInt32 i = 0; i < _generatorQuantity; ++i)
    {
        _genoratorGuards[i] = new ConditionLocker;
        _generatorChange[i] = new std::atomic_bool;
        *_generatorChange[i] = false;
        _generatorMsgQueues[i] = new std::list<FS_MessageBlock *>;
        _msgSwitchQueues[i] = new std::list<FS_MessageBlock *>;
    }

    _consumerGuards.resize(_consumerQuantity);
    _msgConsumerQueueChanges.resize(_consumerQuantity);
    _consumerMsgQueues.resize(_consumerQuantity);
    for(UInt32 i = 0; i < _consumerQuantity; ++i)
    {
        _consumerGuards[i] = new ConditionLocker;
        _msgConsumerQueueChanges[i] = new std::atomic_bool;
        *_msgConsumerQueueChanges[i] = false;
        _consumerMsgQueues[i] = new std::list<FS_MessageBlock *>;
    }

    _pool = new FS_ThreadPool(0, _generatorQuantity);
    
    return StatusDefs::Success;
}

Int32 ConcurrentMessageQueue::Start()
{
    if(_isStart)
    {
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("message queue is already started"));
        return StatusDefs::Success;
    }

    for(UInt32 i = 0; i < _generatorQuantity; ++i)
    {
        auto newDelegate = DelegatePlusFactory::Create(this, &ConcurrentMessageQueue::_Generator2ConsumerQueueTask);
        ITask *newTask = reinterpret_cast<ITask *>(new MessageQueueTask(i, _pool, newDelegate));
        _pool->AddTask(*newTask, true);
    }

    _isStart = true;
    return StatusDefs::Success;
}

void ConcurrentMessageQueue::BeforeClose()
{
    if(!_isStart)
        return;

    // 唤醒生产者线程
    _isWorking = false;
    for(UInt32 i = 0; i < _generatorQuantity; ++i)
    {
        _genoratorGuards[i]->Lock();
        _genoratorGuards[i]->Sinal();
        _genoratorGuards[i]->Unlock();
    }

    if(_pool)
        _pool->Close();

    // 唤醒消费者线程
    while(true)
    {
        bool hasWaiter = false;
        for(UInt32 i = 0; i < _consumerQuantity; ++i)
        {
            _consumerGuards[i]->Lock();
            if(_consumerGuards[i]->HasWaiter())
            {
                hasWaiter = true;
                _consumerGuards[i]->Sinal();
            }
            _consumerGuards[i]->Unlock();
        }

        if(!hasWaiter)
            break;

        Sleep(0);
    }
}

void ConcurrentMessageQueue::Close()
{
    if(!_isStart)
        return;

    // 释放生产者队列资源
    std::set<UInt32> hasMsgQueueId;
    for(UInt32 i = 0; i < _generatorQuantity; ++i)
    {
        if(!_generatorMsgQueues[i]->empty())
            hasMsgQueueId.insert(i);

        STLUtil::DelListContainer(*_generatorMsgQueues[i]);
        Fs_SafeFree(_generatorMsgQueues[i]);

        if(!_msgSwitchQueues[i]->empty())
            hasMsgQueueId.insert(i);

        STLUtil::DelListContainer(*_msgSwitchQueues[i]);
        Fs_SafeFree(_msgSwitchQueues[i]);
        Fs_SafeFree(_generatorChange[i]);
        Fs_SafeFree(_genoratorGuards[i]);
    }

    // 打印未处理的生产者消息队列id
    for(auto queueId : hasMsgQueueId)
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("generator queueId[%u] has msgs unhandled"), queueId);

    hasMsgQueueId.clear();
    for(UInt32 i = 0; i < _consumerQuantity; ++i)
    {
        if(!_consumerMsgQueues[i]->empty())
            hasMsgQueueId.insert(i);

        STLUtil::DelListContainer(*_consumerMsgQueues[i]);
        Fs_SafeFree(_consumerMsgQueues[i]);
        Fs_SafeFree(_msgConsumerQueueChanges[i]);
        Fs_SafeFree(_consumerGuards[i]);
    }

    // 打印未处理的消费者消息队列id
    for(auto queueId : hasMsgQueueId)
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("consumer queueId[%u] has msgs unhandled"), queueId);

    _isStart = false;
}

void ConcurrentMessageQueue::_Generator2ConsumerQueueTask(ITask *task, FS_ThreadPool *pool)
{
    MessageQueueTask *messageQueueTask = reinterpret_cast<MessageQueueTask *>(task);
    const UInt32 generatorQueueId = messageQueueTask->GetQueueId();

    _isWorking = true;
    std::list<FS_MessageBlock *> *genTemp = NULL;

    // 均匀分配
    const UInt32 consumerId = generatorQueueId % _consumerQuantity;
    while(pool->IsPoolWorking() || *_generatorChange[generatorQueueId])
    {
        if(_isWorking)
        {
            _genoratorGuards[generatorQueueId]->Lock();
            _genoratorGuards[generatorQueueId]->Wait();
            _genoratorGuards[generatorQueueId]->Unlock();
        }

        // 生产者消息转移到缓存
        _genoratorGuards[generatorQueueId]->Lock();
        genTemp = _generatorMsgQueues[generatorQueueId];
        _generatorMsgQueues[generatorQueueId] = _msgSwitchQueues[generatorQueueId];
        _msgSwitchQueues[generatorQueueId] = genTemp;
        *_generatorChange[generatorQueueId] = false;
        _genoratorGuards[generatorQueueId]->Unlock();

        _consumerGuards[consumerId]->Lock();
        for(auto iterMsgBlock = _msgSwitchQueues[generatorQueueId]->begin(); 
            iterMsgBlock != _msgSwitchQueues[generatorQueueId]->end();)
        {
            _consumerMsgQueues[consumerId]->push_back(*iterMsgBlock);
            iterMsgBlock = _msgSwitchQueues[generatorQueueId]->erase(iterMsgBlock);
        }

        *_msgConsumerQueueChanges[consumerId] = true;
        _consumerGuards[consumerId]->Sinal();
        _consumerGuards[consumerId]->Unlock();
    }

    _consumerGuards[consumerId]->Lock();
    _consumerGuards[consumerId]->ResetSinal();
    _consumerGuards[consumerId]->Unlock();

    _isWorking = false;
}

FS_NAMESPACE_END

