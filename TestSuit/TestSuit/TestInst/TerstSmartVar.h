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
        var["Insert"] = 11;
        var["Insert2"] = fs::FS_String("wo");
        bool isInsert = var["Insert"].AsBool();
        std::cout << var.ToString() << std::endl;
        std::cout << isInsert << std::endl;

    }
};

#endif
