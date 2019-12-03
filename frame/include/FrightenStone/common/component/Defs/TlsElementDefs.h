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
 * @file  : TlsElementDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */

#ifndef __Frame_Include_FrightenStone_Common_Component_Defs_TlsElementDefs_H__
#define __Frame_Include_FrightenStone_Common_Component_Defs_TlsElementDefs_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Defs/ITlsBase.h"
#include "FrightenStone/common/memorypool/memorypool.h"

// 类型识别缓冲大小
#ifndef __FS_RTTI_BUF_SIZE__
#define __FS_RTTI_BUF_SIZE__    512
#endif

FS_NAMESPACE_BEGIN

class IMemoryPoolMgr;

// 所有局部存储对象请派生于ITlsBase
// 线程局部存储需要存储的对象类型
class BASE_EXPORT TlsElemType
{
public:
    enum
    {
        Begin = 0,
        Tls_Rtti,           // 类型识别
        Tls_TestTls,        // 测试tls
        Tls_MemoryPool,     // 内存池
        End,
    };
};

// 类型识别线程局部存储
struct BASE_EXPORT Tls_Rtti : public ITlsBase
{
    OBJ_POOL_CREATE_DEF(Tls_Rtti);

    Tls_Rtti();
    virtual ~Tls_Rtti();
    virtual void Release();

    char rtti[__FS_RTTI_BUF_SIZE__];
};

// 测试线程局部存储
struct BASE_EXPORT Tls_TestTls : public ITlsBase
{
    OBJ_POOL_CREATE_DEF(Tls_TestTls);

    Tls_TestTls();
    virtual ~Tls_TestTls();

    Int32 count;
};

// 线程局部内存池，仅限于线程生命周期内使用，其他情况会出现未定义
struct BASE_EXPORT Tls_MemoryPool : public ITlsBase
{
    OBJ_POOL_CREATE_DEF(Tls_MemoryPool);

    Tls_MemoryPool();
    virtual ~Tls_MemoryPool();

    IMemoryPoolMgr *_pool;
};
FS_NAMESPACE_END

#include "FrightenStone/common/component/Defs/TlsElementDefsImpl.h"

#endif
