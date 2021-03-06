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
 * @file  : MessageQueueImpl.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/11/1
 * @brief :
 * 
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Impl_MessageQueue_H__

#pragma once

FS_NAMESPACE_BEGIN
inline void MessageQueue::PushLock()
{
    _msgGeneratorGuard.Lock();
}

inline bool MessageQueue::Push(std::list<FS_MessageBlock *> *&msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    if(_msgGeneratorQueue->empty())
    {
        std::list<FS_MessageBlock *> *gemTemp = NULL;
        gemTemp = msgs;
        msgs = _msgGeneratorQueue;
        _msgGeneratorQueue = gemTemp;
    }
    else
    {
        for(auto iterMsgBlock = msgs->begin(); iterMsgBlock != msgs->end();)
        {
            _msgGeneratorQueue->push_back(*iterMsgBlock);
            iterMsgBlock = msgs->erase(iterMsgBlock);
        }
    }

    _msgGeneratorChange = true;
    _msgGeneratorGuard.Sinal();
    return true;
}

inline bool MessageQueue::Push(FS_MessageBlock *msg)
{
    if(UNLIKELY(!_isWorking))
        return false;

    _msgGeneratorQueue->push_back(msg);
    _msgGeneratorChange = true;
    return true;
}

inline void MessageQueue::Notify()
{
    if(UNLIKELY(!_isWorking))
        return;

    _msgGeneratorChange = true;
     _msgGeneratorGuard.Sinal();
}

inline void MessageQueue::PushUnlock()
{
    _msgGeneratorGuard.Unlock();
}

inline void MessageQueue::PopLock()
{
    _msgConsumerGuard.Lock();
}

inline Int32 MessageQueue::WaitForPoping(std::list<FS_MessageBlock *> *&exportMsgsOut, UInt64 timeoutMilisec)
{
    Int32 st = _msgConsumerGuard.Wait(timeoutMilisec);
    if(_msgConsumerQueueChange)
    {
        auto temp = exportMsgsOut;
        exportMsgsOut = _msgConsumerQueue;
        _msgConsumerQueue = temp;
        _msgConsumerQueueChange = false;
    }

    return st;
}

inline void MessageQueue::PopImmediately(std::list<FS_MessageBlock *> *&exportMsgsOut)
{
    if(_msgConsumerQueueChange)
    {
        auto temp = exportMsgsOut;
        exportMsgsOut = _msgConsumerQueue;
        _msgConsumerQueue = temp;
        _msgConsumerQueueChange = false;
    }
}

inline bool MessageQueue::IsQueueInHandling()
{
    return HasMsgToConsume() || IsWorking();
}

inline bool MessageQueue::HasMsgToConsume()
{
    _msgConsumerGuard.Lock();
    bool isEmpty = _msgConsumerQueue->empty();
    _msgConsumerGuard.Unlock();
    return !isEmpty;
}

inline bool MessageQueue::IsWorking() const 
{
    return _isWorking;
}

inline void MessageQueue::PopUnlock()
{
    _msgConsumerGuard.Unlock();
}

inline bool ConcurrentMessageQueue::IsWorking() const
{
    return _isWorking;
}

inline bool ConcurrentMessageQueue::IsQueueInHandling(UInt32 consumerQueueId) const
{
    return HasMsgToConsume(consumerQueueId) || IsWorking();
}

inline void ConcurrentMessageQueue::PushLock(UInt32 generatorQueueId)
{
    _genoratorGuards[generatorQueueId]->Lock();
}

inline bool ConcurrentMessageQueue::Push(UInt32 generatorQueueId, std::list<FS_MessageBlock *> *&msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    if(_generatorMsgQueues[generatorQueueId]->empty())
    {
        std::list<FS_MessageBlock *> *gemTemp = NULL;
        gemTemp = msgs;
        msgs = _generatorMsgQueues[generatorQueueId];
        _generatorMsgQueues[generatorQueueId] = gemTemp;
    }
    else
    {
        auto &msgQueue = _generatorMsgQueues[generatorQueueId];
        for(auto iterMsgBlock = msgs->begin(); iterMsgBlock != msgs->end();)
        {
            msgQueue->push_back(*iterMsgBlock);
            iterMsgBlock = msgs->erase(iterMsgBlock);
        }
    }

    *_generatorChange[generatorQueueId] = true;
    _genoratorGuards[generatorQueueId]->Sinal();

    return true;
}

inline bool ConcurrentMessageQueue::Push(UInt32 generatorQueueId, FS_MessageBlock *messageBlock)
{
    if(!_isWorking)
        return false;

    _generatorMsgQueues[generatorQueueId]->push_back(messageBlock);
    *_generatorChange[generatorQueueId] = true;
    return true;
}

