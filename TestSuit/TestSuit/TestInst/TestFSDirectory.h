#ifndef __Test_TestFSDirectory_H__
#define __Test_TestFSDirectory_H__

#pragma once
#include "stdafx.h"


class TestFSDirectory
{
public:
    static void Run()
    {
        auto filename = fs::FS_DirectoryUtil::GetFileNameInPath(".\\log/net/bb/file.dd");
        std::cout << filename.GetRaw() << std::endl;
    }
};

#endif
