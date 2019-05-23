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
    explicit FS_ThreadPool(Int32 minNum, Int32 maxNum);    // 请默认创建最低线程
    virtual ~FS_ThreadPool();

public:
    // 请外部主动清理线程池避免来不及清理
    void Clear();
    // 添加任务
    bool AddTask(ITask &task, bool forceNewThread = false, Int32 numOfThreadToCreateIfNeed = 1);
    // 禁止添加任务
    void StopAdd();
    // 允许添加任务
    void EnableAdd();
    // 设置线程上下限
    void SetThreadLimit(Int32 minNum, Int32 maxNum);
    // 线程执行体
    static unsigned __stdcall ThreadHandler(void *param);

private:
    // 创建线程
    bool _CreateThread(Int32 numToCreate);    // 线程不安全

private:
    std::atomic<Int32> _minNum{0};                      // 线程池最小准备线程
    std::atomic<Int32> _maxNum{0};                      // 线程池允许创建的最大线程数
    std::atomic<Int32> _curTotalNum{0};                 // 当前创建的线程数
    std::atomic<Int32> _waitNum{0};                     // 空闲的线程数
    std::atomic<bool> _isDestroy{false};                // 是否销毁线程池
    std::atomic<bool> _isStopAddingTask{false};         // 停止添加任务
    std::atomic<bool> _isClearPool{false};              // 是否正在清理线程池
    ConditionLocker _locker;                            // 线程安全对象
    std::list<ITask *> _tasks;                          // 任务队列    ->建议封装一个优先级队列以便优先的执行某个任务否则默认按照添加的先后执行
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
