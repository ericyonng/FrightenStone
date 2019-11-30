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
 * @file  : FS_ThreadPool.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_FS_ThreadPool_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_FS_ThreadPool_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/Macro/MacroDefs.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"
#include "FrightenStone/common/asyn/asyn.h"
#include <atomic>
#include <list>
#include "FrightenStone/common/objpool/objpool.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/component/Defs/ThreadPoolDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ITask;
class BASE_EXPORT IMemoryPoolMgr;

// 不可使用包含线程池对象的对象包装
class BASE_EXPORT FS_ThreadPool
{
public:
    explicit FS_ThreadPool();
    explicit FS_ThreadPool(Int32 minNum, Int32 maxNum);    // 
    virtual ~FS_ThreadPool();
    virtual void Release();

public:
    // 添加任务 请确保同一个任务只投递一次，避免多线程处理同一任务的情况出现（除非线程池只有一个线程）
    bool AddTask(ITask &task, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);
    // 添加delegate任务 delegate内部释放 请确保同一个任务只投递一次，避免多线程处理同一任务的情况出现（除非线程池只有一个线程）
    bool AddTask(IDelegate<void, FS_ThreadPool *> *callback, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);

#ifdef _WIN32
    // 线程任务执行
    static unsigned __stdcall ThreadHandler(void *param);
#else
    static void *ThreadHandler(void *param);//线程处理函数
#endif


    // 清理线程池
    void Close();
    // 停止添加任务
    void StopAdd();
    // 开启添加任务
    void EnableAdd();
    // 设置最小最大线程数
    void SetThreadLimit(Int32 minNum, Int32 maxNum);
    // 是否正在清理线程池
    bool IsPoolWorking() const;
    // 设置是否使用线程级无锁内存池 线程级内存池只对剩余未创建的线程有效
    void SetInitThreadMemPoolWhenThreadStart(bool needInit);
    void Lock();
    void Unlock();
    IMemoryPoolMgr *GetThreadMemPool(Int32 threadId); // 线程不安全
    IMemoryPoolMgr *NewCurThreadMemPool(); // 线程安全

private:
    // 创建线程
    bool _CreateThread(Int32 numToCreate, UInt64 unixStackSize = THREAD_DEF_STACK_SIZE);

private:
    std::atomic<Int32> _minNum{0};                      // 最小线程数
    std::atomic<Int32> _maxNum{0};                      // 最大线程数
    std::atomic<Int32> _curTotalNum{0};                 // 当前线程数
    std::atomic<Int32> _waitNum{0};                     // 正在挂起的线程数
    std::atomic<bool> _isDestroy{false};                // 是否销毁线程池
    std::atomic<bool> _isStopAddingTask{false};         // 是否停止添加任务
    std::atomic<bool> _isPoolWorking{true};             // 线程池是否工作
    std::atomic<bool> _initThreadMemPoolWhenThreadStart{false};      // 创建线程时候是否初始化线程局部内存池（只对剩余未创建的线程有效）(该部分内存仅限于线程生命周期内使用)
    std::map<UInt64, IMemoryPoolMgr *> _threadIdRefMemPool;  // 线程局部内存池
    ConditionLocker _locker;                            // 线程安全对象
    std::list<ITask *> _tasks;                          // 任务队列
};

#pragma region inline
inline void FS_ThreadPool::StopAdd()
{
    _isStopAddingTask = true;
}

inline void FS_ThreadPool::EnableAdd()
{
    _isStopAddingTask = false;
}

inline bool FS_ThreadPool::IsPoolWorking() const
{
    return _isPoolWorking;
}

inline void FS_ThreadPool::SetInitThreadMemPoolWhenThreadStart(bool needInit)
{
    _initThreadMemPoolWhenThreadStart = needInit;
}

inline void FS_ThreadPool::Lock()
{
    _locker.Lock();
}

inline void FS_ThreadPool::Unlock()
{
    _locker.Unlock();
}

inline IMemoryPoolMgr *FS_ThreadPool::GetThreadMemPool(Int32 threadId)
{
    auto iterMemPool = _threadIdRefMemPool.find(threadId);
    if(iterMemPool == _threadIdRefMemPool.end())
        return NULL;

    return iterMemPool->second;
}

#pragma endregion

FS_NAMESPACE_END

// 消息队列（单线程的线程池可当作消息队列）
#define FS_MessageQueue(x) fs::FS_ThreadPool x(1, 1)
#define FS_MessageQueuePtr(x) fs::FS_ThreadPool *x = new fs::FS_ThreadPool(1, 1)
#endif
