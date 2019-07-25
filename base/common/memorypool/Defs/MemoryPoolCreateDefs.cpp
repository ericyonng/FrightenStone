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
 * @file  : MemoryPoolCreateDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/7
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/memorypool/Defs/MemoryPoolCreateDefs.h"
#include "base/common/memorypool/Interface/IMemoryPoolMgr.h"

FS_NAMESPACE_BEGIN

MemoryPoolHelper::MemoryPoolHelper(const Byte8 *objName)
    :_objName{0}
{
    auto len = sprintf(_objName, "%s", objName);
    if(len < 0)
        _objName[0] = 0;
    else
    {
        _objName[len >= BUFFER_LEN256 ? (BUFFER_LEN256 - 1) : len] = 0;
    }
}

MemoryPoolHelper::~MemoryPoolHelper()
{

}

void *MemoryPoolHelper::Alloc(size_t bytes)
{
    g_MemoryPool->Lock();
    auto ptr = g_MemoryPool->Alloc(bytes, _objName);
    g_MemoryPool->Unlock();
    return ptr;
}

void MemoryPoolHelper::Free(void *ptr)
{
    g_MemoryPool->Lock();
    g_MemoryPool->Free(ptr);
    g_MemoryPool->Unlock();
}

void MemoryPoolHelper::AddRef(void *ptr)
{
    g_MemoryPool->Lock();
    g_MemoryPool->AddRef(ptr);
    g_MemoryPool->Unlock();
}

FS_NAMESPACE_END
