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
 * @file  : DelegateTask.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/26
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_Task_Impl_DelegateTask_H__
#define __Base_Common_Component_Impl_Task_Impl_DelegateTask_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/status/status.h"
#include "base/common/component/Impl/Task/Interface/ITask.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;

class BASE_EXPORT DelegateTask : public ITask
{
    // OBJ_POOL_CREATE(DelegateTask, _objPoolHelper);
public:
    DelegateTask(const FS_ThreadPool *pool, IDelegatePlus<void, const FS_ThreadPool *> *callback);
    virtual ~DelegateTask();

    virtual Int32 Run();
    virtual Int32 Release();

private:
    const FS_ThreadPool *_pool;                             // 线程池对象
    IDelegatePlus<void, const FS_ThreadPool *> *_callback;    // 需要释放
};

FS_NAMESPACE_END

#include "base/common/component/Impl/Task/Impl/DelegateTaskImpl.h"

#endif
