/**
* @file    FS_ThreadPool.cpp
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/15
* @brief
*/
#include "stdafx.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/ITask.h"
#include <iostream>

#pragma region 系统支持
#ifdef _WIN32
#include <process.h>     // 多线程支持
#endif
#pragma endregion

FS_NAMESPACE_BEGIN

FS_ThreadPool::FS_ThreadPool()
{

}

FS_ThreadPool::FS_ThreadPool(Int32 minNum, Int32 maxNum)
{
    _minNum = minNum;
    _maxNum = maxNum;

    // 初始化最小线程池
    if(_minNum)
        _CreateThread(_minNum);
}

FS_ThreadPool::~FS_ThreadPool()
{
    Clear();
}

void FS_ThreadPool::Clear()
{
    // 1.已清理过不在清理
    _locker.Lock();
    if(_isClearPool)
    {
        _locker.Unlock();
        return;
    }

    // 2.设置标志
    _isClearPool = true;
    _isStopAddingTask = true;
    _isDestroy = true;
    _locker.Unlock();

    // 3.等待完成剩余任务
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

    // 4.等待线程全部退出
    // _locker.Broadcast();
    std::cout << "clear end" << std::endl;
}

bool FS_ThreadPool::AddTask(ITask &task, bool forceNewThread /*= false*/, Int32 numOfThreadToCreateIfNeed /*= 1*/)
{
    // 1.判断是否可以添加任务
    _locker.Lock();
    if(_isDestroy || _isStopAddingTask || _isClearPool)
    {
        _locker.Unlock();
        return false;
    }

    // 2.校准可创建的线程数量
    const Int32 diffNum = _maxNum - _curTotalNum;
    numOfThreadToCreateIfNeed = diffNum > numOfThreadToCreateIfNeed ? numOfThreadToCreateIfNeed : diffNum;

    // 3.判断是否有空闲线程若有则唤醒，且非强制需要创建线程
    if(_waitNum > 0 && !forceNewThread)    // 若有空闲线程且不需要强制创建线程的唤醒线程
    {
        _tasks.push_back(&task);
        _locker.Sinal();
        _locker.Unlock();
        return true;
    }

    // 4.若当前线程已达到上限则添加失败
    if(UNLIKELY(_curTotalNum + numOfThreadToCreateIfNeed > _maxNum))
    {
        _locker.Unlock();
        return false;
    }

    // 5.创建线程
    _tasks.push_back(&task);
    _CreateThread(numOfThreadToCreateIfNeed);
    _locker.Unlock();

    return true;
}

void FS_ThreadPool::SetThreadLimit(Int32 minNum, Int32 maxNum)
{
    if(maxNum > _maxNum || _isDestroy || _isStopAddingTask || _isClearPool)
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

unsigned __stdcall FS_ThreadPool::ThreadHandler(void *param)
{
    // 1.初始化变量
    FS_ThreadPool *pool = static_cast<FS_ThreadPool *>(param);
    auto &locker = pool->_locker;
    auto &taskList = pool->_tasks;

    // 2.若未消耗则持续执行线程任务
    bool isEmpty = false;
    while(!pool->_isDestroy || !isEmpty)
    {
        locker.Lock();
        if(LIKELY(!taskList.empty()))
        {
            auto task = taskList.front();
            taskList.pop_front();
            isEmpty = false;   // 此时宁愿假设它是非空的
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
        // 3.任务结束需要退出并唤醒清理线程池等待
        locker.Lock();
        --pool->_curTotalNum;
        bool noThread = pool->_curTotalNum <= 0;
        if(noThread)
            locker.Sinal();
        locker.Unlock();
    }
    catch(...)
    {
        std::cout << "hello crash" << std::endl;
    }

    // 4.退出线程
    std::cout << "_endthreadex end" << std::endl;
    _endthreadex(0L);

    return 0L;
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


