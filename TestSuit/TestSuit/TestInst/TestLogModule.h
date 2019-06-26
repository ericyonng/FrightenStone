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
        for(Int32 i = 0; i < 1000000; ++i)
        {
            logMgr->w<TestLogModule>(fs::LogDefs::_SYSLOG_frameLayer_, __FUNCTION__, __LINE__
                                     , "%s", "hello world");
        }
        nowTime2.FlushTime();
        std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
        logMgr->FinishModule();
        Fs_SafeFree(logMgr);

//         std::cout << "-----------------------------------------" << std::endl;
//         fs::Time nowTime, nowTime2, nowTime3;
//         fs::FS_String cache;
//         // 1. log 优化建议：避免format tostring 类似的处理,性能消耗很大
//         // 
//        // fs::FS_String str = "nowtime[1561516405981839]2019-06-26 10:33:25.973291<Warning>[class TestLogModule][TestLogModule::Run]line:16 hello world\n";
//         auto fp = fs::FS_FileUtil::OpenFile(".\\test.log", true);
//         nowTime3.FlushTime();
//         nowTime.FlushTime();
//         for(Int32 i = 0; i < 1000000; ++i)
//         {
//             cache.Clear();
//             cache.Format("%s<%s>[%s][%s]line:%d %s"
//                          ,"2019-06-26 10:33:25.973291"
//                          , fs::LogLevel::GetDescription(1)
//                          , "class TestLogModule"
//                          , __FUNCTION__
//                          , __LINE__
//                          , "hello world\n");
//             fs::FS_FileUtil::WriteFile(*fp, cache);
//             fs::FS_FileUtil::FlushFile(*fp);
//         }
//         nowTime2.FlushTime();
//         std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
    }
};
#endif
