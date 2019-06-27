#ifndef __Test_TestLogModule_H__
#define __Test_TestLogModule_H__
#pragma once
#include "stdafx.h"

class TestLogModule
{
public:
    static void Run()
    {
        fs::TimeUtil::SetTimeZone();
        auto logMgr = fs::ILog::InitModule("main");
        fs::Time nowTime, nowTime2;
        nowTime.FlushTime();
        logMgr->w<TestLogModule>(__FUNCTION__, __LINE__
                                 , "hello world%s", "wocao");
        logMgr->crash<TestLogModule>(__FUNCTION__, __LINE__, "hello world%s", "wocao");
        logMgr->memleak<TestLogModule>(__FUNCTION__, __LINE__, "hello world%s", "wocao");
        logMgr->net<TestLogModule>(__FUNCTION__, __LINE__, "hello world%s", "wocao");
        nowTime2.FlushTime();
        std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
        logMgr->FinishModule();
        Fs_SafeFree(logMgr);

//         std::cout << "-----------------------------------------" << std::endl;
//         fs::Time nowTime, nowTime2, nowTime3;
//         fs::FS_String cache;
//         // 1. log 优化建议：避免format tostring 类似的处理,性能消耗很大
//         // 
//         fs::FS_String str = "2019-06-27 22:13:10.442468<Warn>[class TestLogModule][TestLogModule::Run][line:17]: hello world\n";
//         auto fp = fs::FS_FileUtil::OpenFile(".\\test.log", true);
//         nowTime3.FlushTime();
//         nowTime.FlushTime();
//         for(Int32 i = 0; i < 1000000; ++i)
//         {
//             fs::FS_FileUtil::WriteFile(*fp, str);
//         }
//         fs::FS_FileUtil::FlushFile(*fp);
//         nowTime2.FlushTime();
//         std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
    }
};
#endif