inline void ConcurrentMessageQueue::Notify(UInt32 generatorQueueId)
{
    if(!_isWorking)
        return;

    *_generatorChange[generatorQueueId] = true;
    _genoratorGuards[generatorQueueId]->Sinal();
}

inline void ConcurrentMessageQueue::PushUnlock(UInt32 generatorQueueId)
{
    _genoratorGuards[generatorQueueId]->Unlock();
}

inline void ConcurrentMessageQueue::PopLock(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Lock();
}

// 成功返回超时WaitEventTimeOut或者成功Success
inline Int32 ConcurrentMessageQueue::WaitForPoping(UInt32 consumerQueueId, std::list<FS_MessageBlock *> *&exportMsgsOut, ULong timeoutMilisec)
{
    Int32 st = _consumerGuards[consumerQueueId]->Wait(timeoutMilisec);
    if(*_msgConsumerQueueChanges[consumerQueueId])
    {
        auto temp = exportMsgsOut;
        exportMsgsOut = _consumerMsgQueues[consumerQueueId];
        _consumerMsgQueues[consumerQueueId] = temp;
        *_msgConsumerQueueChanges[consumerQueueId] = false;
    }

    return st;
}

inline void ConcurrentMessageQueue::NotifyPop(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Sinal();
}

inline void ConcurrentMessageQueue::PopImmediately(UInt32 consumerQueueId, std::list<FS_MessageBlock *> *&exportMsgsOut)
{
    if(*_msgConsumerQueueChanges[consumerQueueId])
    {
        auto temp = exportMsgsOut;
        exportMsgsOut = _consumerMsgQueues[consumerQueueId];
        _consumerMsgQueues[consumerQueueId] = temp;
        *_msgConsumerQueueChanges[consumerQueueId] = false;
    }
}

inline void ConcurrentMessageQueue::PopUnlock(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Unlock();
}

inline bool ConcurrentMessageQueue::HasMsgToConsume(UInt32 consumerQueueId) const
{
    _consumerGuards[consumerQueueId]->Lock();
    bool isEmpty = _consumerMsgQueues[consumerQueueId]->empty();
    _consumerGuards[consumerQueueId]->Unlock();
    return !isEmpty;
}

inline ConcurrentMessageQueueNoThread::ConcurrentMessageQueueNoThread(UInt32 generatorQuantity, UInt32 consumerQuantity)
    :_consumerQuantity{consumerQuantity}
    , _generatorQuantity{generatorQuantity}
    , _isWorking{false}
    ,_isStart{false}
    ,_generatorMaxId{0}
    ,_consumerMaxId{0}
{
}

inline ConcurrentMessageQueueNoThread::~ConcurrentMessageQueueNoThread()
{
    BeforeClose();
    Close();
}


inline Int32 ConcurrentMessageQueueNoThread::Start()
{
    if(_isStart)
        return StatusDefs::Success;

    _isStart = true;
    _isWorking = true;
    return StatusDefs::Success;
}

inline bool ConcurrentMessageQueueNoThread::IsWorking() const
{
    return _isWorking;
}

inline UInt32 ConcurrentMessageQueueNoThread::GetGeneratorQuality() const
{
    return _generatorQuantity;
}

inline UInt32 ConcurrentMessageQueueNoThread::GenerateGeneratorId()
{
    _sysLocker.Lock();
    UInt32 id = _generatorMaxId;
    ++_generatorMaxId;
    _sysLocker.Unlock();
    return id;
}

inline UInt32 ConcurrentMessageQueueNoThread::GenerateConsumerId()
{
    _sysLocker.Lock();
    UInt32 id = _consumerMaxId;
    ++_consumerMaxId;
    _sysLocker.Unlock();
    return id;
}

inline bool ConcurrentMessageQueueNoThread::CanGenerateGeneratiroId() const
{
    return _generatorMaxId < _generatorQuantity;
}

inline bool ConcurrentMessageQueueNoThread::CanGenerateConsumerId() const
{
    return _consumerMaxId < _consumerQuantity;
}

inline bool ConcurrentMessageQueueNoThread::Push(UInt32 generatorQueueId, std::list<FS_MessageBlock *> *&msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    const auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);
    auto consumerQueue = _consumerMsgQueues[consumerId];

    _generatorGuards[generatorQueueId]->Lock();
    auto generatorMsgQueue = consumerQueue->at(generatorQueueId);
    if(*_msgGeneratorMsgQueueChanges[generatorQueueId])
    {// 生产者消息还未被消费

        for(auto iterMsg = msgs->begin(); iterMsg != msgs->end(); )
        {
            generatorMsgQueue->push_back(*iterMsg);
            iterMsg = msgs->erase(iterMsg);
        }
    }
    else
    {// 空队列则交换
        std::list<FS_MessageBlock *> *gemTemp = NULL;
        gemTemp = msgs;
        msgs = generatorMsgQueue;
        (*consumerQueue)[generatorQueueId] = gemTemp;
        *_msgGeneratorMsgQueueChanges[generatorQueueId] = true;
    }
    _generatorGuards[generatorQueueId]->Unlock();

    _consumerGuards[consumerId]->Lock();
    *_msgConsumerQueueChanges[consumerId] = true;
    _consumerGuards[consumerId]->Sinal();
    _consumerGuards[consumerId]->Unlock();

    return true;
}

