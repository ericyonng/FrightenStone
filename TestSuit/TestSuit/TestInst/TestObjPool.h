#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once
#undef __FS_THREAD_SAFE__
#define __FS_THREAD_SAFE__ 1
#include "stdafx.h"

#undef TEST_OBJ_NUM
#define TEST_OBJ_NUM    1000000

fs::MemleakMonitor *_g_memMonitor = g_MemleakMonitor;

class TestObjPoolObj
{
    OBJ_POOL_CREATE(TestObjPoolObj, _objPoolHelper);
public:
    TestObjPoolObj() {}
    ~TestObjPoolObj() 
    {
        //std::cout << "Îö¹¹" << std::endl;
    }

private:
    Int64 _int64obj;
};

OBJ_POOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, 10240000)


class TestObjPoolObj2
{
public:
    TestObjPoolObj2() {}
    ~TestObjPoolObj2() {}

private:
    Int64 _int64obj;
};

class TestObjPool
{
public:
    static void Run()
    {
        g_Log->InitModule("TestObjPool");
/*        Int32 i = 0;*/
//         while(1)
//         {
//             auto ptr = new fs::FS_String;
//             ++i;
//             
//             g_MemleakMonitor->PrintMemleakInfo(typeid(fs::FS_String).name());
//         }
//         auto newStr = new fs::FS_String;
//         std::cout << "size of str:"<< sizeof(fs::FS_String) << std::endl;
//         g_MemleakMonitor->PrintMemleakInfo();
//         getchar();
         fs::Time timeNow1, timeNow2;
//         timeNow1.FlushTime();
//         for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
//         {
//              new TestObjPoolObj2;
//         }
//         timeNow2.FlushTime();
//         std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
// 
        timeNow1.FlushTime();
//         for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
//         {            
//             new TestObjPoolObj2;
//         }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
       // std::cout << "memleak:" << TestObjPoolObj::GetMemleakNum() << std::endl;

        // fs::ObjPoolHelper<TestObjPoolObj> charPool(TEST_OBJ_NUM);
         //auto alloctor = charPool._alloctor;
         timeNow1.FlushTime();
         for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
         {
             g_Log->net<TestObjPool>("hello worldasdfasefalksdflas;kdflasdfjaksldjfalskdjflasdkjfalskdjflaskdjflasdkjflaksjflaskdfjkl");
         }
             // new TestObjPoolObj;

          timeNow2.FlushTime();
          std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
// 
         g_MemleakMonitor->PrintObjPoolInfo(typeid(TestObjPoolObj).name());


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
