#ifndef __Test_TestUtf8_H__
#define __Test_TestUtf8_H__

#pragma once
#define __USE_FS_TINY_UTF8__
#include "3rd/3rd.h"
#include "3rd/tiny-utf8/lib/tinyutf8.cpp"
#include "stdafx.h"

class TestUtf8
{
public:
    static void Run()
    {
        utf8_string str = u8"!🌍 olleH";
        std::for_each(str.rbegin(), str.rend(), [](char32_t codepoint) {
            std::cout << codepoint;
        });
    }
};
#endif
