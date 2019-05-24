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
 * filename: SmartPtr.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
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
