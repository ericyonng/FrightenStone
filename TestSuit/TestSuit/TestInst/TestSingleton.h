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
 * @file  : TestSingleton.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */

#ifndef __Test_TestSingleton_H__
#define __Test_TestSingleton_H__
#pragma once
#include "stdafx.h"

class TestSingletomExample
{
public:
    TestSingletomExample()
    {
        g_Log->i<TestSingletomExample>(_LOGFMT_("TestSingletomExample constructor"));
    }
    ~TestSingletomExample()
    {
        g_Log->i<TestSingletomExample>(_LOGFMT_("TestSingletomExample destructor"));
        g_Log->FlushAllFile();
    }
};

class TestSingleton
{
public:
    static void Run()
    {
        g_MemleakMonitor->Start();
        TestSingletomExample *example = new TestSingletomExample;
        g_MemoryPool->InitPool();
        auto log3 = g_Log;
        auto log = fs::ILog::GetInstance();
        auto log2 = fs::ILog::GetInstance();
        g_Log->i<TestSingleton>(_LOGFMT_("log3:%p,log[%p],log2[%p]"), log3, log, log2);
        delete example;
    }
};
#endif
