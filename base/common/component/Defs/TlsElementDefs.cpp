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
 * @file  : TlsElementDefs.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "base/common/component/Defs/TlsElementDefs.h"
#include "base/common/memorypool/memorypool.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(Tls_Rtti, 2, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);
OBJ_POOL_CREATE_DEF_IMPL(Tls_TestTls, 10, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);
OBJ_POOL_CREATE_DEF_IMPL(Tls_MemoryPool, 1, __DEF_OBJ_POOL_MAX_ALLOW_BYTES__);

Tls_MemoryPool::Tls_MemoryPool()
{
    _pool = IMemoryPoolMgr::NewInstance();
}

Tls_MemoryPool::~Tls_MemoryPool()
{
    if(_pool)
        _pool->FinishPool();

    FS_Release(_pool);
}

FS_NAMESPACE_END
