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
 * @file  : MessageQueueImpl.h
 * @author: ericyonng<120453674@qq.com>
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
    :_consumerQuantity(consumerQuantity)
    ,_generatorQuantity(generatorQuantity)
    ,_isWorking(false)
    ,_isStart(false)
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

inline void ConcurrentMessageQueueNoThread::PushLock(UInt32 generatorQueueId)
{
    _generatorGuards[generatorQueueId]->Lock();
}

inline bool ConcurrentMessageQueueNoThread::Push(UInt32 generatorQueueId, std::list<FS_MessageBlock *> *&msgs)
{
    if(UNLIKELY(!_isWorking))
        return false;

    const auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);
    auto consumerQueue = _consumerMsgQueues[consumerId];
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

    *_msgConsumerQueueChanges[consumerId] = true;

    return true;
}

inline bool ConcurrentMessageQueueNoThread::Push(UInt32 generatorQueueId, FS_MessageBlock *messageBlock)
{
    if(UNLIKELY(!_isWorking))
        return false;

    const auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);
    _consumerMsgQueues[consumerId]->at(generatorQueueId)->push_back(messageBlock);
    *_msgGeneratorMsgQueueChanges[generatorQueueId] = true;
    *_msgConsumerQueueChanges[consumerId] = true;

    return true;
}

inline void ConcurrentMessageQueueNoThread::PushUnlock(UInt32 generatorQueueId)
{
    _generatorGuards[generatorQueueId]->Unlock();
}

inline void ConcurrentMessageQueueNoThread::NotifyConsumer(UInt32 generatorQueueId)
{
    if(!_isWorking)
        return;

    auto consumerId = _GetConsumerIdByGeneratorId(generatorQueueId);
    auto guards = _consumerGuards[consumerId];
    guards->Lock();
    guards->Sinal();
    guards->Unlock();
}

inline void ConcurrentMessageQueueNoThread::PopLock(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Lock();
}

inline Int32 ConcurrentMessageQueueNoThread::WaitForPoping(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs, ULong timeoutMilisec)
{
    Int32 st = _consumerGuards[consumerQueueId]->Wait(timeoutMilisec);
    PopImmediately(consumerQueueId, generatorMsgs, hasMsgs);

    return st;
}

inline void ConcurrentMessageQueueNoThread::NotifyPop(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Sinal();
}

inline void ConcurrentMessageQueueNoThread::PopImmediately(UInt32 consumerQueueId, std::vector<std::list<FS_MessageBlock *> *> *&generatorMsgs, bool &hasMsgs)
{
    if(*_msgConsumerQueueChanges[consumerQueueId])
    {
        std::list<FS_MessageBlock *> *temp = NULL;
        auto consumerQueue = _consumerMsgQueues[consumerQueueId];
        auto &generatorMsgVec = *generatorMsgs;
        ConditionLocker *guard = NULL;
        for(UInt32 i = 0; i < _generatorQuantity; ++i)
        {
            auto temp = (*consumerQueue)[i];
            if(!temp)
                continue;

            guard = _generatorGuards[i];
            guard->Lock();
            if(temp->empty())
            {
                guard->Unlock();
                continue;
            }

            (*consumerQueue)[i] = generatorMsgVec[i];
            generatorMsgVec[i] = temp;
            *_msgGeneratorMsgQueueChanges[i] = false;
            guard->Unlock();
            hasMsgs = true;
        }

        *_msgConsumerQueueChanges[consumerQueueId] = false;
    }
}

inline void ConcurrentMessageQueueNoThread::PopUnlock(UInt32 consumerQueueId)
{
    _consumerGuards[consumerQueueId]->Unlock();
}

inline bool ConcurrentMessageQueueNoThread::HasMsgToConsume(UInt32 consumerQueueId) const
{
    return *_msgConsumerQueueChanges[consumerQueueId];
}

inline UInt32 ConcurrentMessageQueueNoThread::_GetConsumerIdByGeneratorId(UInt32 generatorId) const
{
    return generatorId % _consumerQuantity;
}

FS_NAMESPACE_END

#endif
