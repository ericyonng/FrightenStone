#ifndef __Base_Common_Component_Impl_FS_ArgsExpander__H__
#define __Base_Common_Component_Impl_FS_ArgsExpander__H__
/**
* @file    FS_ArgsExpander.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

template<typename T>
class BASE_EXPORT FS_ArgsExpander
{
public:
    template<typename... Args>
    static void Expand(Args&&... args);
};

template<typename T>
class BASE_EXPORT FS_RecursiveArgsExpander
{
public:
    template<typename Arg0, typename... Args>
    static void Expand(Arg0&& arg0, Args&&... rest);

    template<typename Arg0>
    static void Expand(Arg0&& arg0);
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_ArgsExpanderImpl.h"
#endif
