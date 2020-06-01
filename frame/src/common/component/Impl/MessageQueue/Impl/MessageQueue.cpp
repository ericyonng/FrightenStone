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
 * @file  : MessageQueue.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/component/Impl/MessageQueue/Impl/MessageQueue.h"
#include "FrightenStone/common/component/Impl/MessageQueue/Defs/MessageQueueTask.h"
#include "FrightenStone/common/component/Impl/MessageQueue/Defs/FS_MessageBlock.h"

#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_ThreadPool.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/component/Impl/Task/Task.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"

FS_NAMESPACE_BEGIN

OBJ_POOL_CREATE_DEF_IMPL(ConcurrentMessageQueueNoThread, 32);
OBJ_POOL_CREATE_DEF_IMPL(MessageQueueNoThread, 32);

MessageQueue::MessageQueue()
    :_msgGeneratorChange{false}
    ,_msgConsumerQueueChange(false)
    ,_pool(NULL)
    ,_isWorking{false}
    ,_msgGeneratorQueue(NULL)
    ,_msgSwitchQueue(NULL)
    ,_msgConsumerQueue(NULL)
    ,_isStart(false)
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
    if(_isStart)
        return StatusDefs::Success;

    _msgGeneratorQueue = new std::list<FS_MessageBlock *>;
    _msgSwitchQueue = new std::list<FS_MessageBlock *>;
    _msgConsumerQueue = new std::list<FS_MessageBlock *>;
    _pool = new FS_ThreadPool(0, 1);
    return StatusDefs::Success;
}

Int32 MessageQueue::Start()
{
    if(_isStart)
        return StatusDefs::Success;

    auto task = DelegatePlusFactory::Create(this, &MessageQueue::_MsgQueueWaiterThread);
    if(!_pool->AddTask(task, true))
    {
        Fs_SafeFree(task);
        g_Log->e<MessageQueue>(_LOGFMT_("message queue add task fail"));
        return StatusDefs::Error;
    }

    _isStart = true;
    return StatusDefs::Success;
}

void MessageQueue::BeforeClose()
{
    if(!_isStart)
        return;

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
    if(!_isStart)
        return;

    STLUtil::DelListContainer(*_msgGeneratorQueue);
    STLUtil::DelListContainer(*_msgSwitchQueue);
    STLUtil::DelListContainer(*_msgConsumerQueue);
    Fs_SafeFree(_msgGeneratorQueue);
    Fs_SafeFree(_msgSwitchQueue);
    Fs_SafeFree(_msgConsumerQueue);

    _isStart = false;
}

void MessageQueue::_MsgQueueWaiterThread(FS_ThreadPool *pool)
{
    _isWorking = true;
    while(pool->IsPoolWorking()|| _msgGeneratorChange)
    {
        if(_isWorking)
        {
            _msgGeneratorGuard.Lock();
            _msgGeneratorGuard.DeadWait();
            _msgGeneratorGuard.Unlock();
        }

        // 生产者消息转移到缓存
        _msgGeneratorGuard.Lock();
        auto genTemp = _msgGeneratorQueue;
        _msgGeneratorQueue = _msgSwitchQueue;
        _msgSwitchQueue = genTemp;
        _msgGeneratorChange = false;
        _msgGeneratorGuard.Unlock();

        _msgConsumerGuard.Lock();
        for(auto iterMsgBlock = _msgSwitchQueue->begin(); iterMsgBlock != _msgSwitchQueue->end();)
        {
            _msgConsumerQueue->push_back(*iterMsgBlock);
            iterMsgBlock = _msgSwitchQueue->erase(iterMsgBlock);
        }
        _msgConsumerQueueChange = true;
        _msgConsumerGuard.Sinal();
        _msgConsumerGuard.Unlock();
    }

//     _msgConsumerGuard.Lock();
//     _msgConsumerGuard.ResetSinal();
//     _msgConsumerGuard.Unlock();
    g_Log->sys<MessageQueue>(_LOGFMT_("_MsgQueueWaiterThread quit threadId[%llu]"), SystemUtil::GetCurrentThreadId());

    _isWorking = false;
}

/////// 多生产者多消费者

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

        SystemUtil::Sleep(0);
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

    _genoratorGuards.clear();
    _generatorChange.clear();
    _generatorMsgQueues.clear();
    _msgSwitchQueues.clear();
    _consumerGuards.clear();
    _msgConsumerQueueChanges.clear();
    _consumerMsgQueues.clear();

    // 打印未处理的消费者消息队列id
    for(auto queueId : hasMsgQueueId)
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("consumer queueId[%u] has msgs unhandled"), queueId);

    _isStart = false;
}

