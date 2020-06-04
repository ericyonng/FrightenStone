#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once
#include "stdafx.h"

#undef TEST_OBJ_NUM
#define TEST_OBJ_NUM    1000000

// fs::MemleakMonitor *_g_memMonitor = g_MemleakMonitor;

class TestObjPoolObj
{
    OBJ_POOL_CREATE(TestObjPoolObj, _objPoolHelper);
public:
    TestObjPoolObj(Int64 i):_int64obj(i) {}
    ~TestObjPoolObj() 
    {
        //std::cout << "析构" << std::endl;
    }

private:
    Int64 _int64obj;
};

OBJ_POOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, 10000000)


class TestObjPoolObj2
{
public:
    TestObjPoolObj2(Int64 i):_int64obj(i) {}
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
        fs::FS_TlsUtil::CreateUtilTlsHandle();
        g_MemoryPool->InitPool();

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
        std::vector<TestObjPoolObj *> *vecObj = new std::vector<TestObjPoolObj *>;
        vecObj->resize(TEST_OBJ_NUM);
        timeNow1.FlushTime();
        auto &poolHelper = TestObjPoolObj::_objPoolHelper;
        auto poolAlloctor = poolHelper->_alloctor;
        auto sizeobj = sizeof(TestObjPoolObj);
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
           // std::lock_guard<std::mutex> lck(poolAlloctor->GetMtx());
            //TestObjPoolObj::New(i);
            //g_MemoryPool->Alloc(sizeobj);
            new TestObjPoolObj(i);
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

        timeNow1.FlushTime();
       
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
            new TestObjPoolObj2(i);
            //g_MemoryPool->Alloc(sizeobj);
        }
        timeNow2.FlushTime();
        std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;

        timeNow1.FlushTime();
        for(Int32 i = 0; i < TEST_OBJ_NUM; ++i)
        {
            //delete (*vecObj)[i];
        }
        timeNow2.FlushTime();
         std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
//  std::cout << "escape :" << (timeNow2 - timeNow1).GetTotalMicroSeconds() << std::endl;
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
          fs::MemleakMonitor::GetInstance()->PrintObjPoolInfo(fs::RTTIUtil::GetByType<TestObjPoolObj>());


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
