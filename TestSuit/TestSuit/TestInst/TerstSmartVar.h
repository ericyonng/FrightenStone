#ifndef __Test_TestSmartVar_H__
#define __Test_TestSmartVar_H__

#pragma once
#include "stdafx.h"

class TestSmartVar
{
public:
    static void Run()
    {
        fs::SmartVarRtti::InitRttiTypeNames();
        fs::SmartVar var;
        std::cout << sizeof(var) << std::endl;
        std::cout << sizeof(var.GetRaw()) << std::endl;
        std::cout << sizeof(var.GetRaw()._briefData) << std::endl;
        std::cout << sizeof(var.GetRaw()._obj) << std::endl;
        std::cout << sizeof(var.GetRaw()._type) << std::endl;
        var = 11;
        std::cout << var.ToString() << std::endl;
        var["Insert"] = fs::FS_String("wo");
        std::cout << var.ToString() << std::endl;
        std::cout << var["Insert"].AsStr() << std::endl;

    }
};

#endif
