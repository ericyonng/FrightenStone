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
#ifndef __Frame_Include_FrightenStone_Common_ObjPool_Impl_ObjPoolHelper_H__
#define __Frame_Include_FrightenStone_Common_ObjPool_Impl_ObjPoolHelper_H__

#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/asyn/asyn.h"
#include "FrightenStone/common/objpool/Defs/ObjPoolDefs.h"
#include "FrightenStone/common/objpool/Defs/ObjAlloctor.h"
#include "FrightenStone/common/objpool/Defs/AlloctorNode.h"
#include "FrightenStone/common/assist/utils/Impl/RTTIUtil.h"

FS_NAMESPACE_BEGIN

// 支持线程安全 尽量设置线程不安全避免加锁开销,不支持stl容器等自带分配器的对象
template<typename ObjType>
class ObjPoolHelper
{
public:
    ObjPoolHelper(size_t objAmount);
    virtual ~ObjPoolHelper();

public:
    size_t GetMemleakObjNum() const;
    const char *GetObjName() const;
    size_t GetMemleakBytes() const;
    size_t GetPoolBytesOccupied() const;
    size_t PrintObjPool(Int64 &poolOccupiedBytes, Int64 &objInUsed, const char *extStr);

    // 分配器行为接口
    IObjAlloctor<ObjType> *operator->();
    const IObjAlloctor<ObjType> *operator->() const;

public:
    Locker _locker;
    IObjAlloctor<ObjType> *_alloctor;
};

FS_NAMESPACE_END

#include "FrightenStone/common/objpool/Impl/ObjPoolHelperImpl.h"

/// 内存池创建对象便利宏
// 声明中需要添加 便利宏不支持基本数据类型 基本数据类型请直接定义一个helper对象
#undef  OBJ_POOL_CREATE
#define OBJ_POOL_CREATE(ObjType, _objpool_helper)                                                                   \
public:                                                                                                             \
        void  *operator new(size_t bytes)       { return _objpool_helper->_alloctor->Alloc();}                      \
        void   operator delete(void *ptr)       { _objpool_helper->_alloctor->Free(ptr);}                           \
        static size_t GetMemleakNum();                                                                              \
                                                                                                                    \
        template<typename... Args>                                                                                  \
        static ObjType *New(Args &&... args)                                                                        \
        {                                                                                                           \
            return _objpool_helper->_alloctor->New(std::forward<Args>(args)...);                                    \
        }                                                                                                           \
                                                                                                                    \
        static ObjType *NewWithoutConstruct()                                                                       \
        {                                                                                                           \
            return _objpool_helper->_alloctor->NewWithoutConstruct();                                               \
        }                                                                                                           \
                                                                                                                    \
        template<typename... Args>                                                                                  \
        static ObjType *NewByPtr(void *ptr, Args &&... args)                                                        \
        {                                                                                                           \
            return _objpool_helper->_alloctor->NewByPtr(ptr, std::forward<Args>(args)...);                          \
        }                                                                                                           \
                                                                                                                    \
        static void Delete(ObjType *ptr)                                                                            \
        {                                                                                                           \
            _objpool_helper->_alloctor->Delete(ptr);                                                                \
        }                                                                                                           \
                                                                                                                    \
        static void DeleteWithoutDestructor(ObjType *ptr)                                                           \
        {                                                                                                           \
            _objpool_helper->_alloctor->DeleteWithoutDestructor(ptr);                                               \
        }                                                                                                           \
                                                                                                                    \
        static fs::ObjPoolHelper<ObjType> *_objpool_helper

// 在实现文件中需要添加
#undef OBJ_POOL_CREATE_IMPL
#define OBJ_POOL_CREATE_IMPL(ObjType, _objpool_helper, objAmount)                                                   \
fs::ObjPoolHelper<ObjType> *ObjType::_objpool_helper = fs::Singleton<fs::ObjPoolHelper<ObjType>, fs::AssistObjsDefs::NoDel>::GetInstance(objAmount);       \
size_t ObjType::GetMemleakNum()                                                                                     \
{                                                                                                                   \
    return _objpool_helper->GetMemleakObjNum();                                                                     \
}

// 默认以_objPoolHelper命名对象池变量名
#undef OBJ_POOL_CREATE_DEF
#define OBJ_POOL_CREATE_DEF(ObjType)    OBJ_POOL_CREATE(ObjType, _objPoolHelper)

// 默认以_objPoolHelper命名对象池变量名
#undef OBJ_POOL_CREATE_DEF_IMPL
#define OBJ_POOL_CREATE_DEF_IMPL(ObjType, objAmount) OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)

#endif
