#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once

#include "stdafx.h"

#undef __FS_THREAD_SAFE__
#define __FS_THREAD_SAFE__ 0

#undef TEST_OBJ_NUM
#define TEST_OBJ_NUM    10000000

fs::MemleakMonitor *_g_memMonitor = g_MemleakMonitor;

class TestObjPoolObj
{
   OBJ_POOL_CREATE(TestObjPoolObj, _objPoolHelper)
public:
    TestObjPoolObj() {}
    ~TestObjPoolObj() 
    {
        //std::cout << "Îö¹¹" << std::endl;
    }

private:
    Int64 _char;
    Int64 _char2;
    Int64 _char3;
    Int64 _char4;
    Int64 _char5;
};

OBJ_POOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, TEST_OBJ_NUM)

class TestObjPoolObj2
{
public:
    TestObjPoolObj2() {}
    ~TestObjPoolObj2() {}

private:
    Int64 _char;
    Int64 _char2;
    Int64 _char3;
    Int64 _char4;
    Int64 _char5;
};

class TestObjPool
{
public:
    static void Run()
    {
        g_Log->InitModule("TestObjPool");
        fs::Time timeNow1, timeNow2;
        timeNow1.FlushTime();
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
             new TestObjPoolObj2;
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

        timeNow1.FlushTime();
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {            
            new TestObjPoolObj;
        }
        timeNow2.FlushTime();

        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
        std::cout << "memleak:" << TestObjPoolObj::GetMemleakNum() << std::endl;
        g_MemleakMonitor->PrintMemleakInfo();
//         g_Log->w<TestObjPool>(_LOGFMT_("HELLO"));
//         g_Log->FinishModule();
//         timeNow1.FlushTime();
//         for(Int32 i = 0; i < 100000; ++i)
//         {
//             delete objs[i];
//         }
//         timeNow2.FlushTime();
//         std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
// 
//         TestObjPoolObj *newObj = new TestObjPoolObj;
//         newObj = new TestObjPoolObj;
//         printf("%p %llu", newObj, sizeof(TestObjPoolObj));
    }
};
#endif
