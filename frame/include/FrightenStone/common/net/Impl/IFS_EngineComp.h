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
 * @file  : IFS_EngineComp.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/30
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"

FS_NAMESPACE_BEGIN

class IFS_NetEngine;
struct NetEngineTotalCfgs;
class MessageQueueNoThread;
class ConcurrentMessageQueueNoThread;

class BASE_EXPORT IFS_EngineComp
{
public:
    IFS_EngineComp(IFS_NetEngine *engine, UInt32 compId);
    virtual ~IFS_EngineComp() {}

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &totalCfgs) = 0;
    virtual Int32 Start() = 0;
    virtual void AfterStart() = 0;
    virtual void WillClose() = 0;         // 断开与模块之间的依赖
    virtual void BeforeClose() = 0;       // 处理未决数据，初步的清理
    virtual void Close() = 0;
    virtual void AfterClose() = 0;

public:
    // 获取组件id
    virtual UInt32 GetCompId() const;
    // 获取生产者id
    virtual UInt32 GetGeneratorId() const;
    // 获取消费者id
    virtual UInt32 GetConsumerId() const;
    // 并发消息队列参数绑定
    virtual void BindConcurrentParams(UInt32 generatorId, UInt32 consumerId, ConcurrentMessageQueueNoThread *concurrentMq);
    // 绑定消息队列（用于指向性目标只有单一的消息队列），对组件自己本身来说只能pop不能push
    virtual void BindCompMq(MessageQueueNoThread *compMq);
    // 附加所有组件的消息队列
    virtual void AttachAllCompMq(std::vector<MessageQueueNoThread *> *allCompMq);

    // 引擎对象
    IFS_NetEngine *GetEngine();
    
protected:
    UInt32 _compId;
    UInt32 _generatorId;            // engine中concurrentmq指定的生产者id
    UInt32 _consumerId;             // 指定的，并发消息队列中对应的消费者id
    ConcurrentMessageQueueNoThread *_concurrentMq;  // 单向的消息队列,当组件是生产者时只能push不能pop,只能generatiorId
    IFS_NetEngine *_engine;
    MessageQueueNoThread *_myCompMq;  // 对于组件自己本身来说组件自己是消费者,组件不可进行push操作，只能pop
    std::vector<MessageQueueNoThread *> *_allCompMq;    // 所有组件的单一消息队列,使用组件id进行索引
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_EngineCompImpl.h"

#endif
