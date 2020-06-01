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
 * filename: SmartPtr.h
 *
 * author  : Eric Yonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_SmartPtr_H__
#define __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_SmartPtr_H__
/**
* @file SmartPtr.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<FrightenStone/exportbase.h>
#include <FrightenStone/common/assist/assistobjs/Defs/AssistObjsDefs.h>

FS_NAMESPACE_BEGIN

// 智能对象
template<typename ObjType, AssistObjsDefs::DelMethods delMethod = AssistObjsDefs::Delete>
class SmartPtr
{
    #pragma region constructor
    SmartPtr(const SmartPtr<ObjType, delMethod> &obj) = delete;
    SmartPtr<ObjType, delMethod> &operator =(const SmartPtr<ObjType, delMethod> &ptr) = delete;
public:
    SmartPtr();
    SmartPtr(ObjType *ptr);
    SmartPtr(SmartPtr<ObjType, delMethod> &&obj);
    virtual ~SmartPtr();
    ObjType *pop();
    void Release();
    #pragma endregion

    #pragma region operations
public:
    SmartPtr<ObjType, delMethod> &operator =(ObjType *ptr);
    SmartPtr<ObjType, delMethod> &operator =(SmartPtr<ObjType, delMethod> &&ptr) noexcept;
    operator void *();
    operator const void *() const;

    operator ObjType *();
    operator const ObjType *() const;
    // operator SmartPtr<T, delMethod> &();
    ObjType *operator->();
    const ObjType *operator->() const;

    ObjType& operator [](int index);    // no out-range detect
    const ObjType& operator [](int index) const;    // no out-range detect
    ObjType& operator *();
    const ObjType& operator *() const;

    explicit operator bool();
    #pragma endregion

    #pragma region member
protected:
    ObjType *_ptr;
    AutoDelObj<ObjType, delMethod> _delObj;
    #pragma endregion
};

FS_NAMESPACE_END

#include"FrightenStone/common/assist/assistobjs/Impl/SmartPtrImpl.h"

#endif // !__Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_SmartPtr_H__
