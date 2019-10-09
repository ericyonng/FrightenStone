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
 * @file  : FS_TlsTable.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Assist_Utils_Impl_Defs_FS_TlsTable_H__
#define __Base_Common_Assist_Utils_Impl_Defs_FS_TlsTable_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/assist/utils/Defs/ITlsBase.h"
#include "base/common/asyn/asyn.h"
#include "base/common/assist/assistobjs/Impl/Singleton.h"

FS_NAMESPACE_BEGIN

struct BASE_EXPORT ITlsBase;

// 请不要过度依赖于线程局部存储
// 请勿频繁创建
// 请保证线程周期内都不会释放,数组类型元素请在外部包装成派生于ITlsBase基类的结构体，再进行存储
class BASE_EXPORT FS_TlsTable
{
public:
    FS_TlsTable();
    ~FS_TlsTable();

public:
    template<typename ObjType>
    ObjType *GetElement(Int32 type);
    template<typename ObjType, typename... Args>
    ObjType *AllocElement(Int32 type, Args... args);
    void Erase(Int32 type);

private:
    std::map<Int32, ITlsBase *> _elementTypeRefElements;
};

class BASE_EXPORT ThreadTlsTableMgr
{
public:
    ThreadTlsTableMgr();
    ~ThreadTlsTableMgr();

public:
    FS_TlsTable *GetThisThreadTable();
    FS_TlsTable *GetAndCreateThisThreadTable();
    FS_TlsTable *CreateThisThreadTable();
    void FreeThisThreadTable();

private:
    Locker _guard;
    std::map<Int32, FS_TlsTable *> _threadIdRefTlsTable;
};

template class BASE_EXPORT Singleton<ThreadTlsTableMgr>;

FS_NAMESPACE_END

#include "base/common/assist/utils/Defs/FS_TlsTableImpl.h"

#define g_ThreadTlsTableMgr fs::Singleton<fs::ThreadTlsTableMgr>::GetInstance()

#endif