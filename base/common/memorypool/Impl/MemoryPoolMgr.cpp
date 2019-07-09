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

#pragma region macro
#define BLOCK_AMOUNT_DEF 10240      // 默认内存块数量
#pragma endregion

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
    _locker.Lock();
    _Init(0, 64,        _mem64 = new MemoryAlloctor(64, BLOCK_AMOUNT_DEF));
    _Init(64, 128,      _mem128 = new MemoryAlloctor(64, BLOCK_AMOUNT_DEF));
    _Init(128, 256,     _mem256 = new MemoryAlloctor(256, BLOCK_AMOUNT_DEF));
    _Init(256, 512,     _mem512 = new MemoryAlloctor(512, BLOCK_AMOUNT_DEF));
    _Init(512, 1024,    _mem1024 = new MemoryAlloctor(1024, BLOCK_AMOUNT_DEF));
    _Init(1024, 2048,   _mem2048 = new MemoryAlloctor(2048, BLOCK_AMOUNT_DEF));
    _Init(2048, 4096,   _mem4096 = new MemoryAlloctor(4096, BLOCK_AMOUNT_DEF));
    _Init(4096, 8192,   _mem8192 = new MemoryAlloctor(8192, BLOCK_AMOUNT_DEF));
    _Init(8192, 16384,  _mem16K = new MemoryAlloctor(16384, BLOCK_AMOUNT_DEF));
    _Init(16384, 32768, _mem32K = new MemoryAlloctor(32768, BLOCK_AMOUNT_DEF));
    _Init(32768, 65536, _mem64K = new MemoryAlloctor(65536, BLOCK_AMOUNT_DEF));
    _locker.Unlock();

    return StatusDefs::Success;
}

void MemoryPoolMgr::FinishPool()
{
    _locker.Lock();
    _mem64->FinishMemory();
    _mem128->FinishMemory();
    _mem256->FinishMemory();
    _mem512->FinishMemory();
    _mem1024->FinishMemory();
    _mem2048->FinishMemory();
    _mem4096->FinishMemory();
    _mem8192->FinishMemory();
    _mem16K->FinishMemory();
    _mem32K->FinishMemory();
    _mem64K->FinishMemory();
    Fs_SafeFree(_mem64);
    Fs_SafeFree(_mem128);
    Fs_SafeFree(_mem256);
    Fs_SafeFree(_mem512);
    Fs_SafeFree(_mem1024);
    Fs_SafeFree(_mem2048);
    Fs_SafeFree(_mem4096);
    Fs_SafeFree(_mem8192);
    Fs_SafeFree(_mem16K);
    Fs_SafeFree(_mem32K);
    Fs_SafeFree(_mem64K);
    _locker.Unlock();
}

void *MemoryPoolMgr::Alloc(size_t bytes, const Byte8 *objName)
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
        auto len = sprintf(block->_objName, "%s", objName);
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
    alloctor->InitMemory();
    for(size_t i = begin; i < end; ++i)
        _alloctors[i] = alloctor;
}

FS_NAMESPACE_END
