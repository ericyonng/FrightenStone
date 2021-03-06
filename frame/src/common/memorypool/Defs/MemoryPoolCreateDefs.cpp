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
 * @file  : MemoryPoolCreateDefs.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/7
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/memorypool/Defs/MemoryPoolCreateDefs.h"
#include "FrightenStone/common/memorypool/Interface/IMemoryPoolMgr.h"
#include "FrightenStone/common/log/Log.h"

FS_NAMESPACE_BEGIN

MemoryPoolHelper::MemoryPoolHelper()
{
}

MemoryPoolHelper::~MemoryPoolHelper()
{
    g_Log->i<MemoryPoolHelper>(_LOGFMT_("MemoryPoolHelper destructor"));
    g_Log->FlushAllFile();
}

void *MemoryPoolHelper::Alloc(size_t bytes)
{
    g_MemoryPool->Lock();
    auto ptr = g_MemoryPool->Alloc(bytes);
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
