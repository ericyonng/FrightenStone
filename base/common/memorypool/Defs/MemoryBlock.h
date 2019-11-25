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
 * @file  : MemoryBlock.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_MemoryPool_Defs_MemoryBlock_H__
#define __Base_Common_MemoryPool_Defs_MemoryBlock_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_String.h"

FS_NAMESPACE_BEGIN

class IMemoryAlloctor;

class BASE_EXPORT MemoryBlock
{
public:
    MemoryBlock(IMemoryAlloctor *alloctor);

    size_t           _objSize;
    IMemoryAlloctor  *_alloctor;
    MemoryBlock     *_nextBlock;
    Int32           _ref;           // 引用计数
    bool            _isInPool;
    char            _reserver1;     // 保留位，用于内存对齐
    char            _reserver2;     // 保留位，用于内存对齐
    char            _reserver3;     // 保留位，用于内存对齐
};

FS_NAMESPACE_END

#endif
