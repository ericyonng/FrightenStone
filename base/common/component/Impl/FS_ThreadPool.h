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
#ifndef __Base_Common_Component_Impl_FS_ThreadPool_H__
#define __Base_Common_Component_Impl_FS_ThreadPool_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/asyn/asyn.h"
#include <atomic>
#include <list>
#include "base/common/objpool/objpool.h"
#include "base/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ITask;

class BASE_EXPORT FS_ThreadPool
{
    OBJ_POOL_CREATE(fs::FS_ThreadPool, _objPoolHelper)
public:
    explicit FS_ThreadPool();
    explicit FS_ThreadPool(Int32 minNum, Int32 maxNum);    // 
    virtual ~FS_ThreadPool();
    virtual void Release();

public:
    // 添加任务 请确保同一个任务只投递一次，避免多线程处理同一任务的情况出现（除非线程池只有一个线程）
    bool AddTask(ITask &task, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);
    // 添加delegate任务 delegate内部释放 请确保同一个任务只投递一次，避免多线程处理同一任务的情况出现（除非线程池只有一个线程）
    bool AddTask(IDelegatePlus<void, const FS_ThreadPool *> *callback, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);
    // 线程任务执行
    static unsigned __stdcall ThreadHandler(void *param);

    // 清理线程池
    void Clear();
    // 停止添加任务
    void StopAdd();
    // 开启添加任务
    void EnableAdd();
    // 设置最小最大线程数
    void SetThreadLimit(Int32 minNum, Int32 maxNum);
    // 是否正在清理线程池
    bool IsClearingPool() const;

private:
    // 创建线程
    bool _CreateThread(Int32 numToCreate);

private:
    std::atomic<Int32> _minNum{0};                      // 最小线程数
    std::atomic<Int32> _maxNum{0};                      // 最大线程数
    std::atomic<Int32> _curTotalNum{0};                 // 当前线程数
    std::atomic<Int32> _waitNum{0};                     // 正在挂起的线程数
    std::atomic<bool> _isDestroy{false};                // 是否销毁线程池
    std::atomic<bool> _isStopAddingTask{false};         // 是否停止添加任务
    std::atomic<bool> _isClearPool{false};              // 是否正在清理线程池
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

inline bool FS_ThreadPool::IsClearingPool() const
{
    return _isClearPool;
}
#pragma endregion

FS_NAMESPACE_END

// 消息队列（单线程的线程池可当作消息队列）
#define FS_MessageQueue(x) fs::FS_ThreadPool x(1, 1)
#define FS_MessageQueuePtr(x) fs::FS_ThreadPool *x = new fs::FS_ThreadPool(1, 1)
#endif
