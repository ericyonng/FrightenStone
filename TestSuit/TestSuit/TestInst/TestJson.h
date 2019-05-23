#ifndef __Test_TestJson_H__
#define __Test_TestJson_H__

#pragma once
#define __USE_FS_3RD_JSON___
#include "3rd/3rd.h"
#include "stdafx.h"

class TestJson
{
public:
    static void Run()
    {
        nlohmann::json j;
        j["pi"] = 3.141;
        j["name"] = "Niels";
        fs::FS_String p;
        std::cout << p << std::endl;
        std::cout << j << std::endl;
    }
};

#endif

