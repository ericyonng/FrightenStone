#ifndef __Test_TestLogModule_H__
#define __Test_TestLogModule_H__
#pragma once
#include "stdafx.h"

class TestLogModule
{
public:
    static void Run()
    {
        auto logMgr = fs::ILog::InitModule("main");
        fs::Time nowTime, nowTime2;
        nowTime.FlushTime();
        for(Int32 i = 0; i < 1000000; ++i)
        {
            logMgr->w<TestLogModule>(fs::LogDefs::_SYSLOG_frameLayer_, __FUNCTION__, __LINE__
                                     , "%s", "hello world");
        }
        nowTime2.FlushTime();
        std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
        logMgr->FinishModule();
        Fs_SafeFree(logMgr);
    }
};
#endif
