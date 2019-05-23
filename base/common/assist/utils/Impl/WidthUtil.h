#ifndef __Base_Common_Assist_Utils_Impl_WidthUtil_H__
#define __Base_Common_Assist_Utils_Impl_WidthUtil_H__
/**
* @file WidthUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/19
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT WidthUtil
{
public:
    template<typename T>
    static int GetBinaryWidth();
    template<typename T>
    static int GetBinaryWidth(T val);
    template<typename T>
    static int GetDecimalWidth();
    template<typename T>
    static int GetDecimalWidth(T val);
};

FS_NAMESPACE_END

#include "base/common/assist/utils/Impl/WidthUtilImpl.h"

#endif
