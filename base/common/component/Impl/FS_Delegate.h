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
 * @file  : FS_Delegate.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_Component_Impl_FS_Delegate_H__
#define __Base_Common_Component_Impl_FS_Delegate_H__


#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"

#ifndef DELEGATE_ARGS
#define DELEGATE_ARGS(arg)   std::forward<decltype(arg)>(arg)
#endif

FS_NAMESPACE_BEGIN

// R回调返回值类型，Args回调函数参数包 委托基类 用于解耦具体类型，创建类型无关的委托
template <typename Rtn, typename... Args>
class IDelegate
{
public:
    IDelegate();
    virtual ~IDelegate();
    // 左值会绑定成左值引用，右值会绑定成右值引用
    // 请注意引用折叠适当使用std::forward可以完美的将参数传入，原来什么类型传入后绑定的就是什么类型
    virtual Rtn Invoke(Args&&... args) = 0;
    virtual Rtn Invoke(Args&&... args) const = 0;
    virtual void Release();
};

template <typename ObjType, typename Rtn, typename... Args>
class DelegateClass : public IDelegate<Rtn, Args...>
{
public:
    DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...));
    DelegateClass(ObjType *t, Rtn(ObjType::*f)(Args...) const);
    virtual ~DelegateClass();

    virtual Rtn Invoke(Args&&... args);
    virtual Rtn Invoke(Args&&... args) const;

private:
    ObjType *_obj;
    Rtn(ObjType::*_f)(Args...);
    // mutable R(T::*_fconst)(Args...) const;
};

template <typename Rtn, typename... Args>
class DelegateFunction : public IDelegate<Rtn, Args...>
{
public:
    DelegateFunction(Rtn(*f)(Args...));
    virtual ~DelegateFunction();

    virtual Rtn Invoke(Args&&... args);
    virtual Rtn Invoke(Args&&... args) const;

private:
    Rtn(*_f)(Args...);
};

// 支持lambda表达式,std::function等闭包
template <typename CustomFuncType, typename Rtn, typename... Args>
class DelegateCustomFunc : public IDelegate<Rtn, Args...>
{
public:
    DelegateCustomFunc(CustomFuncType &&customFunc);
    DelegateCustomFunc(CustomFuncType const&customFunc);
    virtual ~DelegateCustomFunc();

    virtual Rtn Invoke(Args&&...);
    virtual Rtn Invoke(Args&&... args) const;

private:
    CustomFuncType _customFun;
};

class BASE_EXPORT DelegatePlusFactory
{
public:
    template <typename ObjType, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(ObjType *obj, Rtn(ObjType::*f)(Args...));
    template <typename ObjType, typename Rtn, typename... Args>
    static const IDelegate<Rtn, Args...> *Create(ObjType *obj, Rtn(ObjType::*f)(Args...) const);

    template <typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(Rtn(*f)(Args...));

    // 绑定lambda,std::function,如：DelegatePlusFactory::Create<decltype(func), void, int>(func);
    template <typename CustomFuncType /* = decltype(func) */, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(CustomFuncType &&func);

    // 绑定lambda,std::function,如：DelegatePlusFactory::Create<decltype(func), void, int>(func);
    template <typename CustomFuncType /* = decltype(func) */, typename Rtn, typename... Args>
    static IDelegate<Rtn, Args...> *Create(CustomFuncType const&func);
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_DelegateImpl.h"

#endif
