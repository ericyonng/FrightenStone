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
 * @file  : IFS_EngineCompImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/30
 * @brief :
 */
#ifdef __Frame_Include_FrightenStone_Common_Net_Impl_IFS_EngineComp_H__
#pragma once
FS_NAMESPACE_BEGIN
inline IFS_EngineComp::IFS_EngineComp(IFS_NetEngine *engine, UInt32 compId)
    :_compId(compId)
    ,_engine(engine)
{
}
template<typename CompType>
CompType *IFS_EngineComp::CastTo()
{
    return reinterpret_cast<CompType *>(this);
}

inline UInt32 IFS_EngineComp::GetCompId() const
{
    return _compId;
}

inline void IFS_EngineComp::BindCompMq(MessageQueueNoThread *compMq)
{
    _myCompMq = compMq;
}

// 附加所有组件的消息队列
inline void IFS_EngineComp::AttachAllCompMq(std::vector<MessageQueueNoThread *> *allCompMq)
{
    _allCompMq = allCompMq;
}

inline IFS_NetEngine *IFS_EngineComp::GetEngine()
{
    return _engine;
}

FS_NAMESPACE_END

#endif
