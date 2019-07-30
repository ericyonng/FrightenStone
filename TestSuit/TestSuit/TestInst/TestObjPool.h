#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once

#include "stdafx.h"

#undef __FS_THREAD_SAFE__
#define __FS_THREAD_SAFE__ 0

#undef TEST_OBJ_NUM
#define TEST_OBJ_NUM    1000

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
    char _char;
};

OBJ_POOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, 5)

class TestObjPoolObj2
{
public:
    TestObjPoolObj2() {}
    ~TestObjPoolObj2() {}

private:
    char _char;
};

class TestObjPool
{
public:
    static void Run()
    {
        // g_Log->InitModule("TestObjPool");
        fs::Time timeNow1, timeNow2;
        timeNow1.FlushTime();
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
             new TestObjPoolObj2;
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

        timeNow1.FlushTime();
        TestObjPoolObj *ptr[TEST_OBJ_NUM] = {};
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {            
            ptr[i] = new TestObjPoolObj;
        }
        timeNow2.FlushTime();
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
            delete  ptr[i];
        }

        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
        std::cout << "memleak:" << TestObjPoolObj::GetMemleakNum() << std::endl;
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
