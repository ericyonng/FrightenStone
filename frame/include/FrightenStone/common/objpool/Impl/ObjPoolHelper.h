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
 * @file  : ObjPoolHelper.h
 * @author: Eric Yonng<120453674@qq.com>
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

// 注意对象池创建的类其派生类也必须是对象池创建的否则会导致不可预料的内存问题
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
// 禁止直接使用 OBJ_POOL_CREATE 直接使用会大概率出现内存非法写入到其他对象内存区 ！！！！！！！！！！！！！！！！！！！！！
// 禁止直接使用 OBJ_POOL_CREATE 直接使用会大概率出现内存非法写入到其他对象内存区！！！！！！！！！！！！！！！！！！！！！
// 禁止直接使用 OBJ_POOL_CREATE 直接使用会大概率出现内存非法写入到其他对象内存区！！！！！！！！！！！！！！！！！！！！！
#pragma region WarningSeriously Dont Use objpoolcreate derectly!!!!!!!!!!
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
public:                                                                                                             \
        static fs::ObjPoolHelper<ObjType> *_objpool_helper

// 禁止直接使用 OBJ_POOL_CREATE_IMPL 直接使用会大概率出现内存非法写入到其他对象内存区 ！！！！！！！！！！！！！！
// 禁止直接使用 OBJ_POOL_CREATE_IMPL 直接使用会大概率出现内存非法写入到其他对象内存区！！！！！！！！！！！！！！！
// 禁止直接使用 OBJ_POOL_CREATE_IMPL 直接使用会大概率出现内存非法写入到其他对象内存区！！！！！！！！！！！！！！！
#undef OBJ_POOL_CREATE_IMPL
#define OBJ_POOL_CREATE_IMPL(ObjType, _objpool_helper, objAmount)                                                   \
fs::ObjPoolHelper<ObjType> *ObjType::_objpool_helper = new fs::ObjPoolHelper<ObjType>(objAmount);                   \
size_t ObjType::GetMemleakNum()                                                                                     \
{                                                                                                                   \
    return _objpool_helper->GetMemleakObjNum();                                                                     \
}                                                                                                                   
#pragma  endregion

/* 祖先类对象池创建 */
#pragma region ancestor
#undef OBJ_POOL_CREATE_ANCESTOR
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_ANCESTOR(ObjType)                                                                           \
OBJ_POOL_CREATE(ObjType, _objPoolHelper);                                                                           \
public:                                                                                                             \
    void ObjType##_objpooldetect();

#undef OBJ_POOL_CREATE_ANCESTOR_IMPL
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_ANCESTOR_IMPL(ObjType, objAmount)                                                           \
OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)                                                            \
void ObjType::ObjType##_objpooldetect(){}
#pragma endregion

/* 派生类对象池创建 */
#pragma region derive
#undef OBJ_POOL_CREATE_DERIVE
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_DERIVE(ObjType, Parent)                                                                     \
OBJ_POOL_CREATE(ObjType, _objPoolHelper);                                                                           \
public:                                                                                                             \
    void ObjType##_objpooldetect(){ Parent::Parent##_objpooldetect(); }                                                 

#undef OBJ_POOL_CREATE_DERIVE_IMPL
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_DERIVE_IMPL(ObjType, objAmount)                                                             \
OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)                                                            
#pragma endregion

/* 祖先类是模版类的 */
#pragma region ancestor
#undef OBJ_POOL_CREATE_TEMPLATE_ANCESTOR
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
// TempClassName是不带模版参数的模版类名字
#define OBJ_POOL_CREATE_TEMPLATE_ANCESTOR(ObjType, TempClassName)                                                   \
OBJ_POOL_CREATE(ObjType, _objPoolHelper);                                                                           \
public:                                                                                                             \
    void TempClassName##_objpooldetect();

#undef OBJ_POOL_CREATE_TEMPLATE_ANCESTOR_IMPL
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_TEMPLATE_ANCESTOR_IMPL(ObjType, objAmount)                                                  \
OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)                                                            \
void ObjType::ObjType##_objpooldetect(){}
#pragma endregion

/* 派生类父类是模版类的对象池创建 */
#pragma region derive
#undef OBJ_POOL_CREATE_DERIVE_BASE_TEMPLATE
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
// ParentTempClassName是父类不戴模版参数的类名
#define OBJ_POOL_CREATE_DERIVE_BASE_TEMPLATE(ObjType, Parent, ParentTempClassName)                                  \
OBJ_POOL_CREATE(ObjType, _objPoolHelper);                                                                           \
public:                                                                                                             \
    void ObjType##_objpooldetect(){ Parent::ParentTempClassName##_objpooldetect(); }                                 

#undef OBJ_POOL_CREATE_DERIVE_BASE_TEMPLATE_IMPL
// 请祖先类与派生类选择性使用,
// 使用各自类的_objpooldetect自检函数，可以实现在编译器检查出基类是不是对象池创建的
// 若祖先类是对象池创建的那么派生出来的子类必须是对象池创建的,
// 否则会出现当前对象非法访问本类其他实例内存区，造成不可挽回的内存非法访问，
#define OBJ_POOL_CREATE_DERIVE_BASE_TEMPLATE_IMPL(ObjType, objAmount)                                               \
OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)                                                            
#pragma endregion

// #undef OBJ_POOL_CREATE_DEF
// // 默认以_objPoolHelper命名对象池变量名
// // 注意对象池创建的类其派生类也必须是对象池创建的否则会导致不可预料的内存问题
// #define OBJ_POOL_CREATE_DEF(ObjType)    OBJ_POOL_CREATE(ObjType, _objPoolHelper)
// 
// // 默认以_objPoolHelper命名对象池变量名
// // 注意对象池创建的类其派生类也必须是对象池创建的否则会导致不可预料的内存问题
// #undef OBJ_POOL_CREATE_DEF_IMPL
// #define OBJ_POOL_CREATE_DEF_IMPL(ObjType, objAmount) OBJ_POOL_CREATE_IMPL(ObjType, _objPoolHelper, objAmount)

#endif
