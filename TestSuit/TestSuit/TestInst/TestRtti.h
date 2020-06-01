/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : TestRtti.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestRtti_H__
#define __Test_TestRtti_H__
#pragma once

#include "stdafx.h"

class TestRttiObj
{
public:
};

struct TestRttiStruct
{

};

class TestRtti
{
public:
    static void Run()
    {
        fs::FS_TlsUtil::CreateUtilTlsHandle();
        std::cout << fs::RTTIUtil::GetByType<Int32>() << std::endl;
        std::cout << fs::RTTIUtil::GetByType<TestRttiObj>() << std::endl;
        std::cout << fs::RTTIUtil::GetByType<TestRttiObj *>() << std::endl;
        std::cout << fs::RTTIUtil::GetByType<TestRttiStruct>() << std::endl;
        std::cout << fs::RTTIUtil::GetByType<TestRttiStruct *>() << std::endl;
    }
};

#endif
