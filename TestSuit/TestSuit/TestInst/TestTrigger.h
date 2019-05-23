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
        fs::Trigger trigger(TestTriggerDefs::CanInterrupt);
        auto __testTriggerFunc = [](fs::TriggerExecuteBody *thisExecBody) ->Int32
        {
            std::cout << "hello trigger" << std::endl;
            return StatusDefs::Success;
        };
        trigger.Reg(TestTriggerDefs::Test, TestTriggerDefs::TestTrigger, __testTriggerFunc);
        trigger.Exec(TestTriggerDefs::Test);
    }
};
#endif
