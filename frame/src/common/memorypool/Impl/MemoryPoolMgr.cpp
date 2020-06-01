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
 * @file  : MemoryPoolMgr.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/memorypool/Impl/MemoryPoolMgr.h"
#include "FrightenStone/common/memorypool/Defs/MemoryBlock.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/status/status.h"
#include "FrightenStone/common/memleak/memleak.h"
#include "FrightenStone/common/log/Log.h"
#include "iostream"

#pragma region macro
// #ifndef BLOCK_AMOUNT_DEF
// #define BLOCK_AMOUNT_DEF 128       // 默认内存块数量
// #endif
#pragma endregion

FS_NAMESPACE_BEGIN

MemoryPoolMgr::MemoryPoolMgr(const BuildMemoryPoolMgr &buildData)
    :_isInit{false}
    ,_printCallback(NULL)
    , _maxAllowOccupiedBytes{ buildData._maxAllowOccupied }
    , _maxAllocBlockBytes(__FS_MEMORY_ALIGN__(buildData._maxAllocBlockBytes))
    , _minAllocBlockBytes(buildData._minAllocBlockBytes)
    , _initBlockNum(buildData._initBlockNum)
{
    InitPool();
}

MemoryPoolMgr::~MemoryPoolMgr()
{
    FinishPool();
}

Int32 MemoryPoolMgr::InitPool()
{
    _locker.Lock();
    if(_isInit)
    {
        _locker.Unlock();
        return StatusDefs::Success;
    }

    // 设定内存范围
    _memRanges.resize(_maxAllocBlockBytes);
    _curThreadId = SystemUtil::GetCurrentThreadId();
    size_t multi = _minAllocBlockBytes;
    for(size_t i = 0; i < _maxAllocBlockBytes; )
    {
        auto updateMemPoolOccupied = DelegatePlusFactory::Create(this, &MemoryPoolMgr::_UpdateMemPoolOccupied);

        // i-i+multi容量范围全部分配i + multi的内存块,默认初始化BLOCK_AMOUNT_DEF个内存块,
        // updateMemPoolOccupied为了监控内存池占用情况
        // _canCreateNewNode判断是否可再创建内存块,避免内存池不受限制的膨胀,不可创建的话就会使用系统的创建
        _Init(i, i + multi, new MemoryAlloctor(i + multi, _initBlockNum, updateMemPoolOccupied, &_canCreateNewNode));
        //_UpdateMemPoolOccupied((i + multi) * _initBlockNum);

        // 下一次分配的是原来的2倍的内存 64-128-256-512-...
        i += multi;
        multi = i;
    }
    _isInit = true;
    _locker.Unlock();

    _RegisterPrintCallback();

    return StatusDefs::Success;
}

void MemoryPoolMgr::FinishPool()
{
    if(!_isInit)
        return;

    _UnRegisterPrintCallback(_curThreadId);

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
    if(bytes < _maxAllocBlockBytes)
    {
        auto ptr = _memRanges[bytes]->AllocMemory(bytes);
        if(ptr)
            return ptr;
    }

    // 非内存池分配
    auto alignBytes = __FS_MEMORY_ALIGN__(bytes);
    alignBytes += sizeof(MemoryBlock);

    char *cache = reinterpret_cast<char *>(::malloc(alignBytes));
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(cache);
    block->_isInPool = false;
    block->_ref = 1;
    block->_alloctor = 0;
    block->_nextBlock = 0;
    block->_objSize = bytes;
    return  cache + sizeof(MemoryBlock);
}

void *MemoryPoolMgr::Realloc(void *ptr, size_t bytes)
{
    // 1.判断是否在内存池中
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
    if(UNLIKELY(!block->_isInPool))
    {
        auto alignBytes = __FS_MEMORY_ALIGN__(bytes);
        alignBytes += sizeof(MemoryBlock);

        char *newCache = reinterpret_cast<char *>(::realloc(block, alignBytes));
        block = reinterpret_cast<MemoryBlock *>(newCache);
        block->_isInPool = false;
        block->_ref = 1;
        block->_alloctor = NULL;
        block->_nextBlock = NULL;
        block->_objSize = bytes;
        return  newCache + sizeof(MemoryBlock);
    }

    // 2.原来内存块够用则重复利用
    if(block->_alloctor->GetEffectiveBlockSize() >= bytes)
        return ptr;

    // 3.不够用则先分配合适的内存块大小 再将数据拷贝到新的，然后释放原来的
    auto newCache = Alloc(bytes);
    ::memcpy(newCache, ptr, block->_alloctor->GetEffectiveBlockSize());
    block->_alloctor->FreeMemory(ptr);
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

void MemoryPoolMgr::PrintMemPoolInfo() const
{
    size_t totalOccupiedBytes = 0;
    size_t bytesInUsed = 0;
    g_Log->mempool("threadId[%llu] mem pool total info:", (UInt64)(_curThreadId));
    for(auto &alloctor : _allAlloctors)
    {
        alloctor->PrintMemInfo();
        totalOccupiedBytes += alloctor->GetOccupiedBytes();
        bytesInUsed += alloctor->GetInUsedBytes();
    }
    g_Log->mempool("mem pool total occupied bytes[%llu] maxAllowBytes[%llu],in used bytes[%llu]"
                   , totalOccupiedBytes, (UInt64)(_maxAllowOccupiedBytes), bytesInUsed);
    g_Log->mempool("=========================================================");
}

void MemoryPoolMgr::_Init(size_t begin, size_t end, IMemoryAlloctor *alloctor)
{
    alloctor->InitMemory();
    for(size_t i = begin; i < end; ++i)
        _memRanges[i] = alloctor;

    _allAlloctors.push_back(alloctor);
}

void MemoryPoolMgr::_FreePoolPtr(void *ptr)
{
    MemoryBlock *block = reinterpret_cast<MemoryBlock *>( reinterpret_cast<char*>
        ( reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock) ) );
    block->_alloctor->FreeMemory(ptr);
}

void MemoryPoolMgr::_RegisterPrintCallback()
{
    _printCallback = const_cast<IDelegate<void> *>(DelegatePlusFactory::Create(this, &MemoryPoolMgr::PrintMemPoolInfo));
    MemleakMonitor::GetInstance()->RegisterMemPoolPrintCallback(_curThreadId, _printCallback);
}

void MemoryPoolMgr::_UnRegisterPrintCallback(UInt64 threadId)
{
    MemleakMonitor::GetInstance()->UnRegisterMemPoolPrintCallback(threadId, _printCallback);
    FS_Release(_printCallback);
}

FS_NAMESPACE_END


