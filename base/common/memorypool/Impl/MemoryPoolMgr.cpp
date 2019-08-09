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
 * @file  : MemoryPoolMgr.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/memorypool/Impl/MemoryPoolMgr.h"
#include "base/common/memorypool/Defs/MemoryBlock.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/status/status.h"
#include "iostream"

#pragma region macro
// #ifndef BLOCK_AMOUNT_DEF
// #define BLOCK_AMOUNT_DEF 128       // 默认内存块数量
// #endif
#pragma endregion

FS_NAMESPACE_BEGIN

MemoryPoolMgr::MemoryPoolMgr()
    :_alloctors{NULL}
    ,_isInit(false)
{
    InitPool();
}

MemoryPoolMgr::~MemoryPoolMgr()
{

}

Int32 MemoryPoolMgr::InitPool()
{
    _locker.Lock();
    if(_isInit)
    {
        _locker.Unlock();
        return StatusDefs::Success;
    }
    Int32 multi = __MEMORY_POOL_MINIMUM_BLOCK__;
    for(Int32 i = 0; i < __MEMORY_POOL_MAXBLOCK_LIMIT__; )
    {
        _Init(i, i + multi, new MemoryAlloctor(i + multi, BLOCK_AMOUNT_DEF));

        // 下一次分配的是原来的2倍的内存 64-128-256-512-...
        i += multi;
        multi = i;
    }
    _isInit = true;
    _locker.Unlock();

    return StatusDefs::Success;
}

void MemoryPoolMgr::FinishPool()
{
    _locker.Lock();
    _isInit = false;
    for(auto iter : _allAlloctors)
        Fs_SafeFree(iter);
    _allAlloctors.clear();
    _locker.Unlock();
}

void *MemoryPoolMgr::Alloc(size_t bytes)
{
    // 判断是否内存池可分配
    if(bytes < __MEMORY_POOL_MAXBLOCK_LIMIT__ / sizeof(IMemoryAlloctor *))
    {
        return  _alloctors[bytes]->AllocMemory(bytes);
    }
    else
    {
        auto alignBytes = bytes / __MEMORY_POOL_ALIGN_BYTES__ * __MEMORY_POOL_ALIGN_BYTES__ + (bytes%__MEMORY_POOL_ALIGN_BYTES__ ? __MEMORY_POOL_ALIGN_BYTES__ : 0);
        alignBytes = bytes + sizeof(MemoryBlock);

        char *cache = reinterpret_cast<char *>(::malloc(alignBytes));
        MemoryBlock *block = reinterpret_cast<MemoryBlock*>(cache);
        block->_isInPool = false;
        block->_ref = 1;
        block->_alloctor = 0;
        block->_nextBlock = 0;
        return  cache + sizeof(MemoryBlock);
    }
}

void *MemoryPoolMgr::Realloc(void *ptr, size_t bytes)
{
    // 1.判断是否在内存池中
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
    if(!block->_isInPool)
    {
        auto alignBytes = bytes / __MEMORY_POOL_ALIGN_BYTES__ * __MEMORY_POOL_ALIGN_BYTES__ + (bytes%__MEMORY_POOL_ALIGN_BYTES__ ? __MEMORY_POOL_ALIGN_BYTES__ : 0);
        alignBytes = bytes + sizeof(MemoryBlock);

        char *newCache = reinterpret_cast<char *>(::realloc(block, alignBytes));
        block = reinterpret_cast<MemoryBlock *>(newCache);
        block->_isInPool = false;
        block->_ref = 1;
        block->_alloctor = 0;
        block->_nextBlock = 0;
        return  newCache + sizeof(MemoryBlock);
    }

    // 2.原来内存块够用则重复利用
    if(block->_alloctor->GetEffectiveBlockSize() >= bytes)
        return ptr;

    // 3.不够用则先分配合适的内存块大小 再将数据拷贝到新的，然后释放原来的
    auto newCache = Alloc(bytes);
    ::memcpy(newCache, ptr, block->_alloctor->GetEffectiveBlockSize());
    Free(ptr);
    return newCache;
}

void MemoryPoolMgr::Free(void *ptr)
{
    char *mem = reinterpret_cast<char*>(ptr);
    MemoryBlock *block = reinterpret_cast<MemoryBlock *>(reinterpret_cast<char*>(mem - sizeof(MemoryBlock)));
    if(block->_isInPool)
    {
        block->_alloctor->FreeMemory(ptr);
    }
    else if(--block->_ref == 0)
    {
        ::free(block);
    }
}

void MemoryPoolMgr::AddRef(void *ptr)
{
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
    ++block->_ref;
}

void MemoryPoolMgr::_Init(size_t begin, size_t end, IMemoryAlloctor *alloctor)
{
    alloctor->InitMemory();
    for(size_t i = begin; i < end; ++i)
        _alloctors[i] = alloctor;

    _allAlloctors.push_back(alloctor);
}

FS_NAMESPACE_END
