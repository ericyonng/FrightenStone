#ifndef __Test_TestString_H__
#define __Test_TestString_H__

#pragma once
#include "stdafx.h"


#include <limits>

inline fs::FS_String &&GetString()
{
    return fs::FS_String();
}

inline fs::FS_String GetString(bool)
{
    fs::FS_String str = "i love you";
    return str;
}

class TestString
{
public:
    static void Run()
    {
        fs::FS_String str1, str2, cache, md5str, hexstr, hex2;
        auto fp = fs::FS_FileUtil::OpenFile("./log/woai.txt");
        fs::FS_FileUtil::ReadFile(*fp, str1);
        str2 = str1;
        fs::StringUtil::MakeMd5(str1, md5str);
        fs::StringUtil::ToHexString(md5str, hexstr);
        md5str.Clear();
        fs::StringUtil::MakeMd5(str2, md5str);
        fs::StringUtil::ToHexString(md5str, hex2);
        std::cout << hex2.c_str() << std::endl;
        std::cout << hexstr.c_str() << std::endl;
        std::cout << (hex2 == hexstr) << std::endl;
    }
};

#endif
