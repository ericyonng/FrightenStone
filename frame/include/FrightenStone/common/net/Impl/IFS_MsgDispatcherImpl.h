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
 * @file  : IFS_MsgDispatcherImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/01/05
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_MsgDispatcher_H__
#pragma once

FS_NAMESPACE_BEGIN
inline IFS_MsgDispatcher::IFS_MsgDispatcher(IFS_NetEngine *engine, UInt32 compId)
    :IFS_EngineComp(engine, compId)
    ,_generatorId(0)
    ,_consumerId(0)
    ,_concurrentMq(NULL)
{
}

inline IFS_MsgDispatcher::~IFS_MsgDispatcher()
{

}

inline UInt32 IFS_MsgDispatcher::GetGeneratorId() const
{
    return _generatorId;
}

inline UInt32 IFS_MsgDispatcher::GetConsumerId() const
{
    return _consumerId;
}

inline void IFS_MsgDispatcher::BindConcurrentParams(UInt32 generatorId, UInt32 consumerId, ConcurrentMessageQueueNoThread *concurrentMq)
{
    _generatorId = generatorId;
    _consumerId = consumerId;
    _concurrentMq = concurrentMq;
}

FS_NAMESPACE_END

#endif
