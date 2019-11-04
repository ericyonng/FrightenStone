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
 * @file  : FS_ThreadPool.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/Task/Task.h"
#include <iostream>

#include "base/common/assist/utils/utils.h"

#pragma region 
#ifdef _WIN32
#include <process.h>     // 
#endif
#pragma endregion

FS_NAMESPACE_BEGIN

// OBJ_POOL_CREATE_IMPL(FS_ThreadPool, _objPoolHelper, __DEF_OBJ_POOL_OBJ_NUM__, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__)

FS_ThreadPool::FS_ThreadPool()
{

}

FS_ThreadPool::FS_ThreadPool(Int32 minNum, Int32 maxNum)
{
    _minNum = minNum;
    _maxNum = maxNum;

    if(_minNum)
        _CreateThread(_minNum);
}

FS_ThreadPool::~FS_ThreadPool()
{
    // Clear();
}

void FS_ThreadPool::Release()
{
    delete this;
}

bool FS_ThreadPool::AddTask(ITask &task, bool forceNewThread /*= false*/, Int32 numOfThreadToCreateIfNeed /*= 1*/)
{
    _locker.Lock();
    if(_isDestroy || _isStopAddingTask || !_isPoolWorking)
    {
        _locker.Unlock();
        return false;
    }

    const Int32 diffNum = _maxNum - _curTotalNum;
    numOfThreadToCreateIfNeed = diffNum > numOfThreadToCreateIfNeed ? numOfThreadToCreateIfNeed : diffNum;

    if(_waitNum > 0 && !forceNewThread)    // 
    {
        _tasks.push_back(&task);
        _locker.Sinal();
        _locker.Unlock();
        return true;
    }

    if(UNLIKELY(_curTotalNum + numOfThreadToCreateIfNeed > _maxNum))
    {
        _locker.Unlock();
        return false;
    }

    _tasks.push_back(&task);
    _CreateThread(numOfThreadToCreateIfNeed);
    _locker.Unlock();

    return true;
}

bool FS_ThreadPool::AddTask(IDelegate<void, FS_ThreadPool *> *callback, bool forceNewThread /*= false*/, Int32 numOfThreadToCreateIfNeed /*= 1*/)
{
    // 创建一个delegate task
    DelegateTask *newTask = new DelegateTask(this, callback);
    return AddTask(*newTask, forceNewThread, numOfThreadToCreateIfNeed);
}

unsigned __stdcall FS_ThreadPool::ThreadHandler(void *param)
{
    FS_ThreadPool *pool = static_cast<FS_ThreadPool *>(param);
    auto &locker = pool->_locker;
    auto &taskList = pool->_tasks;
    bool needInitMemPool = pool->_initThreadMemPoolWhenThreadStart;
    auto &threadIdRefMemPool = pool->_threadIdRefMemPool;

    // 创建线程局部内存池
    if(needInitMemPool)
        ASSERT(pool->NewCurThreadMemPool());

    bool isEmpty = false;
    while(!pool->_isDestroy || !isEmpty)
    {
        locker.Lock();
        if(LIKELY(!taskList.empty()))
        {
            auto task = taskList.front();
            taskList.pop_front();
            isEmpty = false;
            locker.Unlock();
            task->Run();
            task->Release();
            continue;
        }

        ++pool->_waitNum;
        locker.Wait();
        --pool->_waitNum;
        isEmpty = taskList.empty();
        locker.Unlock();
    }

    try
    {
        locker.Lock();
        --pool->_curTotalNum;
        bool noThread = pool->_curTotalNum <= 0;
        if(noThread)
            locker.Sinal();
        locker.Unlock();
    }
    catch(...)
    {
        throw std::logic_error("thread pool task thread crash");
        std::cout << "hello crash" << std::endl;
    }

    // 释放线程局部存储资源
    FS_TlsUtil::FreeUtilTlsTable();

    std::cout << "_endthreadex end" << std::endl;
    _endthreadex(0L);

    return 0L;
}

void FS_ThreadPool::Close()
{
    _locker.Lock();
    if(!_isPoolWorking)
    {
        _locker.Unlock();
        return;
    }

    _isPoolWorking = false;
    _isStopAddingTask = true;
    _isDestroy = true;
    _locker.Unlock();

    while(true)
    {
        _locker.Lock();
        _locker.Sinal();
        _locker.Unlock();
        Sleep(THREAD_POOL_WAIT_FOR_COMPLETED_TIME);
        _locker.Lock();
        if(_tasks.empty() && _curTotalNum <= 0)
        {
            _locker.ResetSinal();
            _locker.Unlock();
            break;
        }
        _locker.Unlock();
    }

    // _locker.Broadcast();
    //std::cout << "clear end" << std::endl;
}


void FS_ThreadPool::SetThreadLimit(Int32 minNum, Int32 maxNum)
{
    if(maxNum > _maxNum || _isDestroy || _isStopAddingTask || !_isPoolWorking)
        return;

    Int32 maxNumTmp = _maxNum;
    _maxNum = std::max<Int32>(maxNum, maxNumTmp);

    Int32 diffMin = minNum - _minNum;
    if(diffMin > 0)
    {
        _minNum = minNum;
        _CreateThread(diffMin);
    }
}

IMemoryPoolMgr *FS_ThreadPool::NewCurThreadMemPool()
{
    // 创建内存池
    IMemoryPoolMgr *memPool = NULL;
    auto tlsTable = FS_TlsUtil::GetUtilTlsTable();
    auto tlsMemPool = tlsTable->GetElement<Tls_MemoryPool>(TlsElemType::Tls_MemoryPool);
    if(!tlsMemPool)
        tlsMemPool = tlsTable->AllocElement<Tls_MemoryPool>(TlsElemType::Tls_MemoryPool);

    memPool = tlsMemPool->_pool;
    if(memPool)
        ASSERT(memPool->InitPool() != StatusDefs::Success);

    _locker.Lock();
    _threadIdRefMemPool[SystemUtil::GetCurrentThreadId()] = memPool;
    _locker.Unlock();
    return memPool;
}

bool FS_ThreadPool::_CreateThread(Int32 numToCreate)
{
    if(numToCreate <= 0 || numToCreate > (_maxNum - _curTotalNum))
        return false;

    UInt32 threadId = 0;
    for(Int32 i = 0; i < numToCreate; ++i)
    {
        auto threadHandle = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, ThreadHandler, static_cast<void *>(this), 0, &threadId));
        if(LIKELY(threadHandle != INVALID_HANDLE_VALUE))
            CloseHandle(threadHandle);
        else
        {
            if(i == 0)
                return false;

            break;
        }

        ++_curTotalNum;
    }

    return true;
}

FS_NAMESPACE_END


