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

FS_NAMESPACE_BEGIN

MemoryPoolMgr::MemoryPoolMgr()
    :_alloctors{NULL}
{
}

MemoryPoolMgr::~MemoryPoolMgr()
{

}

Int32 MemoryPoolMgr::InitPool()
{
    _Init(0, 64, &_mem64);
    _Init(64, 128, &_mem128);
    _Init(128, 256, &_mem256);
    _Init(256, 512, &_mem512);
    _Init(512, 1024, &_mem1024);
    _Init(1024, 2048, &_mem2048);
    _Init(2048, 4096, &_mem4096);
    _Init(4096, 8192, &_mem8192);
    _Init(8192, 16384, &_mem16K);
    _Init(16384, 32768, &_mem32K);
    _Init(32768, 65536, &_mem64K);
    return StatusDefs::Success;
}

void MemoryPoolMgr::FinishPool()
{
    _locker.Lock();
    _mem64._FinishMemory();
    _mem128._FinishMemory();
    _mem256._FinishMemory();
    _mem512._FinishMemory();
    _mem1024._FinishMemory();
    _mem2048._FinishMemory();
    _mem4096._FinishMemory();
    _mem8192._FinishMemory();
    _mem16K._FinishMemory();
    _mem32K._FinishMemory();
    _mem64K._FinishMemory();
    _locker.Unlock();
}

void *MemoryPoolMgr::Alloc(size_t bytes, const FS_String &objName)
{
    // 判断是否内存池可分配
    if(bytes < sizeof(_alloctors) / sizeof(IMemoryAlloctor *))
    {
        return  _alloctors[bytes]->AllocMemory(bytes, objName);
    }
    else
    {
        char *cache = reinterpret_cast<char *>(::malloc(bytes + sizeof(MemoryBlock)));
        MemoryBlock *block = reinterpret_cast<MemoryBlock*>(cache);
        block->_isInPool = false;
        block->_ref = 1;
        block->_alloctor = 0;
        block->_nextBlock = 0;
        auto len = sprintf(block->_objName, "%s", objName.c_str());
        if(len > 0)
            block->_objName[BUFFER_LEN256 > len ? len : BUFFER_LEN256 - 1] = 0;
        else
            block->_objName[0] = 0;
        return  cache + sizeof(MemoryBlock);
    }
}

void MemoryPoolMgr::Free(void *ptr)
{
    char *mem = reinterpret_cast<char*>(ptr);
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(mem - sizeof(MemoryBlock)));
    if(block->_isInPool)
    {
        block->_alloctor->FreeMemory(ptr);
    }
    else if(--block->_ref == 0)
    {
        ::free(ptr);
    }
}

void MemoryPoolMgr::AddRef(void *ptr)
{
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
    ++block->_ref;
}

void MemoryPoolMgr::_Init(size_t begin, size_t end, IMemoryAlloctor *alloctor)
{
    alloctor->_InitMemory();
    for(size_t i = begin; i < end; ++i)
        _alloctors[i] = alloctor;
}

FS_NAMESPACE_END
