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
#ifndef __Base_Common_Component_Impl_FS_ThreadPool__H__
#define __Base_Common_Component_Impl_FS_ThreadPool__H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"
#include "base/common/asyn/asyn.h"
#include <atomic>
#include <list>

FS_NAMESPACE_BEGIN

class BASE_EXPORT ITask;

class BASE_EXPORT FS_ThreadPool
{
public:
    explicit FS_ThreadPool();
    explicit FS_ThreadPool(Int32 minNum, Int32 maxNum);    // 
    virtual ~FS_ThreadPool();

public:
    void Clear();
    bool AddTask(ITask &task, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);
    void StopAdd();
    void EnableAdd();
    void SetThreadLimit(Int32 minNum, Int32 maxNum);
    static unsigned __stdcall ThreadHandler(void *param);

    // ״̬
    bool IsClearingPool() const;

private:
    bool _CreateThread(Int32 numToCreate);    // 

private:
    std::atomic<Int32> _minNum{0};                      // 
    std::atomic<Int32> _maxNum{0};                      // 
    std::atomic<Int32> _curTotalNum{0};                 // 
    std::atomic<Int32> _waitNum{0};                     // 
    std::atomic<bool> _isDestroy{false};                // 
    std::atomic<bool> _isStopAddingTask{false};         // 
    std::atomic<bool> _isClearPool{false};              // 
    ConditionLocker _locker;                            // 
    std::list<ITask *> _tasks;                          // 
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

#define FS_MessageQueue(x) fs::FS_ThreadPool x(1, 1)
#define FS_MessageQueuePtr(x) fs::FS_ThreadPool *x = new fs::FS_ThreadPool(1, 1)
#endif
