#ifndef __Base_Common_Component_Impl_FS_ThreadPool__H__
#define __Base_Common_Component_Impl_FS_ThreadPool__H__
/**
* @file    FS_ThreadPool.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/15
* @brief
*/

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
#pragma endregion

FS_NAMESPACE_END

#define FS_MessageQueue(x) fs::FS_ThreadPool x(1, 1)
#define FS_MessageQueuePtr(x) fs::FS_ThreadPool *x = new fs::FS_ThreadPool(1, 1)
#endif
