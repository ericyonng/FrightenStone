#ifndef __Test_TestRandom_H__
#define __Test_TestRandom_H__
#pragma once
#include "stdafx.h"

class TestRandom
{
public:
    static void Run()
    {
        fs::FS_Int64Random random(28000000000, 58000000000, time(0));
        std::cout << random() << std::endl;
        std::cout << random() << std::endl;
        std::cout << random() << std::endl;
        std::cout << random() << std::endl;
    }
};

#endif
