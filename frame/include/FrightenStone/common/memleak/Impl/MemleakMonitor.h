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
 * @file  : MemleakMonitor.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/07/31
 * @brief : 内存泄漏监视器，支持对象池中创建的对象
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Memleak_Impl_MemleakMonitor_H__
#define __Frame_Include_FrightenStone_Common_Memleak_Impl_MemleakMonitor_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/component/Impl/FS_Delegate.h"
#include "FrightenStone/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class BASE_EXPORT MemleakMonitor
{
public:
    MemleakMonitor();
    virtual ~MemleakMonitor();

    static MemleakMonitor *GetInstance();
    Int32 BeforeStart(UInt64 maxAllowObjPoolBytes, UInt64 maxAllowMemoryPoolBytes);
    void Start();
    void Finish();

    void RegisterObjPoolCallback(const char *name, IDelegate<size_t, Int64 &, Int64 &, const char *> *callback);
    void UnRegisterObjPool(const char *name);
    void RegisterMemPoolPrintCallback(UInt64 threadId, const IDelegate<void> *callback);
    void UnRegisterMemPoolPrintCallback(UInt64 threadId);

    void RegisterObjPoolModifyMaxAllowBytesCallback(IDelegate<void, UInt64> *callback);

    void PrintObjPoolInfo() const;
    // 请使用typeid作为objName
    void PrintObjPoolInfo(const char *objName) const;
    // 打印系统占用内存信息
    void PrintSysMemoryInfo() const;

    // print objpool/mempool
    void PrintPoolAll() const;

private:
    void _PrintInfoPerSeconds(FS_ThreadPool *pool);

private:
    static Locker _locker;
    std::map<FS_String, std::vector<IDelegate<size_t, Int64 &, Int64 &, const char *> *> *> _objNameRefPrintCallback;
    std::list<IDelegate<void, UInt64> *> _objPoolSetMaxAllowOccupiedBytes;
    std::map<UInt64, const IDelegate<void> *> _threadIdRefMemPoolPrintCallback;

    FS_ThreadPool *_printInfoPool;
};

FS_NAMESPACE_END

extern BASE_EXPORT fs::MemleakMonitor *g_MemleakMonitor;

#endif
