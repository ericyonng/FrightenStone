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
 * @file  : LogTask.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/25
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Log_Defs_LogTask_H__
#define __Base_Common_Log_Defs_LogTask_H__
#pragma once

#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/Task/Task.h"
#include "base/common/component/Impl/FS_Delegate.h"

// 日志任务
FS_NAMESPACE_BEGIN
class FS_ThreadPool;

class LogTask :public ITask
{
public:
    LogTask(FS_ThreadPool *pool, IDelegate<void, Int32> *taskDelegate, Int32 workIntervalMsTime, Int32 logFileIndex);
    virtual ~LogTask();

    // 任务执行体
    virtual Int32 Run();

protected:
    IDelegate<void, Int32> *_taskDelegate;
    FS_ThreadPool *_pool;
    Int32 _workIntervalMsTime;    // 写日志时间间隔
    Int32 _logFileIndex;
};


FS_NAMESPACE_END
#endif
