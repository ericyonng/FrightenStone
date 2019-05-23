#ifdef __Base_Common_Component_Impl_FS_ArgsExpander__H__
/**
* @file    FS_ArgsExpanderImpl.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/12
* @brief
*/

#pragma once

FS_NAMESPACE_BEGIN

template<typename T>
template<typename... Args>
inline void FS_ArgsExpander<T>::Expand(Args&&... args)
{
    
}

template<typename T>
template<typename Arg0, typename... Args>
inline void FS_RecursiveArgsExpander<T>::Expand(Arg0&& arg0, Args&&... rest)
{
    Expand(std::forward<Args>(rest)...);
}

template<typename T>
template<typename Arg0>
inline void FS_RecursiveArgsExpander<T>::Expand(Arg0&& arg0)
{
}

FS_NAMESPACE_END

#endif
