#ifndef __Base_Common_Assist_Utils_Impl_ThreadUtil_H__
#define __Base_Common_Assist_Utils_Impl_ThreadUtil_H__
/**
* @file ThreadUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<base\exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ThreadUtil
{
public:
    // 暂时让出线程调度
    static void YieldScheduler();
};

FS_NAMESPACE_END

#endif // !__Base_Common_Assist_Utils_Impl_ThreadUtil_H__
