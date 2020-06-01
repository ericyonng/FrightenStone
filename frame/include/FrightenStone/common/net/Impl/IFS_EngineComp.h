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
 * @file  : IFS_EngineComp.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/12/30
 * @brief :
 */
#ifndef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__
#define __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/net/Defs/EngineCompDefs.h"

FS_NAMESPACE_BEGIN

class IFS_NetEngine;
struct NetEngineTotalCfgs;
class MessageQueueNoThread;
class ConcurrentMessageQueueNoThread;

class BASE_EXPORT IFS_EngineComp
{
public:
    IFS_EngineComp(IFS_NetEngine *engine, UInt32 compId, Int32 compType);
    virtual ~IFS_EngineComp() {}

public:
    virtual Int32 BeforeStart(const NetEngineTotalCfgs &totalCfgs);
    virtual Int32 Start();
    virtual void AfterStart() = 0;
    virtual void WillClose() = 0;         // 断开与模块之间的依赖
    virtual void BeforeClose();
    virtual void Close();
    virtual void AfterClose() = 0;

public:
    virtual void OnSessionDisconnected(UInt64 sessionId) {}

public:
    // 类型转换
    template<typename CompType>
    CompType *CastTo();
    // 类型转换
    template<typename CompType>
    const CompType *CastTo() const;

    // 获取组件id
    virtual UInt32 GetCompId() const;
    virtual Int32 GetCompType() const;
    // 绑定消息队列（用于指向性目标只有单一的消息队列），对组件自己本身来说只能pop不能push
    virtual void BindCompMq(MessageQueueNoThread *compMq);
    // 附加所有组件的消息队列
    virtual void AttachAllCompMq(std::vector<MessageQueueNoThread *> *allCompMq);
    MessageQueueNoThread *GetMyMailBox();

    // 引擎对象
    IFS_NetEngine *GetEngine();
    void MaskReady(bool isReady);
    bool IsReady() const;

    FS_String ToString() const;
    
protected:
    UInt32 _compId;
    Int32 _compType;
    IFS_NetEngine *_engine;
    MessageQueueNoThread *_myCompMq;  // 对于组件自己本身来说组件自己是消费者,组件不可进行push操作，只能pop
    std::vector<MessageQueueNoThread *> *_allCompMq;    // 所有组件的单一消息队列,使用组件id进行索引
    std::atomic_bool _isCompReady;
};

FS_NAMESPACE_END

#include "FrightenStone/common/net/Impl/IFS_EngineCompImpl.h"

#endif
