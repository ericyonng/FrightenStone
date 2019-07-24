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
 * @file  : MemoryBlockImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_MemoryPool_Defs_MemoryBlock_H__
#pragma once

FS_NAMESPACE_BEGIN

template<typename ObjType>
inline MemoryBlock<ObjType>::MemoryBlock(IMemoryAlloctor *alloctor)
    :_ref(0)
    , _objSize(0)
    , _alloctor(alloctor)
    , _nextBlock(NULL)
    , _isInPool(false)
{

}

template<typename ObjType>
inline MemoryBlock<ObjType>::~MemoryBlock()
{

}

template<typename ObjType>
const char *MemoryBlock<ObjType>::GetObjName()
{
    return typeid(ObjType).name();
}

FS_NAMESPACE_END

#endif
