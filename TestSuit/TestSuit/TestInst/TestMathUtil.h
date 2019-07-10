#ifndef __Test_TestMathUtil_H__
#define __Test_TestMathUtil_H__

#pragma once

#include "stdafx.h"

class TestMathUtil
{
public:
    static void Run()
    {
        Int64 a = 4, b = 10;
        std::cout << fs::MathUtil::GetGcd(a, b) << std::endl;
        std::cout << fs::MathUtil::GetLcm(a, b) << std::endl;
    }
};

#endif
