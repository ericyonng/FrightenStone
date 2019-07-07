#ifndef __Test_TestMemoryPool_H__
#define __Test_TestMemoryPool_H__

#pragma once

#include "stdafx.h"

class TestObj
{
    MEM_POOL_CREATE(_memHelper)

};

MEMPOOL_CREATE_IMPL(TestObj, _memHelper)

class TestMemoryPool
{
public:
    static void Run()
    {
        g_Log->InitModule("main");
        g_MemoryPool->InitPool();
        printf("mem:[%p]log[%p]", g_MemoryPool, g_Log);

        TestObj *newObj = new TestObj;
        //g_MemoryPool->FinishPool();
        //g_Log->FinishModule();
    }
};

#endif
