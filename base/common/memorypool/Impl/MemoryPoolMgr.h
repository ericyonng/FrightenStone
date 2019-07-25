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
 * @file  : MemoryPoolMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_MemoryPool_Impl_MemoryPoolMgr_H__
#define __Base_Common_MemoryPool_Impl_MemoryPoolMgr_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/memorypool/Defs/MemoryAlloctor.h"
#include "base/common/asyn/asyn.h"
#include "base/common/assist/assistobjs/Impl/Singleton.h"
#include "base/common/memorypool/Interface/IMemoryPoolMgr.h"

FS_NAMESPACE_BEGIN

class FS_String;

// TODO 抽象一个IMemoryPoolMgr 提供给外部使用
class BASE_EXPORT MemoryPoolMgr : public IMemoryPoolMgr
{
public:
    MemoryPoolMgr();
    virtual ~MemoryPoolMgr();

public:
    virtual Int32 InitPool();
    virtual void FinishPool();
    virtual void *Alloc(size_t bytes, const Byte8 *objName);
    virtual void  Free(void *ptr);
    virtual void  AddRef(void *ptr);
    virtual void Lock();
    virtual void Unlock();

private:
    void  _Init(size_t begin, size_t end, IMemoryAlloctor *alloctor);

private:
    // MemoryAlloctor<64, 10240>       _mem64;        // 64字节分配器
    MemoryAlloctor * _mem64;                         // 64字节分配器
    MemoryAlloctor *_mem128;                         // 64字节分配器
    MemoryAlloctor *_mem256;                         // 64字节分配器
    MemoryAlloctor *_mem512;                         // 64字节分配器
    MemoryAlloctor *_mem1024;                         // 64字节分配器
    MemoryAlloctor *_mem2048;                         // 64字节分配器
    MemoryAlloctor *_mem4096;                         // 64字节分配器
    MemoryAlloctor *_mem8192;                         // 64字节分配器
    MemoryAlloctor *_mem16K;                         // 64字节分配器
    MemoryAlloctor *_mem32K;                         // 64字节分配器
    MemoryAlloctor *_mem64K;                         // 64字节分配器
    IMemoryAlloctor                 *_alloctors[65536];  // 所有内存分配器

    Locker _locker;
};

inline void MemoryPoolMgr::Lock()
{
    _locker.Lock();
}

inline void MemoryPoolMgr::Unlock()
{
    _locker.Unlock();
}

FS_NAMESPACE_END

// #define g_MemoryPoolMgr fs::Singleton<fs::MemoryPoolMgr>::GetInstance()

#endif
