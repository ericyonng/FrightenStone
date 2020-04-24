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
 * filename: SmartPtrImpl.h
 *
 * author  : ericyonng<120453674@qq.com>
 * date    : 2019/5/24
 *
 * 
 */
#ifdef __Frame_Include_FrightenStone_Common_Assist_AssistObjs_Impl_SmartPtr_H__
/**
* @file SmartPtrImpl.h
* @auther songhuiya
* @date 2018/10/16
* @brief
*/


#pragma once

FS_NAMESPACE_BEGIN

#pragma region constructor
template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
SmartPtr<ObjType, delMethod>::SmartPtr()
    :_ptr(NULL)
{

}

template<typename ObjType,  AssistObjsDefs::DelMethods delMethod>
SmartPtr<ObjType, delMethod>::SmartPtr(ObjType *ptr)
    :_ptr(ptr)
{

}

template<typename ObjType,  AssistObjsDefs::DelMethods delMethod>
SmartPtr<ObjType, delMethod>::SmartPtr(SmartPtr<ObjType, delMethod> &&obj)
{
    _ptr = obj.pop();
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
SmartPtr<ObjType, delMethod>::~SmartPtr()
{
    Release();
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline ObjType *SmartPtr<ObjType, delMethod>::pop()
{
    ObjType *obj = _ptr;
    _ptr = NULL;
    return obj;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline void SmartPtr<ObjType, delMethod>::Release()
{
    _delObj.Release(_ptr);
    _ptr = NULL;
}
#pragma endregion

#pragma region operations
template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod> &SmartPtr<ObjType, delMethod>::operator =(ObjType *ptr)
{
    if(_ptr == ptr)
        return *this;

    Release();
    _ptr = ptr;

    return *this;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod> &SmartPtr<ObjType, delMethod>::operator =(SmartPtr<ObjType, delMethod> &&ptr) noexcept
{
    _ptr = ptr.pop();
    return *this;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod>::operator void *()
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod>::operator const void *() const
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod>::operator ObjType *()
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod>::operator const ObjType *() const
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline ObjType *SmartPtr<ObjType, delMethod>::operator->()
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline const ObjType *SmartPtr<ObjType, delMethod>::operator->() const
{
    return _ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline ObjType &SmartPtr<ObjType, delMethod>::operator [](int index)
{
    ObjType *ptr = _ptr;
    if(ptr)
        ptr += index;

    return *ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline const ObjType &SmartPtr<ObjType, delMethod>::operator [](int index) const
{
    ObjType *ptr = _ptr;
    if(ptr)
        ptr += index;

    return *ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod /*= AssistObjsDefs::Delete*/>
inline const ObjType& SmartPtr<ObjType, delMethod>::operator*() const
{
    return *_ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod /*= SmartPtrEnums::Delete*/>
inline ObjType& SmartPtr<ObjType, delMethod>::operator*()
{
    return *_ptr;
}

template<typename ObjType, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<ObjType, delMethod>::operator bool()
{
    return (_ptr != NULL);
}

#pragma endregion

FS_NAMESPACE_END

#endif
