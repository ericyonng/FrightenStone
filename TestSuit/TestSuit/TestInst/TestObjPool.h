#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once

#include "stdafx.h"

#undef __FS_THREAD_SAFE__
#define __FS_THREAD_SAFE__ 0

class TestObjPoolObj
{
   OBJ_POOL_CREATE(TestObjPoolObj, _objPoolHelper)
public:
    TestObjPoolObj() {}
    ~TestObjPoolObj() {}

private:
    char _cnt;
};

OBJ_POOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, 5000)

class TestObjPoolObj2
{
public:
    TestObjPoolObj2() {}
    ~TestObjPoolObj2() {}

private:
    char _cnt;
};

class TestObjPool
{
public:
    static void Run()
    {
        g_Log->InitModule("TestObjPool");
        fs::Time timeNow1, timeNow2;
        timeNow1.FlushTime();
        for(Int32 i = 0; i < 50000000; ++i)
        {
             delete new TestObjPoolObj2;
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

        timeNow1.FlushTime();
        for(Int32 i = 0; i < 50000000; ++i)
        {
            delete new TestObjPoolObj;
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

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
