/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : MessageQueueTask.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/11/04
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Defs_MessageQueueTask_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_MessageQueue_Defs_MessageQueueTask_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/Task/Task.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;

class BASE_EXPORT MessageQueueTask : public ITask 
{
public:
    MessageQueueTask(UInt32 messageQueueId, FS_ThreadPool *pool, IDelegate<void, ITask *, FS_ThreadPool *> *callback);
    virtual ~MessageQueueTask();

public:
    virtual Int32 Run();
    UInt32 GetQueueId();

private:
    UInt32 _messageQueueId;
    FS_ThreadPool *_pool;
    IDelegate<void, ITask *, FS_ThreadPool *> *_callback;
};

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/MessageQueue/Defs/MessageQueueTaskImpl.h"

#endif
