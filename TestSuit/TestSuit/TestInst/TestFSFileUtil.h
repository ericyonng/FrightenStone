#ifndef __Test_TestFSFileUtil_H__
#define __Test_TestFSFileUtil_H__

#pragma once
#include "stdafx.h"

class TestFSFileUtil
{
public:
    static void Run()
    {
        auto fp = fs::FS_FileUtil::OpenFile("./log/net/bb/log2.txt", false);
        std::cout << fs::FS_FileUtil::IsFileExist("./log/net/bb/log2.txt") << std::endl;
        std::cout << fs::FS_FileUtil::GetFileSize(*fp) << std::endl;
        std::cout << fs::FS_FileUtil::GetFileSizeEx("./log/net/bb/log2.txt") << std::endl;
    }
};

#endif
