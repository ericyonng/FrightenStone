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
 * @file  : IMemoryPoolMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/6
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_MemoryPool_Interface_IMemoryPoolMgr_H__
#define __Base_Common_MemoryPool_Interface_IMemoryPoolMgr_H__
#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/SingletonAgency.h"

FS_NAMESPACE_BEGIN

class FS_String;

class BASE_EXPORT IMemoryPoolMgr
{
public:
    IMemoryPoolMgr();
    virtual ~IMemoryPoolMgr();
    virtual void Release();

public:
    static IMemoryPoolMgr *GetInstance();
    static IMemoryPoolMgr *NewInstance();
    virtual Int32 InitPool() = 0;
    virtual void FinishPool() = 0;
    template<typename ObjType>
    ObjType *Alloc(size_t bytes);
    virtual void *Alloc(size_t bytes) = 0;
    template<typename ObjType>
    ObjType *Realloc(void *ptr, size_t bytes);
    virtual void *Realloc(void *ptr, size_t bytes) = 0;
    virtual void  Free(void *ptr) = 0;
    virtual void  AddRef(void *ptr) = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual void PrintMemPoolInfo() const = 0;
    virtual void SetMaxAllowOccupiedBytes(UInt64 maxAllowBytes) = 0;
};

template<typename ObjType>
inline ObjType *IMemoryPoolMgr::Alloc(size_t bytes)
{
    return reinterpret_cast<ObjType *>(Alloc(bytes));
}

template<typename ObjType>
inline ObjType *IMemoryPoolMgr::Realloc(void *ptr, size_t bytes)
{
    return reinterpret_cast<ObjType *>(Realloc(ptr, bytes));
}
FS_NAMESPACE_END

// 不建议适用全局内存池建议使用线程局部内存池
extern BASE_EXPORT fs::IMemoryPoolMgr *g_MemoryPool;
// #define g_MemoryPool  fs::IMemoryPoolMgr::GetInstance()

#endif