inline bool ConcurrentMessageQueueNoThread::PushByConsumerId(UInt32 generatorQueueId, UInt32 consumerId, std::list<FS_MessageBlock *> *&msgs)
{
    if (UNLIKELY(!_isWorking))
        return false;

    auto consumerQueue = _consumerMsgQueues[consumerId];

    _generatorGuards[generatorQueueId]->Lock();
    auto generatorMsgQueue = consumerQueue->at(generatorQueueId);
    if (*_msgGeneratorMsgQueueChanges[generatorQueueId])
    {// 生产者消息还未被消费

        for (auto iterMsg = msgs->begin(); iterMsg != msgs->end(); )
        {
            generatorMsgQueue->push_back(*iterMsg);
            iterMsg = msgs->erase(iterMsg);
        }
    }
    else
    {// 空队列则交换
        std::list<FS_MessageBlock *> *gemTemp = NULL;
        gemTemp = msgs;
        msgs = generatorMsgQueue;
        (*consumerQueue)[generatorQueueId] = gemTemp;
        *_msgGeneratorMsgQueueChanges[generatorQueueId] = true;
    }
    _generatorGuards[generatorQueueId]->Unlock();

    _consumerGuards[consumerId]->Lock();
    *_msgConsumerQueueChanges[consumerId] = true;
    _consumerGuards[consumerId]->Sinal();
    _consumerGuards[consumerId]->Unlock();

    return true;
}

inline bool ConcurrentMessageQueueNoThread::Push(UInt32 generatorQueueId, FS_MessageBlock *messageBlock)
{
    if(UNLIKELY(!_isWorking))
        return false;

    const auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);

    _generatorGuards[generatorQueueId]->Lock();
    _consumerMsgQueues[consumerId]->at(generatorQueueId)->push_back(messageBlock);
    *_msgGeneratorMsgQueueChanges[generatorQueueId] = true;
    _generatorGuards[generatorQueueId]->Unlock();

    _consumerGuards[consumerId]->Lock();
    *_msgConsumerQueueChanges[consumerId] = true;
    _consumerGuards[consumerId]->Sinal();
    _consumerGuards[consumerId]->Unlock();

    return true;
}

inline bool ConcurrentMessageQueueNoThread::PushByConsumerId(UInt32 generatorQueueId, UInt32 consumerId, FS_MessageBlock *messageBlock)
{
    if (UNLIKELY(!_isWorking))
        return false;

    _generatorGuards[generatorQueueId]->Lock();
    _consumerMsgQueues[consumerId]->at(generatorQueueId)->push_back(messageBlock);
    *_msgGeneratorMsgQueueChanges[generatorQueueId] = true;
    _generatorGuards[generatorQueueId]->Unlock();

    _consumerGuards[consumerId]->Lock();
    *_msgConsumerQueueChanges[consumerId] = true;
    _consumerGuards[consumerId]->Sinal();
    _consumerGuards[consumerId]->Unlock();

    return true;
}

inline void ConcurrentMessageQueueNoThread::NotifyConsumerByGenerator(UInt32 generatorQueueId)
{
    if(!_isWorking)
        return;

    auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);
    auto guards = _consumerGuards[consumerId];
    guards->Lock();
    guards->Sinal();
    guards->Unlock();
}

inline Int32 ConcurrentMessageQueueNoThread::WaitForPoping(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs, ULong timeoutMilisec)
{
    auto consumerGuard = _consumerGuards[consumerQueueId];
    consumerGuard->Lock();
    Int32 st = StatusDefs::Success;
    if(!*_msgConsumerQueueChanges[consumerQueueId])
    {
        st = consumerGuard->Wait(timeoutMilisec);
    }
    hasMsgs = *_msgConsumerQueueChanges[consumerQueueId];
    *_msgConsumerQueueChanges[consumerQueueId] = false;
    consumerGuard->Unlock();

    auto consumerQueue = _consumerMsgQueues[consumerQueueId];
    _PopImmediately(consumerQueue, generatorMsgs, hasMsgs);

    return st;
}

inline void ConcurrentMessageQueueNoThread::NotifyConsumer(UInt32 consumerQueueId)
{
    auto guard = _consumerGuards[consumerQueueId];
    guard->Lock();
    guard->Sinal();
    guard->Unlock();
}

