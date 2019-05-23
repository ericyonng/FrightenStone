#ifdef __Base_Common_Component_Impl_FS_Delegate__H__
/**
* @file    FS_DelegateImpl.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once

#include <forward_list>

FS_NAMESPACE_BEGIN

template <class T, class R, typename ... Args>
FS_Delegate<T, R, Args...>::FS_Delegate(T *obj, R(T::*f)(Args...))
:_obj(obj)
,_f(f)
{
    
}

template <class T, class R, typename ... Args>
inline R FS_Delegate<T, R, Args...>::operator()(Args&&... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <class T, class R, typename ... Args>
inline FS_Delegate<T, R, Args...>* DelegateFactory::Create(T *obj, R ( T::*f)(Args ...))
{
    return new FS_Delegate<T, R, Args...>(obj, f);
}

FS_NAMESPACE_END

#endif

