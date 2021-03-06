/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : TestMemoryPool.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/07/15
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestMemoryPool_H__
#define __Test_TestMemoryPool_H__

#pragma once

#include "stdafx.h"

class TestMemPoolObj
{
    MEM_POOL_CREATE(_memHelper);
public:
    TestMemPoolObj()
    {

    }
    ~TestMemPoolObj()
    {
    }

    void Print()
    {
        std::cout << "int"<<_int << std::endl;
    }

public:
    Int64 _int;

};

// MEMPOOL_CREATE_IMPL(TestMemPoolObj, _memHelper)


class TestMemPoolObj2
{
public:
    TestMemPoolObj2()
    {

    }
    ~TestMemPoolObj2()
    {
    }

    void Print()
    {
        std::cout << "int" << _int << std::endl;
    }

public:
    Int64 _int;

};

class TestMemoryPool
{
public:
    static void Run()
    {
        //g_MemoryPool->InitPool();
        //printf("mem:[%p]log[%p]", g_MemoryPool, g_Log);

        printf("g_Log %p\n", g_Log);
        printf("g_MemoryPool %p\n", g_MemoryPool);
        g_Log->InitModule("main");

        fs::Time _time1, _time2;
        const size_t sizeOfObj = sizeof(TestMemPoolObj2);
       // auto pool = g_MemoryPool;
        _time1.FlushTime();
        for(auto i = 0; i < 1000000; ++i)
        {
            g_MemoryPool->Lock();
            g_MemoryPool->Alloc(sizeOfObj);
            g_MemoryPool->Unlock();
        }
        _time2.FlushTime();
        std::cout << "slice:" << (_time2 - _time1).GetTotalMicroSeconds() << std::endl;

        _time1.FlushTime();
        for(auto i = 0; i < 1000000; ++i)
        {
            new TestMemPoolObj2;
        }
        _time2.FlushTime();
        std::cout << "slice:" << (_time2 - _time1).GetTotalMicroSeconds() << std::endl;
    }
};

#endif
