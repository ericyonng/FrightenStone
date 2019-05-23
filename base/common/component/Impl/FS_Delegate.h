#ifndef __Base_Common_Component_Impl_FS_Delegate__H__
#define __Base_Common_Component_Impl_FS_Delegate__H__
/**
* @file    FS_Delegate.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

template <class T, class R, typename... Args>
class FS_Delegate
{
public:
    FS_Delegate(T *t, R(T::*f)(Args...));
    R operator()(Args&&... args);

private:
    T *_obj;
    R(T::*_f)(Args...);
};

class BASE_EXPORT DelegateFactory
{
public:
    template <class T, class R, typename... Args>
    static FS_Delegate<T, R, Args...> *Create(T *obj, R(T::*f)(Args...));
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_DelegateImpl.h"

#endif
