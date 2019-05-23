#ifndef __Test_TestTime_H__
#define __Test_TestTime_H__

#pragma once
#include "stdafx.h"


class TestTime
{
public:
    static void Run()
    {
        fs::TimeUtil::SetTimeZone();
        fs::Time time1;
        time1.FlushTime();
        time1 = time1.GetZeroTime();
        std::cout << time1.Format().c_str() << std::endl;
        std::cout << time1.FormatAsGmt().c_str() << std::endl;
    }
};

#endif
