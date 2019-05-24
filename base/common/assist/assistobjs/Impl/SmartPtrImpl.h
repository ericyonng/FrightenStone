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
#ifdef __Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
/**
* @file SmartPtrImpl.h
* @auther songhuiya
* @date 2018/10/16
* @brief
*/


#pragma once

FS_NAMESPACE_BEGIN

#pragma region constructor
template<typename T, AssistObjsDefs::DelMethods delMethod>
SmartPtr<T, delMethod>::SmartPtr()
    :_ptr(NULL)
{

}

template<typename T,  AssistObjsDefs::DelMethods delMethod>
SmartPtr<T, delMethod>::SmartPtr(T *ptr)
    :_ptr(ptr)
{

}

template<typename T,  AssistObjsDefs::DelMethods delMethod>
SmartPtr<T, delMethod>::SmartPtr(SmartPtr<T, delMethod> &&obj)
{
    _ptr = obj.pop();
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
SmartPtr<T, delMethod>::~SmartPtr()
{
    Release();
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline T *SmartPtr<T, delMethod>::pop()
{
    T *obj = _ptr;
    _ptr = NULL;
    return obj;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline void SmartPtr<T, delMethod>::Release()
{
    _delObj.Release(_ptr);
    _ptr = NULL;
}
#pragma endregion

#pragma region operations
template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod> &SmartPtr<T, delMethod>::operator =(T *ptr)
{
    if(_ptr == ptr)
        return *this;

    Release();
    _ptr = ptr;

    return *this;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod> &SmartPtr<T, delMethod>::operator =(SmartPtr<T, delMethod> &&ptr) noexcept
{
    _ptr = ptr.pop();
    return *this;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod>::operator void *()
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod>::operator const void *() const
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod>::operator T *()
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod>::operator const T *() const
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline T *SmartPtr<T, delMethod>::operator->()
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline const T *SmartPtr<T, delMethod>::operator->() const
{
    return _ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline T &SmartPtr<T, delMethod>::operator [](int index)
{
    T *ptr = _ptr;
    if(ptr)
        ptr += index;

    return *ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline const T &SmartPtr<T, delMethod>::operator [](int index) const
{
    T *ptr = _ptr;
    if(ptr)
        ptr += index;

    return *ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod /*= AssistObjsDefs::Delete*/>
inline const T& SmartPtr<T, delMethod>::operator*() const
{
    return *_ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod /*= SmartPtrEnums::Delete*/>
inline T& SmartPtr<T, delMethod>::operator*()
{
    return *_ptr;
}

template<typename T, AssistObjsDefs::DelMethods delMethod>
inline SmartPtr<T, delMethod>::operator bool()
{
    return (_ptr != NULL);
}

#pragma endregion

FS_NAMESPACE_END

#endif
