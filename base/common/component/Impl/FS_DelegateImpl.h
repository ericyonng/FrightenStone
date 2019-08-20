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
 * @file  : FS_DelegateImpl.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifdef __Base_Common_Component_Impl_FS_Delegate_H__


#pragma once

#include <forward_list>

FS_NAMESPACE_BEGIN

template <typename T, typename R, typename ... Args>
inline FS_Delegate<T, R, Args...>::FS_Delegate(T *obj, R(T::*f)(Args...))
:_obj(obj)
,_f(f)
{
    
}

template <typename T, typename R, typename ... Args>
inline R FS_Delegate<T, R, Args...>::operator()(Args&&... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename T, typename R, typename ... Args>
inline void FS_Delegate<T, R, Args...>::Release()
{
    delete this;
}

template <typename T, typename R, typename ... Args>
inline FS_Delegate<T, R, Args...>* DelegateFactory::Create(T *obj, R ( T::*f)(Args ...))
{
    return new FS_Delegate<T, R, Args...>(obj, f);
}

template <typename R, typename... Args>
inline IDelegatePlus<R, Args...>::IDelegatePlus()
{
}

template <typename R, typename... Args>
inline IDelegatePlus<R, Args...>::~IDelegatePlus()
{
}

template <typename R, typename... Args>
inline void IDelegatePlus<R, Args...>::Release()
{
    delete this;
}

template <typename T, class R, typename... Args>
inline DelegateClassPlus<T, R, Args...>::DelegateClassPlus(T *t, R(T::*f)(Args...))
    :_obj(t)
    ,_f(f)
{
}

template <typename T, class R, typename... Args>
inline DelegateClassPlus<T, R, Args...>::DelegateClassPlus(T *t, R(T::*f)(Args...) const)
    :_obj(t)
    , _f(decltype(_f)(f))
{
}


template <typename T, typename R, typename... Args>
inline DelegateClassPlus<T, R, Args...>::~DelegateClassPlus()
{

}

template <typename T, typename R, typename... Args>
inline R DelegateClassPlus<T, R, Args...>::operator()(Args&&... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename T, typename R, typename... Args>
inline R DelegateClassPlus<T, R, Args...>::operator()(Args&&... args) const
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename R, typename... Args>
inline DelegateFunctionPlus<R, Args...>::DelegateFunctionPlus(R(*f)(Args...))
    :_f(f)
{
}

template <typename R, typename... Args>
inline DelegateFunctionPlus<R, Args...>::~DelegateFunctionPlus()
{
}

template <typename R, typename... Args>
inline R DelegateFunctionPlus<R, Args...>::operator()(Args&&... args)
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename R, typename... Args>
inline R DelegateFunctionPlus<R, Args...>::operator()(Args&&... args) const
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>::DelegateCustomFuncPlus(CustomFuncType &&customFunc)
    :_customFun(std::forward<CustomFuncType>(customFunc))
{

}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>::DelegateCustomFuncPlus(CustomFuncType const&customFunc)
    :_customFun(customFunc)
{

}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>::~DelegateCustomFuncPlus()
{
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline Rtn DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>::operator()(Args&&... args)
{
    return _customFun(std::forward<Args>(args)...);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline Rtn DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>::operator()(Args&&... args) const
{
    return _customFun(std::forward<Args>(args)...);
}

template <typename T, typename R, typename... Args>
inline IDelegatePlus<R, Args...> *DelegatePlusFactory::Create(T *obj, R(T::*f)(Args...))
{
    return new DelegateClassPlus<T, R, Args...>(obj, f);
}

template <typename T, typename R, typename... Args>
inline const IDelegatePlus<R, Args...> *DelegatePlusFactory::Create(T *obj, R(T::*f)(Args...) const)
{
    return new DelegateClassPlus<T, R, Args...>(obj, f);
}

template <typename R, typename... Args>
inline IDelegatePlus<R, Args...> *DelegatePlusFactory::Create(R(*f)(Args...))
{
    return new DelegateFunctionPlus<R, Args...>(f);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline IDelegatePlus<Rtn, Args...> *DelegatePlusFactory::Create(CustomFuncType &&func)
{
    return new DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>(std::forward<CustomFuncType>(func));
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline IDelegatePlus<Rtn, Args...> *DelegatePlusFactory::Create(CustomFuncType const&func)
{
    return new DelegateCustomFuncPlus<CustomFuncType, Rtn, Args...>(func);
}

FS_NAMESPACE_END

#endif

