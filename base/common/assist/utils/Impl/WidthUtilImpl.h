#ifdef __Base_Common_Assist_Utils_Impl_WidthUtil_H__
/**
* @file WidthUtilImpl.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/19
* @brief
*/

#pragma once

FS_NAMESPACE_BEGIN

template<typename T>
inline int WidthUtil::GetBinaryWidth()
{
    int cnt = 1;
    auto max = (std::numeric_limits<T>::max)();
    while((max /= 2) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetBinaryWidth(T val)
{
    int cnt = 1;
    while((val /= 2) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetDecimalWidth()
{
    int cnt = 1;
    T max = (std::numeric_limits<T>::max)();
    while((max /= 10) != 0)
        ++cnt;

    return cnt;
}

template<typename T>
inline int WidthUtil::GetDecimalWidth(T val)
{
    int cnt = 1;
    while((val /= 10) != 0)
        ++cnt;

    return cnt;
}

FS_NAMESPACE_END

#endif
