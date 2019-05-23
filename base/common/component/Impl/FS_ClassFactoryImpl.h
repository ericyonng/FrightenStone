#ifdef __Base_Common_Component_Impl_FS_ClassFactory__H__
/**
* @file    FS_ClassFactoryImpl.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once

FS_NAMESPACE_BEGIN

template<typename T, typename... Args>
T *FS_ClassFactory::Create(Args&&... args)
{
    return new T(std::forward<Args>(args)...);
}

FS_NAMESPACE_END

#endif
