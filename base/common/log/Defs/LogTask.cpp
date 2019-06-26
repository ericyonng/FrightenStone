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
 * @file  : LogTask.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/06/26
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/log/Defs/LogTask.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/FS_ThreadPool.h"
#include "base/common/component/Impl/Time.h"
#include <iostream>

FS_NAMESPACE_BEGIN

LogTask::LogTask(FS_ThreadPool *pool, IDelegatePlus<void> *taskDelegate, Int32 workIntervalMsTime)
    :_taskDelegate(taskDelegate)
    ,_pool(pool)
    ,_workIntervalMsTime(workIntervalMsTime)
{

}

LogTask::~LogTask()
{

}

Int32 LogTask::Run()
{
    while(true)
    {
        // 结束任务判断
        if(_pool->IsClearingPool())
        {
            (*_taskDelegate)();
            break;
        }

        // 写日志
        (*_taskDelegate)();

        // 休息一会儿
        Sleep(_workIntervalMsTime);
    }

    return StatusDefs::Success;
}

FS_NAMESPACE_END

