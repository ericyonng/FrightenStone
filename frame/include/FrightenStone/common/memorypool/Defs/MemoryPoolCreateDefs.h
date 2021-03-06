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
 * @file  : MemoryPoolCreateDefs.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/7
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_MemoryPool_Defs_MemoryPoolCreateDefs_H__
#define __Frame_Include_FrightenStone_Common_MemoryPool_Defs_MemoryPoolCreateDefs_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/Interface/IMemoryPoolMgr.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT MemoryPoolHelper
{
public:
    MemoryPoolHelper();
    virtual ~MemoryPoolHelper();

public:
    void *Alloc(size_t bytes);
    void Free(void *ptr);
    void AddRef(void *ptr);
};

FS_NAMESPACE_END

/// 内存池创建对象便利宏
// 声明中需要添加
#undef  MEM_POOL_CREATE
#define MEM_POOL_CREATE(_mempool_helper)                                                            \
public:                                                                                             \
        void  *operator new(size_t bytes){ return _mempool_helper->Alloc(bytes); }                  \
        void   operator delete(void *ptr)       { _mempool_helper->Free(ptr);}                      \
        void  *operator new[](size_t bytes)     { return _mempool_helper->Alloc(bytes);}            \
        void   operator delete[] (void *ptr)    { _mempool_helper->Free(ptr);}                      \
public:                                                                                             \
static fs::MemoryPoolHelper *_mempool_helper

// 在实现文件中需要添加
#undef MEM_POOL_CREATE_IMPL
#define MEM_POOL_CREATE_IMPL(objType, _mempool_helper)                                           \
fs::MemoryPoolHelper *objType::_mempool_helper = fs::Singleton<fs::MemoryPoolHelper, fs::AssistObjsDefs::NoDel>::GetInstance();

#undef MEM_POOL_CREATE_DEF
#undef MEM_POOL_CREATE_IMPL_DEF
#define  MEM_POOL_CREATE_DEF()  MEM_POOL_CREATE(_mempool_helper)
#define MEM_POOL_CREATE_IMPL_DEF(objType)    MEM_POOL_CREATE_IMPL(objType, _mempool_helper)
#endif
