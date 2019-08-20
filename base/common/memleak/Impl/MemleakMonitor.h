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
#ifndef __Base_Common_Memleak_Impl_MemleakMonitor_H__
#define __Base_Common_Memleak_Impl_MemleakMonitor_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/component/Impl/FS_String.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ThreadPool;
class BASE_EXPORT MemleakMonitor
{
public:
    MemleakMonitor();
    virtual ~MemleakMonitor();

    static MemleakMonitor *GetInstance();
    void RegisterObjPoolCallback(const char *name, IDelegatePlus<size_t, Int64 &> *callback);
    void UnRegisterObjPool(const char *name);
    void RegisterMemPoolPrintCallback(const IDelegatePlus<void> *callback);
    void UnRegisterMemPoolPrintCallback();

    void PrintObjPoolInfo() const;
    // 请使用typeid作为objName
    void PrintObjPoolInfo(const char *objName) const;

    // print objpool/mempool
    void PrintPoolAll() const;

private:
    void _PrintInfoPerSeconds(const FS_ThreadPool *pool);

private:
    static Locker _locker;
    std::map<FS_String, std::vector<IDelegatePlus<size_t, Int64 &> *> *> _objNameRefPrintCallback;
    const IDelegatePlus<void> *_memPoolPrintCallback;

    FS_ThreadPool *_printInfoPool;
};

FS_NAMESPACE_END

extern BASE_EXPORT fs::MemleakMonitor *g_MemleakMonitor;

#endif
