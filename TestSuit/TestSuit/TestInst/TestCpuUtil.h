#ifndef __Test_TestCpuUtil_H__
#define __Test_TestCpuUtil_H__

#pragma once
#include "stdafx.h"

class TestCpuUtil
{
public:
    static void Run()
    {
        fs::FS_CpuUtil::Initialize();
        Int64 usage = static_cast<Int64>(fs::FS_CpuUtil::GetUsage() * 100);
        std::cout << usage << std::endl;
        std::cout << fs::FS_CpuUtil::GetCpuCoreCnt() << std::endl;
    }
};
#endif
