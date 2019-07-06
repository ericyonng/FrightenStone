#ifndef __Test_TestLogModule_H__
#define __Test_TestLogModule_H__
#pragma once
#include "stdafx.h"

// 定义global

class EasyGloal : public fs::IEasyGlobal
{
public:
    EasyGloal() {}
    virtual ~EasyGloal()
    {}
#pragma region init/finish
public:
    virtual Int32 Init() 
    {
        _log = g_Log;
        _log->InitModule("main");
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

typedef fs::Singleton<EasyGloal> EasyGloalInstance;
#define g_EasyGlobal2 EasyGloalInstance::GetInstance()

class TestLogModule
{
public:
    static void Run()
    {
        fs::TimeUtil::SetTimeZone();
        g_EasyGlobal2->Init();
        fs::Time nowTime, nowTime2;
        nowTime.FlushTime();
//         logMgr->w<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
//         logMgr->e<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
//         logMgr->crash<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
//         logMgr->memleak<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
//         logMgr->net<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
            g_Log->w<TestLogModule>(_LOGFMT_("hello world%s"), "wocao");
        nowTime2.FlushTime();
        std::cout << (nowTime2 - nowTime).GetTotalMilliSeconds() << std::endl;
        g_EasyGlobal2->Finish();

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
