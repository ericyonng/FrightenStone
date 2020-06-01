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
 * @file  : MemoryPoolMgr.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/5
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_MemoryPool_Impl_MemoryPoolMgr_H__
#define __Frame_Include_FrightenStone_Common_MemoryPool_Impl_MemoryPoolMgr_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/memorypool/Defs/MemoryAlloctor.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/assist/assistobjs/Impl/Singleton.h"
#include "FrightenStone/common/memorypool/Interface/IMemoryPoolMgr.h"
#include "FrightenStone/common/memorypool/Defs/MemoryPoolDefs.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"

FS_NAMESPACE_BEGIN

class FS_String;

// TODO 抽象一个IMemoryPoolMgr 提供给外部使用 最大支持__MEMORY_POOL_MAX_EXPAND_BYTES__ 内存池大小
class BASE_EXPORT MemoryPoolMgr : public IMemoryPoolMgr
{
public:
    // maxAllocUinitBytes 最大分配内存块大小
    MemoryPoolMgr(const BuildMemoryPoolMgr &buildData);
    virtual ~MemoryPoolMgr();

public:
    virtual Int32 InitPool();
    virtual void FinishPool();
    virtual void *Alloc(size_t bytes);
    virtual void *Realloc(void *ptr, size_t bytes);
    virtual void  Free(void *ptr);
    virtual void  AddRef(void *ptr);
    virtual void Lock();
    virtual void Unlock();
    virtual void PrintMemPoolInfo() const;
    virtual void SetMaxAllowOccupiedBytes(UInt64 maxAllowBytes);

private:
    void  _Init(size_t begin, size_t end, IMemoryAlloctor *alloctor);
    void _FreePoolPtr(void *ptr);
    void _RegisterPrintCallback();
    void _UnRegisterPrintCallback(UInt64 threadId);
    void _UpdateMemPoolOccupied(size_t newOccupiedBytes);

private:
    std::vector<IMemoryAlloctor *> _memRanges;          // 某范围的内存分配器 下标用要分配的内存大小索引到分配器, 
                                                        // 内存分配器O(1)复杂度，默认最大支持__MEMORY_POOL_MAXBLOCK_LIMIT__的内存分配
                                                        // ,其他内存由系统分配
    std::vector<IMemoryAlloctor *> _allAlloctors;       // 用于释放, x-y内存范围会公用一个alloctor，vector中只存在一份
    std::atomic<bool> _isInit;
    Locker _locker;
    IDelegate<void> *_printCallback;

    // 内存参数
    std::atomic<size_t> _curTotalOccupiedBytes{0};      // 当前内存池占用的总字节数
    std::atomic<size_t> _maxAllowOccupiedBytes;         // 内存池最大允许占用
    const size_t _maxAllocBlockBytes;                   // 最大的块大小
    const size_t _minAllocBlockBytes;                   // 最小内存块大小 0-_minAllocBlockBytes的内存范围都会分配_minAllocBlockBytes大小内存块
    const size_t _initBlockNum;                         // 初始化时候分配的块大小

    std::atomic_bool _canCreateNewNode{ true };
    std::atomic<UInt64> _curThreadId{0};
};

inline void MemoryPoolMgr::Lock()
{
    _locker.Lock();
}

inline void MemoryPoolMgr::Unlock()
{
    _locker.Unlock();
}

inline void MemoryPoolMgr::SetMaxAllowOccupiedBytes(UInt64 maxAllowBytes)
{
    _maxAllowOccupiedBytes = maxAllowBytes;
}

inline void MemoryPoolMgr::_UpdateMemPoolOccupied(size_t newOccupiedBytes)
{
    _curTotalOccupiedBytes += newOccupiedBytes;
    if(!_canCreateNewNode)
        return;

    if(_curTotalOccupiedBytes >= _maxAllowOccupiedBytes)
        _canCreateNewNode = false;
}

FS_NAMESPACE_END

// #define g_MemoryPoolMgr fs::Singleton<fs::MemoryPoolMgr>::GetInstance()
// 强烈建银get出pool的真实指针避免直接使用本宏，消耗很大
// extern BASE_EXPORT fs::SingletonAgency<fs::MemoryPoolMgr> g_MemoryPoolAgency;

// #define g_MemoryPool g_MemoryPoolAgency

// template class BASE_EXPORT fs::Singleton<fs::MemoryPoolMgr, fs::AssistObjsDefs::NoDel>;
// #define g_MemoryPool  static_cast<fs::IMemoryPoolMgr *>(fs::Singleton<fs::MemoryPoolMgr, fs::AssistObjsDefs::NoDel>::GetInstance())

#endif
