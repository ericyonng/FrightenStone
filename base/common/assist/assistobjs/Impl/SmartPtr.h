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

// ÖÇÄÜ¶ÔÏó
template<typename T, AssistObjsDefs::DelMethods delMethod = AssistObjsDefs::Delete>
class SmartPtr
{
    #pragma region constructor
    SmartPtr(const SmartPtr<T, delMethod> &obj) = delete;
    SmartPtr<T, delMethod> &operator =(const SmartPtr<T, delMethod> &ptr) = delete;
public:
    SmartPtr();
    SmartPtr(T *ptr);
    SmartPtr(SmartPtr<T, delMethod> &&obj);
    virtual ~SmartPtr();
    T *pop();
    void Release();
    #pragma endregion

    #pragma region operations
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

    T& operator [](int index);    // no out-range detect
    const T& operator [](int index) const;    // no out-range detect
    T& operator *();
    const T& operator *() const;

    explicit operator bool();
    #pragma endregion

    #pragma region member
protected:
    T *_ptr;
    AutoDelObj<T, delMethod> _delObj;
    #pragma endregion
};

FS_NAMESPACE_END

#include"base/common/assist/assistobjs/Impl/SmartPtrImpl.h"

#endif // !__Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
