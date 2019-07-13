#ifndef __Test_TestCrashHandle_H__
#define __Test_TestCrashHandle_H__

#pragma once

#include "stdafx.h"


class TestCrashEasyGloal : public fs::IEasyGlobal
{
public:
    TestCrashEasyGloal() { g_EasyGlobal = this; Init(); }
    virtual ~TestCrashEasyGloal()
    {}
#pragma region init/finish
public:
    virtual Int32 Init()
    {
        _log = fs::ILog::InitModule("main");
        return StatusDefs::Success;
    }
    virtual void Finish()
    {
        // 释放组件
        Fs_SafeFree(_log);
    }
#pragma endregion

    // 获取资源
public:
    virtual fs::ILog *GetLog()
    {
        return _log;
    }

private:
    fs::ILog *_log;
};

TestCrashEasyGloal g_Global;

class TestCrashHandle
{
public:
    static void Run()
    {
//         g_EasyGlobal = new TestCrashEasyGloal;
//         g_EasyGlobal->Init();
        fs::CrashHandleUtil::InitCrashHandleParams();

        EXCEPTION_RECORD SavedExceptionRec;
        CONTEXT SavedContextRec;
        __try
        {
            Int32 *intPtr = NULL;
            *intPtr = 89;
        }
        __except(fs::CrashHandleUtil::RecordExceptionInfo(*GET_EXCEPTION_INFO()))
        {
        }
    }
};

#endif
