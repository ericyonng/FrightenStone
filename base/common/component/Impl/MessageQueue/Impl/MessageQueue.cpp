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

#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/log/Log.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(MessageQueue, __DEF_OBJ_POOL_OBJ_NUM__);

MessageQueue::MessageQueue()
    :_msgGeneratorChange{false}
    ,_msgComsumerQueueChange(false)
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
            _msgComsumerQueue.push_back(*iterMsgBlock);
            iterMsgBlock = _msgSwitchQueue.erase(iterMsgBlock);
        }
        _msgComsumerQueueChange = true;
        _msgConsumerGuard.Sinal();
        _msgConsumerGuard.Unlock();
    }

    _msgConsumerGuard.Lock();
    _msgConsumerGuard.ResetSinal();
    _msgConsumerGuard.Unlock();

    _isWorking = false;
}
FS_NAMESPACE_END

