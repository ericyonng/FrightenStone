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
 * @file  : ObjPoolHelper.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/9
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_ObjPool_Impl_ObjPoolHelper_H__
#define __Base_Common_ObjPool_Impl_ObjPoolHelper_H__

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/BaseDefs.h"
#include "base/common/asyn/asyn.h"
#include "base/common/objpool/Defs/ObjAlloctor.h"
#include "base/common/objpool/Defs/AlloctorNode.h"

FS_NAMESPACE_BEGIN

template<typename ObjType>
class ObjPoolHelper
{
public:
    ObjPoolHelper(size_t objAmount);
    virtual ~ObjPoolHelper();

public:
    void *Alloc();
    void  Free(void *ptr);
    size_t GetMemleakObjNum() const;
    const char *GetObjName() const;
    size_t GetMemleakBytes() const;
    size_t GetPoolBytesOccupied() const;
    size_t PrintMemleak(Int64 &poolOccupiedBytes);

private:
    void _Lock();
    void _Unlock();

    Locker _locker;
    size_t _objAmount;
    IObjAlloctor<ObjType> *_alloctor;
};

FS_NAMESPACE_END

#include "base/common/objpool/Impl/ObjPoolHelperImpl.h"

/// 内存池创建对象便利宏
// 声明中需要添加
#undef  OBJ_POOL_CREATE
#define OBJ_POOL_CREATE(ObjType, _objpool_helper)                                                       \
public:                                                                                                 \
        void  *operator new(size_t bytes)       { return _objpool_helper.Alloc();}                      \
        void   operator delete(void *ptr)       { _objpool_helper.Free(ptr);}                           \
        static size_t GetMemleakNum();                                                                  \
                                                                                                        \
static fs::ObjPoolHelper<ObjType> _objpool_helper;

// 在实现文件中需要添加
#undef OBJ_POOL_CREATE_IMPL
#define OBJ_POOL_CREATE_IMPL(ObjType, _objpool_helper, objAmount)                                       \
fs::ObjPoolHelper<ObjType> ObjType::_objpool_helper(objAmount);                                         \
size_t ObjType::GetMemleakNum()                                                                         \
{                                                                                                       \
    return _objpool_helper.GetMemleakObjNum();                                                          \
}

#endif