void ConcurrentMessageQueue::_Generator2ConsumerQueueTask(ITask *task, FS_ThreadPool *pool)
{
    MessageQueueTask *messageQueueTask = reinterpret_cast<MessageQueueTask *>(task);
    const UInt32 generatorQueueId = messageQueueTask->GetQueueId();

    // 参数
    _isWorking = true;
    const UInt32 consumerId = generatorQueueId % _consumerQuantity;
    ConditionLocker *localGenGuard = _genoratorGuards[generatorQueueId];
    auto &genChange = *_generatorChange[generatorQueueId];
    ConditionLocker *localConsumerGuard = _consumerGuards[consumerId];
    auto &consumerChange = *_msgConsumerQueueChanges[consumerId];

    std::list<FS_MessageBlock *> *genTemp = NULL;
    auto &consumerMsgQueue = _consumerMsgQueues[consumerId];

    // 均匀分配
    while(pool->IsPoolWorking() || *_generatorChange[generatorQueueId])
    {
        if(_isWorking)
        {
            localGenGuard->Lock();
            localGenGuard->DeadWait();
            localGenGuard->Unlock();
        }

        // 生产者消息转移到缓存
        localGenGuard->Lock();
        auto &localGenMsgQueue = _generatorMsgQueues[generatorQueueId];
        auto &localSwitchQueue = _msgSwitchQueues[generatorQueueId];
        genTemp = localGenMsgQueue;
        localGenMsgQueue = localSwitchQueue;
        localSwitchQueue = genTemp;
        genChange = false;
        localGenGuard->Unlock();

        localConsumerGuard->Lock();
        for(auto iterMsgBlock = localSwitchQueue->begin(); 
            iterMsgBlock != localSwitchQueue->end();)
        {
            consumerMsgQueue->push_back(*iterMsgBlock);
            iterMsgBlock = localSwitchQueue->erase(iterMsgBlock);
        }

        consumerChange = true;
        localConsumerGuard->Sinal();
        localConsumerGuard->Unlock();
    }

//     _consumerGuards[consumerId]->Lock();
//     _consumerGuards[consumerId]->ResetSinal();
//     _consumerGuards[consumerId]->Unlock();
    g_Log->sys<ConcurrentMessageQueue>(_LOGFMT_("_Generator2ConsumerQueueTask quit threadId[%llu]"), SystemUtil::GetCurrentThreadId());
    _isWorking = false;
}


Int32 ConcurrentMessageQueueNoThread::BeforeStart()
{
    if(_isStart)
    {
        g_Log->w<ConcurrentMessageQueueNoThread>(_LOGFMT_("message queue is already started"));
        return StatusDefs::Success;
    }

    _generatorGuards.resize(_generatorQuantity);
    _consumerGuards.resize(_consumerQuantity);
    _msgConsumerQueueChanges.resize(_consumerQuantity);
    _consumerMsgQueues.resize(_consumerQuantity);
    _msgGeneratorMsgQueueChanges.resize(_generatorQuantity);

    for(UInt32 j = 0; j < _generatorQuantity; ++j)
    {
        auto newQueueChange = new std::atomic_bool;
        *newQueueChange = false;
        _msgGeneratorMsgQueueChanges[j] = newQueueChange;
        _generatorGuards[j] = new ConditionLocker;
    }

    for(UInt32 i = 0; i < _consumerQuantity; ++i)
    {
        _consumerGuards[i] = new ConditionLocker;
        _msgConsumerQueueChanges[i] = new std::atomic_bool;
        *_msgConsumerQueueChanges[i] = false;
        auto newConsumerMsgQueue = new std::vector<std::list<FS_MessageBlock *> *>;
        _consumerMsgQueues[i] = newConsumerMsgQueue;
        newConsumerMsgQueue->resize(_generatorQuantity);
        for(UInt32 j = 0; j < _generatorQuantity; ++j)
            (*newConsumerMsgQueue)[j] = new std::list<FS_MessageBlock *>;
    }

    return StatusDefs::Success;
}

void ConcurrentMessageQueueNoThread::BeforeClose()
{
    if(!_isStart)
        return;

    // 唤醒生产者线程
    _isWorking = false;

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

        SystemUtil::Sleep(0);
    }
}

void ConcurrentMessageQueueNoThread::Close()
{
    if(!_isStart)
        return;

    // 释放生产者队列资源
    std::set<UInt32> hasMsgQueueId;
    STLUtil::DelVectorContainer(_generatorGuards);
    STLUtil::DelVectorContainer(_msgGeneratorMsgQueueChanges);

    hasMsgQueueId.clear();
    for(UInt32 i = 0; i < _consumerQuantity; ++i)
    {
        auto consumerMsgQueue = _consumerMsgQueues[i];
        for(UInt32 j = 0; j < _generatorQuantity; ++j)
        {
            auto generatorMsgQueue = (*consumerMsgQueue)[j];
            if(!generatorMsgQueue->empty())
            {
                STLUtil::DelListContainer(*generatorMsgQueue);
                hasMsgQueueId.insert(j);
            }
            Fs_SafeFree(generatorMsgQueue);
        }
        Fs_SafeFree(consumerMsgQueue);
        Fs_SafeFree(_msgConsumerQueueChanges[i]);
        Fs_SafeFree(_consumerGuards[i]);
    }
    _consumerMsgQueues.clear();
    _msgConsumerQueueChanges.clear();
    _consumerGuards.clear();
    _generatorGuards.clear();

    // 打印未处理的消费者消息队列id
    for(auto queueId : hasMsgQueueId)
        g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("consumer queueId[%u] has msgs unhandled"), queueId);

    _isStart = false;
}

Int32 MessageQueueNoThread::BeforeStart()
{
    if(_isStart)
        return StatusDefs::Success;

    _consumerMsgQueue = new std::list<FS_MessageBlock *>;
    return StatusDefs::Success;
}

Int32 MessageQueueNoThread::Start()
{
    if(_isStart)
        return StatusDefs::Success;

    _isStart = true;
    _isWorking = true;
    return StatusDefs::Success;
}

void MessageQueueNoThread::BeforeClose()
{
    if(!_isStart)
        return;

    // 唤醒生产者线程
    _isWorking = false;

    // 唤醒消费者线程
    _consumerGuard.Broadcast();
}

void MessageQueueNoThread::Close()
{
    if(!_isStart)
        return;

    // 释放生产者队列资源
    auto unhandleBlock = _consumerMsgQueue->size();
    STLUtil::DelListContainer(*_consumerMsgQueue);
    Fs_SafeFree(_consumerMsgQueue);

    // 打印未处理的消费者消息队列id
    g_Log->w<ConcurrentMessageQueue>(_LOGFMT_("consumer has [%llu] msgs unhandled"), unhandleBlock);

    _isStart = false;
}

FS_NAMESPACE_END

