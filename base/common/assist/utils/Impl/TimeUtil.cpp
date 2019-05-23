/**
* @file TimeUtil.cpp
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/11
* @brief
*/

#include "stdafx.h"
#include "base/common/assist/utils/Impl/TimeUtil.h"
#include <atomic>
#include <time.h>

FS_NAMESPACE_BEGIN

std::atomic<Int32> __g_timezone{0};

void TimeUtil::SetTimeZone()
{
#ifndef _WIN32
    tzset();
    time_t now = time(NULL);
    __g_timezone = static_cast<Int32>(localtime(&now)->tm_gmtoff);
#else // WIN32
    _tzset();
    __g_timezone = _timezone;
#endif // Non-WIN32
}

Int32 TimeUtil::GetTimeZone()
{
    return __g_timezone;
}

Int32 TimeUtil::GetMonthMaxDays(Int32 year, Int32 month)
{
    if(month >= 1 && month <= 7)
    {
        if(month % 2 == 1)
            return 31;
        else if(month == 2)
            return IsLeapYear(year) ? 29 : 28;
        else
            return 30;
    }
    else if(month >= 8 && month <= 12)
    {
        return month % 2 == 0 ? 31 : 30;
    }
    else
    {
        printf("error param");
        return -1;
    }
}

FS_NAMESPACE_END

