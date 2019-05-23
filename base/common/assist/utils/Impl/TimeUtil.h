#ifndef __Base_Common_Assist_Utils_Impl_TimeUtil_H__
#define __Base_Common_Assist_Utils_Impl_TimeUtil_H__
/**
* @file TimeUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/11
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT TimeUtil
{
public:
    static void SetTimeZone();    // 线程不安全只需设置一次
    static Int32 GetTimeZone();
    static bool IsLeapYear(Int32 year);
    /**
     * Get specific month max days.
     * @param[in] year  - the year.
     * @param[in] month - the month.
     * @return int - the specific month max days, if failed, return 0.
     */
    static Int32 GetMonthMaxDays(Int32 year, Int32 month);
};

#pragma region Inline
inline bool TimeUtil::IsLeapYear(Int32 year)
{
    return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);
}
#pragma endregion

FS_NAMESPACE_END

#endif
