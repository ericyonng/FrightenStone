#ifndef __Test_TestFile_H__
#define __Test_TestFile_H__

#pragma once
#include "stdafx.h"

class TestFile
{
public:
    static void Run()
    {
        fs::FS_File file;
        file.Open("./log/yizhihua.log", true);
        fs::FS_String str;
        std::cout << file.ReadOneLine(str) << std::endl;
        std::cout << str.GetRaw() << std::endl;
        std::cout << file.IsOpen() << std::endl;
        std::cout << file.GetPath()<< std::endl;
        std::cout << file.GetFileName() << std::endl;
        std::cout << file << std::endl;
        FILE *fileptr = file;
    }
};

#endif
