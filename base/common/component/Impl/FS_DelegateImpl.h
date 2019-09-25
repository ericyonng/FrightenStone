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

template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...>::IDelegate()
{
}

template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...>::~IDelegate()
{
}

template <typename Rtn, typename... Args>
inline void IDelegate<Rtn, Args...>::Release()
{
    delete this;
}

template <typename ObjType, class Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...))
    :_obj(t)
    ,_f(f)
{
}

template <typename ObjType, class Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...) const)
    :_obj(t)
    , _f(decltype(_f)(f))
{
}


template <typename ObjType, typename Rtn, typename... Args>
inline DelegateClass<ObjType, Rtn, Args...>::~DelegateClass()
{

}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClass<ObjType, Rtn, Args...>::Invoke(Args&&... args)
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename ObjType, typename Rtn, typename... Args>
inline Rtn DelegateClass<ObjType, Rtn, Args...>::Invoke(Args&&... args) const
{
    return (_obj->*_f)(std::forward<Args>(args)...);
}

template <typename Rtn, typename... Args>
inline DelegateFunction<Rtn, Args...>::DelegateFunction(Rtn(*f)(Args...))
    :_f(f)
{
}

template <typename Rtn, typename... Args>
inline DelegateFunction<Rtn, Args...>::~DelegateFunction()
{
}

template <typename Rtn, typename... Args>
inline Rtn DelegateFunction<Rtn, Args...>::Invoke(Args&&... args)
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename Rtn, typename... Args>
inline Rtn DelegateFunction<Rtn, Args...>::Invoke(Args&&... args) const
{
    return (*_f)(std::forward<Args>(args)...);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFunc<CustomFuncType, Rtn, Args...>::DelegateCustomFunc(CustomFuncType &&customFunc)
    :_customFun(std::forward<CustomFuncType>(customFunc))
{

}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFunc<CustomFuncType, Rtn, Args...>::DelegateCustomFunc(CustomFuncType const&customFunc)
    :_customFun(customFunc)
{

}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline DelegateCustomFunc<CustomFuncType, Rtn, Args...>::~DelegateCustomFunc()
{
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline Rtn DelegateCustomFunc<CustomFuncType, Rtn, Args...>::Invoke(Args&&... args)
{
    return _customFun(std::forward<Args>(args)...);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline Rtn DelegateCustomFunc<CustomFuncType, Rtn, Args...>::Invoke(Args&&... args) const
{
    return _customFun(std::forward<Args>(args)...);
}

template <typename ObjType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegatePlusFactory::Create(ObjType *obj, Rtn(ObjType::*f)(Args...))
{
    return new DelegateClass<ObjType, Rtn, Args...>(obj, f);
}

template <typename ObjType, typename Rtn, typename... Args>
inline const IDelegate<Rtn, Args...> *DelegatePlusFactory::Create(ObjType *obj, Rtn(ObjType::*f)(Args...) const)
{
    return new DelegateClass<ObjType, Rtn, Args...>(obj, f);
}

template <typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegatePlusFactory::Create(Rtn(*f)(Args...))
{
    return new DelegateFunction<Rtn, Args...>(f);
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegatePlusFactory::Create(CustomFuncType &&func)
{
    return new DelegateCustomFunc<CustomFuncType, Rtn, Args...>(std::forward<CustomFuncType>(func));
}

template <typename CustomFuncType, typename Rtn, typename... Args>
inline IDelegate<Rtn, Args...> *DelegatePlusFactory::Create(CustomFuncType const&func)
{
    return new DelegateCustomFunc<CustomFuncType, Rtn, Args...>(func);
}

FS_NAMESPACE_END

#endif

