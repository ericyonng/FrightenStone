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
 * @file  : TestTrigger.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestTrigger_H__
#define __Test_TestTrigger_H__

#pragma once
#include "stdafx.h"

class TestTriggerDefs
{
public:
    enum
    {
        Begin = 0,
        Test,
        End,
    };

    enum
    {
        TestTrigger = 0,
    };
    static bool CanInterrupt(Int32 occasion);
};

bool TestTriggerDefs::CanInterrupt(Int32 occasion)
{
    if(occasion > Begin && occasion < End)
        return true;

    return false;
}

class TestTrigger
{
public:
    static void Run()
    {
        fs::Trigger trigger(fs::DelegatePlusFactory::Create(&TestTriggerDefs::CanInterrupt));
        auto __testTriggerFunc = [](fs::TriggerExecuteBody *thisExecBody) ->Int32
        {
            std::cout << "hello trigger" << std::endl;
            return StatusDefs::Success;
        };
        auto testTriggerFunc = fs::DelegatePlusFactory::Create<decltype(__testTriggerFunc), Int32, fs::TriggerExecuteBody *>(__testTriggerFunc);
        trigger.Reg(TestTriggerDefs::Test, TestTriggerDefs::TestTrigger, *testTriggerFunc);
        trigger.Exec(TestTriggerDefs::Test);
        Fs_SafeFree(testTriggerFunc);
    }
};
#endif