inline void ConcurrentMessageQueueNoThread::PopImmediately(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs)
{
    auto consumerGuard = _consumerGuards[consumerQueueId];
    consumerGuard->Lock();
    hasMsgs = *_msgConsumerQueueChanges[consumerQueueId];
    *_msgConsumerQueueChanges[consumerQueueId] = false;
    consumerGuard->Unlock();

    auto consumerQueue = _consumerMsgQueues[consumerQueueId];
    _PopImmediately(consumerQueue, generatorMsgs, hasMsgs);
}

inline bool ConcurrentMessageQueueNoThread::HasMsgToConsume(UInt32 consumerQueueId) const
{
    return *_msgConsumerQueueChanges[consumerQueueId];
}

inline UInt32 ConcurrentMessageQueueNoThread::_GetConsumerIdByGeneratorId(UInt32 generatorId) const
{
    return generatorId % _consumerQuantity;
}

inline void ConcurrentMessageQueueNoThread::_PopImmediately(std::vector<std::list<FS_MessageBlock *> *> *consumerQueue, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool isConsumerQueueChange)
{
    if(!isConsumerQueueChange)
        return;

    std::list<FS_MessageBlock *> *temp = NULL;
    auto &generatorMsgVec = *generatorMsgs;
    ConditionLocker *guard = NULL;
    for(UInt32 i = 0; i < _generatorQuantity; ++i)
    {
        guard = _generatorGuards[i];
        guard->Lock();
        temp = (*consumerQueue)[i];
        if(!temp || temp->empty())
        {
            guard->Unlock();
            continue;
        }

        (*consumerQueue)[i] = generatorMsgVec[i];
        generatorMsgVec[i] = temp;
        *_msgGeneratorMsgQueueChanges[i] = false;
        guard->Unlock();
    }
}

inline MessageQueueNoThread::MessageQueueNoThread()
    :_msgConsumerQueueChange(false)
    ,_consumerMsgQueue(NULL)
    ,_isWorking(false)
    ,_isStart(false)
{
}

inline MessageQueueNoThread::~MessageQueueNoThread()
{
    BeforeClose();
    Close();
}

inline bool MessageQueueNoThread::IsWorking() const
{
    return _isWorking;
}

inline bool MessageQueueNoThread::Push(std::list<FS_MessageBlock *> *&msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    _consumerGuard.Lock();
    if(_msgConsumerQueueChange)
    {
        for(auto iterMsg = msgs->begin(); iterMsg != msgs->end();)
        {
            _consumerMsgQueue->push_back(*iterMsg);
            iterMsg = msgs->erase(iterMsg);
        }
    }
    else
    {
        std::list<FS_MessageBlock *> *gemTemp = NULL;
        gemTemp = msgs;
        msgs = _consumerMsgQueue;
        _consumerMsgQueue = gemTemp;
        _msgConsumerQueueChange = true;
    }
    _consumerGuard.Sinal();
    _consumerGuard.Unlock();
    return true;
}

inline bool MessageQueueNoThread::Push(FS_MessageBlock *messageBlock)
{
    if(UNLIKELY(!_isWorking))
        return false;

    _consumerGuard.Lock();
    _msgConsumerQueueChange = true;
    _consumerMsgQueue->push_back(messageBlock);
    _consumerGuard.Sinal();
    _consumerGuard.Unlock();
    return true;
}

inline Int32 MessageQueueNoThread::WaitForPoping(std::list<FS_MessageBlock *> *&generatorMsgs, ULong timeoutMilisec)
{
    _consumerGuard.Lock();
    Int32 ret = StatusDefs::Success;
    if(!_msgConsumerQueueChange)
    {
        ret = _consumerGuard.Wait(timeoutMilisec);
    }
    _PopImmediately(generatorMsgs);
    _consumerGuard.Unlock();
    return ret;
}

inline void MessageQueueNoThread::PopImmediately(std::list<FS_MessageBlock *> *&generatorMsgs)
{
    _consumerGuard.Lock();
    _PopImmediately(generatorMsgs);
    _consumerGuard.Unlock();
}

inline void MessageQueueNoThread::Notify()
{
    _consumerGuard.Lock();
    _consumerGuard.Sinal();
    _consumerGuard.Unlock();
}

inline bool MessageQueueNoThread::HasMsgToConsume() const
{
    return _msgConsumerQueueChange;
}

inline void MessageQueueNoThread::_PopImmediately(std::list<FS_MessageBlock *> *&generatorMsgs)
{
    if(!_msgConsumerQueueChange)
        return;

    std::list<FS_MessageBlock *> *temp = NULL;
    temp = generatorMsgs;
    generatorMsgs = _consumerMsgQueue;
    _consumerMsgQueue = temp;
    _msgConsumerQueueChange = false;
}
FS_NAMESPACE_END

#endif
