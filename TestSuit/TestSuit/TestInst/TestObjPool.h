#ifndef __Test_TestObjPool_H__
#define __Test_TestObjPool_H__
#pragma once

#include "stdafx.h"

class TestObjPoolObj
{
   OBJ_POOL_CREATE(_objPoolHelper)
public:
    TestObjPoolObj() {}
    virtual ~TestObjPoolObj() {}

private:
    // Int64 _cnt;
};

OBJPOOL_CREATE_IMPL(TestObjPoolObj, _objPoolHelper, 10240)

class TestObjPool
{
public:
    static void Run()
    {
        g_Log->InitModule("TestObjPool");
        TestObjPoolObj *newObj = new TestObjPoolObj;
        printf("%p %llu", newObj, sizeof(TestObjPoolObj));
    }
};
#endif
