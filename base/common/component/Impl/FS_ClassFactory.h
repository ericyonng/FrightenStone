#ifndef __Base_Common_Component_Impl_FS_ClassFactory__H__
#define __Base_Common_Component_Impl_FS_ClassFactory__H__
/**
* @file    FS_ClassFactory.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_ClassFactory
{
public:
    template<typename T, typename... Args>
    static T *Create(Args&&... args);
};

FS_NAMESPACE_END

#include "base/common/component/Impl/FS_ClassFactoryImpl.h"

#endif
