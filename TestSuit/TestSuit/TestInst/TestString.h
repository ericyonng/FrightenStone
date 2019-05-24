/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : TestString.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
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
