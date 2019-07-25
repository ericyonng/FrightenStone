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
 * @file  : ObjBlock.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_ObjPool_Defs_ObjBlock_H__
#define __Base_Common_ObjPool_Defs_ObjBlock_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
class IObjAlloctor;

template<typename ObjType>
class ObjBlock
{
public:
    ObjBlock();
    ~ObjBlock();

    const char *GetObjName() const;

public:
    Int64                   _ref;
    Int64                   _objSize;
    IObjAlloctor<ObjType>   *_alloctor;
    ObjBlock<ObjType>       *_nextBlock;
    bool                    _isInPool;
};

FS_NAMESPACE_END

#include "base/common/objpool/Defs/ObjBlockImpl.h"


// block链表:每次new的时候从链表头取内存块，每次释放内存时总是将内存块加到内存头
// free时候需要找到内存块（使用对象内存与block内存对称方式）并加到内存block头

#endif
