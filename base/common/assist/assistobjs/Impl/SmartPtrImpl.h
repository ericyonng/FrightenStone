#ifdef __Base_Common_Assist_AssistObjs_Impl_SmartPtr_H__
/**
* @file SmartPtrImpl.h
* @auther songhuiya
* @date 2018/10/16
* @brief
*/


#pragma once

FS_NAMESPACE_BEGIN

#pragma region 构造/析构/弹出指针/释放（解除接管）
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

#pragma region 操作符重载 指针符号, 下标索引等
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
