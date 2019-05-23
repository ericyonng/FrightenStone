#ifndef __Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
#define __Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
/**
* @file SmartPtr.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<base\exportbase.h>
#include "base/common/assist/assistobjs/Defs/AssistObjsDefs.h"

FS_NAMESPACE_BEGIN

// 智能对象
template<typename T, AssistObjsDefs::DelMethods delMethod = AssistObjsDefs::Delete>
class SmartPtr
{
    #pragma region 构造/析构/弹出指针/释放（解除接管）
    SmartPtr(const SmartPtr<T, delMethod> &obj) = delete;
    SmartPtr<T, delMethod> &operator =(const SmartPtr<T, delMethod> &ptr) = delete;
public:
    SmartPtr();
    SmartPtr(T *ptr);
    SmartPtr(SmartPtr<T, delMethod> &&obj);    // 外部T *会先构造一个SmartPtr临时对象再进行构造新对象
    virtual ~SmartPtr();
    T *pop();
    void Release();
    #pragma endregion

    #pragma region 操作符重载 指针符号, 下标索引等
public:
    SmartPtr<T, delMethod> &operator =(T *ptr);
    SmartPtr<T, delMethod> &operator =(SmartPtr<T, delMethod> &&ptr) noexcept;
    operator void *();
    operator const void *() const;

    operator T *();
    operator const T *() const;
    // operator SmartPtr<T, delMethod> &();
    T *operator->();
    const T *operator->() const;

    T& operator [](int index);    // 无越界检测
    const T& operator [](int index) const;    // 无越界检测
    T& operator *();
    const T& operator *() const;

    explicit operator bool();
    #pragma endregion

    #pragma region 数据成员 指针/释放对象
protected:
    T *_ptr;
    AutoDelObj<T, delMethod> _delObj;
    #pragma endregion
};

FS_NAMESPACE_END

#include"base/common/assist/assistobjs/Impl/SmartPtrImpl.h"

#endif // !__Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
