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
 * @file  : ObjPool.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/objpool/Impl/ObjPool.h"
#include "base/common/memorypool/Defs/MemoryBlock.h"


FS_NAMESPACE_BEGIN

ObjPool::ObjPool(size_t objSize, size_t objAmount)
    : _objSize(objSize)
    , _objAmount(objAmount)
    , _objAlloctor(NULL)

{

}

ObjPool::~ObjPool()
{
    _locker.Lock();
    Fs_SafeFree(_objAlloctor);
    _locker.Unlock();
}

void *ObjPool::Alloc(size_t bytes, const Byte8 *objName)
{
    // 判断是否内存池可分配
    if(bytes <= _objAlloctor->GetBlockSize())
        return  _objAlloctor->AllocMemory(bytes, objName);

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

void ObjPool::Free(void *ptr)
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

void ObjPool::AddRef(void *ptr)
{
    MemoryBlock *block = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(ptr) - sizeof(MemoryBlock));
    ++block->_ref;
}

FS_NAMESPACE_END
