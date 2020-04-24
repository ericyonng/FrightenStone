#ifndef __Test_TestRandom_H__
#define __Test_TestRandom_H__
#pragma once
#include "stdafx.h"

class TestRandom
{
public:
    static void Run()
    {
        fs::FS_Int64Random random(-100000000, -200000000);
        fs::FS_Int64Random randOmdd;
        std::cout << random(g_RandomSeed) << std::endl;
        std::cout << random(g_RandomSeed) << std::endl;
        std::cout << random(g_RandomSeed) << std::endl;
        std::cout << random(g_RandomSeed) << std::endl;
    }
};

#endif
